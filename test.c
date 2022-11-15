#define F_CPU 16000000
#define __AVR_ATmega2560__
#ifndef __AVR_ATmega2560__
#endif
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)
#define TX_BUFFER_SIZE 128
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

char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;

void appendSerial(char c);
void serialWrite(char  c[]);
void ADCsetup(void);
void swapADCchannels(void);
void rawADCRead(uint16_t adc_read);

int16_t capture[FFT_N]; /* Wave captureing buffer */ 
complex_t bfly_buff[FFT_N]; /* FFT buffer */ 
uint16_t spektrum[FFT_N/2]; /* Spectrum output buffer */

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

   ADCsetup();

   // UART communication setup
   UBRR0H = (BRC >> 8);
   UBRR0L = BRC;
   UCSR0B = (1<<TXEN0) | (1<< TXCIE0);
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

   sei();
   ADCSRA |= _BV(ADSC); // Start ADC conversion
   
    while (1)
    {
      mTimer(1000);
      if (ADC_result_flag)
     {
      //fft_input(testing_wav, bfly_buff);
      // fft_input(ADC_result, bfly_buff);
      // fft_execute(bfly_buff);
      // fft_output(bfly_buff, spektrum);
      
      serialWrite("Start\n\r");
      // for (uint8_t i=0;i<FFT_N/2;i++)
      // {
      //    temp = spektrum[i];
      //    sprintf(terminal_read,"%u",temp);
      //    serialWrite(terminal_read);
      //    serialWrite("\n\r");
      //    mTimer(250);   
      // }
      rawADCRead(ADC_result);  
      ADC_result_flag = 0x00;
      //sprintf(done_FFT, "%u",
      serialWrite("DONE\n\r");
      ADCSRA |= _BV(ADSC); // Start another ADC conversion
      mTimer(1000);
     }
    }
    return (0); 
}

ISR(ADC_vect)
{
   ADC_result = ADCH;
   ADC_result_flag = 1;
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
   sprintf(adc_msg,"%u",adc_read);
   serialWrite(adc_msg);
}