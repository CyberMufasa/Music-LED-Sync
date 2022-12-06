#define F_CPU 16000000
//#define	SYSCLK		16000000
#define __AVR_ATmega2560__
#ifndef __AVR_ATmega2560__
#endif
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)
#define TX_BUFFER_SIZE 256
#include <stdio.h>
#include <stdlib.h> // the header of the general-purpose standard library of C programming language
#include <avr/io.h> // the header of the I/O port library
#include <util/delay_basic.h> // header of the delay functions
#include <util/delay.h>
#include "lcd_test.h"
#include "lcd.h"
#include "ffft.h"
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

char terminal_read[16];
char terminal_read2[16];
char terminal_readmean[16];
char terminal_readbefore[16];
char terminal_readafter[16];
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

   uint16_t temp, temp2, m, i;            
   cli();

   // UART communication setup
   UBRR0H = (BRC >> 8);
   UBRR0L = BRC;
   UCSR0B = (1<<TXEN0) | (1<< TXCIE0);
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

   sei();
   
    while (1)
    {
       
      capture_wave(capture, FFT_N);

    int mean = capture[0]; 
      for (i =1 ; i< FFT_N; i++)
      {
         mean = capture[i];
         mean += (mean/10000);
         mTimer(10);
        // sprintf(terminal_readbefore,"%u",capture[i]);
       //  serialWrite(terminal_readbefore);
       //  serialWrite(" ");
   
     }
      serialWrite("Start\n\r");
     mean=(mean/FFT_N)*10000;
      for (i =0 ; i<  FFT_N; i++)
      {
         capture[i]=capture[i] - mean;
      }

      fft_input(capture, bfly_buff);
      t1 = TCNT1; TCNT1 = 0;
      fft_execute(bfly_buff);
      t2 = TCNT1; TCNT1 = 0;
      fft_output(bfly_buff, spektrum);
      t3 = TCNT1;

      for (i=0;i<FFT_N/2;i++)
      {


         temp = spektrum[i];
         mTimer(10);
        sprintf(terminal_read2,"%u",i);
        serialWrite(terminal_read2);
      // serialWrite(i);
        // UDR0 = i;
         serialWrite(":");
         
         sprintf(terminal_read,"%u",temp);
         serialWrite(terminal_read);
      //serialWrite("%5u ", temp);
         serialWrite(" ");
         
      //    temp /= 512;
		// 	for (m = 0; m < temp; m++) UDR0 = '*';
      //  //  mTimer(250);  

       }
      serialWrite("\n\r");
      serialWrite("DONE\n\n\r");
     }
    return (0); 
}

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

void capture_wave (int16_t *buffer, uint16_t count)
{
   // REFS0 - AVCC with external reference
   // ADLAR - Left adjust ADC result
   // MUX0 - Set ADC1 as input
	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX0);	// channel

	do {
      // ADEN - Enable ADC
      // ADSC - Start ADC conversion
      // ADIF - ADC Interrupt flag, set when ADC completes, cleared by interrupt vector
      // ADPS2 & ADPS1 - 64 division factor 
		ADCSRA = _BV(ADEN)|_BV(ADSC) |_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1);//|_BV(ADPS0);
     // ADCSRB = ~_BV(ADTS2)| ~_BV(ADTS1)| ~_BV(ADTS0);
		while(bit_is_clear(ADCSRA, ADIF)); // Loop in place while no ADC read is happening
     // ADC = ADC - 512;
		*buffer++ = ADC - 32768; 
      //*buffer++ = ADC; // Subtract integer datatype value
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