#define F_CPU 16000000
#define __AVR_ATmega2560__
#ifndef __AVR_ATmega2560__
#endif
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)
#define TX_BUFFER_SIZE 256
//#define FFT_N	64	// redefine for smaller samples
#include <stdio.h>
#include <stdlib.h> // the header of the general-purpose standard library of C programming language
#include <avr/io.h> // the header of the I/O port library
#include <util/delay_basic.h> // header of the delay functions
#include <util/delay.h>
#include "lcd_test.h"
#include "lcd.h"
#include "ffft.h"
//#include "test3.c"
#include "1000hz_1.c"
#include <avr/interrupt.h> // header for interrupts

volatile unsigned int ADC_result;
volatile unsigned int ADC_result_flag;
volatile unsigned int input_count;

char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;

void appendSerial(char c);
void serialWrite(char  c[]);
void ADCsetup(void);
void swapADCchannels(void);
void rawADCRead(uint16_t adc_read);
void capture_wave (int16_t *buffer, uint16_t count);
void captureInput(int16_t *buffer);

int16_t capture[FFT_N]; /* Wave captureing buffer */ 
complex_t bfly_buff[FFT_N]; /* FFT buffer */ 
uint16_t spektrum[FFT_N/2]; /* Spectrum output buffer */
uint16_t t1,t2,t3;

char terminal_read[10];
char done_FFT[10];

#define ADCpin1 0x61

void mTimer (int count)
{
   /***
      Setup Timer1 as a ms timer
	  Using polling method not Interrupt Driven
   ***/
   sei();
   int i;

   i = 0;

   TCCR1B |= _BV (CS11);  // Set prescaler (/8) clock 16MHz/8 -> 2MHz
   /* Set the Waveform gen. mode bit description to clear
     on compare mode only */
   TCCR1B |= _BV(WGM12);

   /* Set output compare register for 2000 cycles, 1ms */
   OCR1A = 0x07D0;
 
   /* Initialize Timer1 to zero */
   TCNT1 = 0x0000;

   /* Enable the output compare interrupt */
   //TIMSK1 |= _BV(OCIE1A);  //remove if global interrups is set (sei())

   /* Clear the Timer1 interrupt flag and begin timing */
   TIFR1 |= _BV(OCF1A);

   /* Poll the timer to determine when the timer has reached 1ms */
   while (i < count)
   {
      while ((TIFR1 & 0x02) != 0x02);
	
	   /* Clear the interrupt flag by WRITING a ONE to the bit */
	   TIFR1 |= _BV(OCF1A);
	   i++;
   } /* while */
   TCCR1B &= ~_BV (CS11);  //  disable prescalar, no Clock
   return;
}  /* mTimer */

void appendSerial(char c)
{
   serialBuffer[serialWritePos] = c;
   serialWritePos++;

   if(serialWritePos >= TX_BUFFER_SIZE)
   {
      serialWritePos = 0;
   }
}

void serialWrite(char c[])
{
   for(uint8_t i = 0; i < strlen(c); i++)
   {
      appendSerial(c[i]);
   }

   if(UCSR0A & (1 << UDRE0))
   {
      UDR0 = 0;
   }

}

int main(int argc, char *argv[])
{ 

   uint16_t temp;            
   cli();

  // ADCsetup();

   // UART communication setup
   UBRR0H = (BRC >> 8);
   UBRR0L = BRC;
   UCSR0B = (1<<TXEN0) | (1<< TXCIE0);
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

   sei();
   //ADCSRA |= _BV(ADSC); // Start ADC conversion
   
    while (1)
    {
      //mTimer(2500);
      //capture_wave(capture, FFT_N);
     // printf("%d",ADC_result);
    // printf('T');
      //rawADCRead(ADC_result);
      //captureInput(capture);
   //    capture_wave(capture, FFT_N);
   //   //rawADCRead(ADC_result);
   // //  if (ADC_result_flag)
   //   //{
   //    //rawADCRead(ADC_result);  
   //    //fft_input(testing_wav, bfly_buff);
   //    fft_input(capture, bfly_buff);
   //    fft_execute(bfly_buff);
   //    fft_output(bfly_buff, spektrum);


      capture_wave(capture, FFT_N);
      TCNT1 = 0;	/* performance counter */
      fft_input(capture, bfly_buff);
      t1 = TCNT1; TCNT1 = 0;
      fft_execute(bfly_buff);
      t2 = TCNT1; TCNT1 = 0;
      fft_output(bfly_buff, spektrum);
      t3 = TCNT1;
      
      serialWrite("Start\n\r");
      for (uint16_t i=0;i<FFT_N/2;i++)
      {
         temp = spektrum[i];
         sprintf(terminal_read,"%u",temp);
         serialWrite(terminal_read);
         serialWrite(" ");
       //  mTimer(250);   
      }
      serialWrite("\n\n");
      //rawADCRead(ADC_result);  
      //ADC_result_flag = 0x00;
      //sprintf(done_FFT, "%u",
      serialWrite("DONE\n\r");
      mTimer(2500);
      //ADCSRA |= _BV(ADSC); // Start another ADC conversion
     // mTimer(1000);
     }
 //   }
    return (0); 
}

// ISR(ADC_vect)
// {
//    ADC_result = ADCH;
//    ADC_result_flag = 1;
// }

ISR(USART0_TX_vect)
{
   if(serialReadPos != serialWritePos)
   {
      UDR0 = serialBuffer[serialReadPos];
      serialReadPos++;

      if(serialReadPos >= TX_BUFFER_SIZE)
      {
         serialReadPos++;
      }
   }
}

void ADCsetup()
{
	ADCSRA |= _BV(ADEN); // enable ADC
	ADCSRA |= _BV(ADIE); // enable interrupt of ADC
	ADMUX |= _BV(ADLAR) |_BV(REFS0) | _BV(MUX0); // Select AVCC as voltage reference, set ADC data register to left adjust result, and select analog channel 1
}

void swapADCchannels()
{
   // Turn off analog channel 1 by inverting the on bit
   // Turn on analog channel 0 (set all MUXx to 0 to default to channel 0)
  ADMUX ^= _BV(MUX0); 
}

void rawADCRead(uint16_t adc_read)
{
   char adc_msg[10];
   uint8_t t = 0;
 //  uint16_t new_val;
   //new_val = ((int16_t)(ADC_result)-127)*4;
   //new_val =  ADC - 32768;
   //itoa(adc_read, adc_msg, 10);
   //sprintf(adc_msg,"%u",new_val);
  sprintf(adc_msg,"%u",adc_read);
 // serialWrite(adc_msg);
   while(adc_msg[t] != '\0') 
  { 
     UDR0 = adc_msg[t];
    t++; 
  } 
  serialWrite("\n\n");
  //UDR1 = adc_msg;
  //UDR0 = adc_msg;
}

void captureInput(int16_t *buffer)
{
   uint16_t offset = 0;
   while (offset != FFT_N)
   {
   //sprintf(terminal_read,"%u",ADC_result);
   //serialWrite("Offset");
   UDR0 = offset;
   buffer[offset] = ((int16_t)(ADC_result)-127)*4; // convert to signed linear and multiplied by 4
   offset++;
   }
}

// Taken from other code
// void capture_wave (int16_t *buffer, uint16_t count)
// {
//   //  ADMUX = _BV(REFS0)|_BV(ADLAR)| _BV(MUX0);;    // channel

//     do {
//         ADCSRA = _BV(ADEN)|_BV(ADSC)|_BV(ADATE)|_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
//         while(bit_is_clear(ADCSRA, ADIF));
//         *buffer++ = ADC - 32768;
//     } while(--count);

//     ADCSRA = 0;
// }


void capture_wave (int16_t *buffer, uint16_t count)
{
	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX0);	// channel

	do {
		ADCSRA = _BV(ADEN)|_BV(ADSC) |_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1);
		while(bit_is_clear(ADCSRA, ADIF));
		*buffer++ = ADC - 32768;
	} while(--count);

	ADCSRA = 0;
}


// void TransmitData( unsigned char data )
// {
//    /* Wait for empty buffer */
//    while ( !( UCSR1A & (1<<UDRE)) );
//    /* Put data into buffer, send the data */
//    UDR1 = data;
// }