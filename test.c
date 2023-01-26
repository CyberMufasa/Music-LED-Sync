#define F_CPU 16000000
// #define	SYSCLK		16000000
#define __AVR_ATmega2560__
#ifndef __AVR_ATmega2560__
#endif
#define BUAD 9600
#define BRC ((F_CPU / 16 / BUAD) - 1)
#define TX_BUFFER_SIZE 256
#include <stdio.h>
#include <stdlib.h>           // the header of the general-purpose standard library of C programming language
#include <avr/io.h>           // the header of the I/O port library
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
void serialWrite(char c[]);
void ADCsetup(void);
void swapADCchannels(void);
void capture_wave(int16_t *buffer, uint16_t count);
void captureInput(int16_t *buffer);
void set_frequncy(void);
void UART_display(char *display, int16_t input);
uint16_t *max_freq_val(uint16_t *input);
void turn_on_LED(uint16_t freq_bin);

int16_t capture[FFT_N];       /* Wave captureing buffer */
complex_t bfly_buff[FFT_N];   /* FFT buffer */
uint16_t spektrum[FFT_N / 2]; /* Spectrum output buffer */
uint16_t t1, t2, t3;

char terminal_read[16];
char terminal_read2[16];
char terminal_readmean[16];
char terminal_readbefore[16];
char terminal_readafter[16];
char terminal_MaxFreq[16];
uint16_t *array_MaxFreq;
char terminal_Maxindex[16];
char terminal_debug[16];
char done_FFT[10];

#define ADCpin1 0x61

void mTimer(int count)
{
   /***
      Setup Timer1 as a ms timer
     Using polling method not Interrupt Driven
   ***/
   sei();
   int i;

   i = 0;

   TCCR1B |= _BV(CS11); // Set prescaler (/8) clock 16MHz/8 -> 2MHz
   /* Set the Waveform gen. mode bit description to clear
     on compare mode only */
   TCCR1B |= _BV(WGM12);

   /* Set output compare register for 2000 cycles, 1ms */
   OCR1A = 0x07D0;

   /* Initialize Timer1 to zero */
   TCNT1 = 0x0000;

   /* Enable the output compare interrupt */
   // TIMSK1 |= _BV(OCIE1A);  //remove if global interrups is set (sei())

   /* Clear the Timer1 interrupt flag and begin timing */
   TIFR1 |= _BV(OCF1A);

   /* Poll the timer to determine when the timer has reached 1ms */
   while (i < count)
   {
      while ((TIFR1 & 0x02) != 0x02)
         ;

      /* Clear the interrupt flag by WRITING a ONE to the bit */
      TIFR1 |= _BV(OCF1A);
      i++;
   }                     /* while */
   TCCR1B &= ~_BV(CS11); //  disable prescalar, no Clock
   return;
} /* mTimer */

void appendSerial(char c)
{
   serialBuffer[serialWritePos] = c;
   serialWritePos++;

   if (serialWritePos >= TX_BUFFER_SIZE)
   {
      serialWritePos = 0;
   }
}

void serialWrite(char c[])
{
   for (uint8_t i = 0; i < strlen(c); i++)
   {
      appendSerial(c[i]);
   }

   if (UCSR0A & (1 << UDRE0))
   {
      UDR0 = 0;
   }
}

int main(int argc, char *argv[])
{

   uint16_t temp, temp2, m, i, max_freq;

   // DDRF = 0b00000010;	/* PE1:<conout>, PE0:<conin> in N81 38.4kbps */
   DDRF = 0b11111101;
   TCCR1B = 3; /* clk/64 */

   // Turn LEDs on as test
   DDRC = 0b00001110;
   PORTC = 0b00000000;
   // PORTC = 0b0000(Red LED)(Green LED)(Blue LED)0;

   cli();

   // UART communication setup
   UBRR0H = (BRC >> 8);
   UBRR0L = BRC;
   UCSR0B = (1 << TXEN0) | (1 << TXCIE0);
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

   sei();

   while (1)
   {

      capture_wave(capture, FFT_N);

      int mean = capture[0];
      for (i = 1; i < FFT_N; i++)
      {
         mean = capture[i];
         mean += (mean / 10000);
      }
      serialWrite("\n\r");
      serialWrite("Start\n\r");
      mean = (mean / FFT_N) * 10000;
      for (i = 0; i < FFT_N; i++)
      {
         capture[i] = capture[i] - mean;
      }

      fft_input(capture, bfly_buff);
      t1 = TCNT1;
      TCNT1 = 0;
      fft_execute(bfly_buff);
      t2 = TCNT1;
      TCNT1 = 0;
      fft_output(bfly_buff, spektrum);
      t3 = TCNT1;

      // Use only to connect with serial terminal
      // for (i = 0; i < FFT_N / 2; i++)
      // {

      //    temp = spektrum[i];
      //    //    mTimer(1);
      //    //   sprintf(terminal_read2,"%u",i);
      //    //   serialWrite(terminal_read2);
      //    // // serialWrite(i);
      //    //   // UDR0 = i;
      //    //    serialWrite(":");

      //    //  sprintf(terminal_read,"%u",temp);
      //    //  serialWrite(terminal_read);
      //    // serialWrite("%5u ", temp);
      //    //    serialWrite(" ");

      //    //    temp /= 512;
      //    // 	for (m = 0; m < temp; m++) UDR0 = '*';
      //    //  //  mTimer(250);
      // }

      array_MaxFreq = max_freq_val(spektrum);
      serialWrite("\n\n\rMax Freq 1: ");
      UART_display(terminal_MaxFreq, spektrum[array_MaxFreq[0]]);
      serialWrite("\n\rMax index 1: ");
      UART_display(terminal_Maxindex, array_MaxFreq[0]);
      serialWrite("\n\n\rMax Freq 2: ");
      UART_display(terminal_MaxFreq, spektrum[array_MaxFreq[1]]);
      serialWrite("\n\rMax index 2: ");
      UART_display(terminal_Maxindex, array_MaxFreq[1]);

      serialWrite("\n\r");
      turn_on_LED(array_MaxFreq[0]);

      serialWrite("\n\r");
      serialWrite("DONE\n\n\r");

      // Free array to relase memory and prevent memory leak
      free(array_MaxFreq);
      //  mTimer(50);
   }
   return (0);
}

// Configure terminal connections
ISR(USART0_TX_vect)
{
   if (serialReadPos != serialWritePos)
   {
      UDR0 = serialBuffer[serialReadPos];
      serialReadPos++;

      if (serialReadPos >= TX_BUFFER_SIZE)
      {
         serialReadPos++;
      }
   }
}

void ADCsetup()
{
   ADCSRA |= _BV(ADEN);                          // enable ADC
   ADCSRA |= _BV(ADIE);                          // enable interrupt of ADC
   ADMUX |= _BV(ADLAR) | _BV(REFS0) | _BV(MUX0); // Select AVCC as voltage reference, set ADC data register to left adjust result, and select analog channel 1
}

void swapADCchannels()
{
   // Turn off analog channel 1 by inverting the on bit
   // Turn on analog channel 0 (set all MUXx to 0 to default to channel 0)
   ADMUX ^= _BV(MUX0);
}

void captureInput(int16_t *buffer)
{
   uint16_t offset = 0;
   while (offset != FFT_N)
   {
      UDR0 = offset;
      buffer[offset] = ((int16_t)(ADC_result)-127) * 4; // convert to signed linear and multiplied by 4
      offset++;
   }
}

void capture_wave(int16_t *buffer, uint16_t count)
{
   // REFS0 - AVCC with external reference
   // ADLAR - Left adjust ADC result
   // MUX0 - Set ADC1 as input
   ADMUX = _BV(REFS0) | _BV(ADLAR) | _BV(MUX0); // channel

   do
   {
      // ADEN - Enable ADC
      // ADSC - Start ADC conversion
      // ADIF - ADC Interrupt flag, set when ADC completes, cleared by interrupt vector
      // ADPS2 & ADPS1 - 64 division factor
      ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADIF) | _BV(ADPS2) | _BV(ADPS1); //|_BV(ADPS0);
      // ADCSRB = ~_BV(ADTS2)| ~_BV(ADTS1)| ~_BV(ADTS0);
      while (bit_is_clear(ADCSRA, ADIF))
         ; // Loop in place while no ADC read is happening
           // ADC = ADC - 512;
           //*buffer++ = ADCH - 32768;
      //*buffer++ = ADCH - 65536;
      *buffer++ = ADCH;

      //*buffer++ = ADC; // Subtract integer datatype value
   } while (--count);

   ADCSRA = 0;
}

void set_frequency(int16_t *input)
{
   int16_t i = 0;
   char frequency[16];
   for (i; i < sizeof(input); i++)
   {
      if (input[i] > 5000)
         sprintf(frequency, "%u", "");
      serialWrite(frequency);
      serialWrite("High");
   }
}

void turn_on_LED(uint16_t freq_bin)
{
   if (freq_bin < 10)
   {
      serialWrite("LOW freq");
      PORTC = 0b00001000;
   }
   else if (freq_bin > 9 && freq_bin < 20)
   {
      serialWrite("Mid freq");
      PORTC = 0b00000100;
   }

   else
   {
      serialWrite("High freq");
      PORTC = 0b00000010;
   }
}

void UART_display(char *display, int16_t input)
{
   sprintf(display, "%u", input);
   serialWrite(display);
   serialWrite(" ");
}

uint16_t *max_freq_val(uint16_t *input)
{
   /*TODO:
   If a new value matches an existing value check the previous FFT array. If one of the new bins is a peak bin
   from a previous sample, assume that that same bin (frequency) is dominant and use it over the other bin.
   If they both were present in the old sampled FFT, then assume both are dominant frequnecies.
   */

   uint16_t max_val1 = input[2]; // Skip bins 0 and 1
   uint16_t max_val2 = 0;        // Skip bins 0 and 1
   uint16_t max_val3 = 0;        // Skip bins 0 and 1
   uint16_t max_index1 = 2;
   uint16_t max_index2 = 2;
   uint16_t max_index3 = 2;
   uint16_t *maxindex = malloc(2); // Must keep memory when done with loop

   // Iterate full array
   for (int i = 2; i < FFT_N / 2; i++)
   {
      if (max_val1 < input[i])
      {
         max_val3 = max_val2; // Replace second highest value with old max value
         max_index3 = max_index2;
         max_val2 = max_val1; // Replace 3rd highest value with old second max value
         max_index2 = max_index1;
         max_val1 = input[i];
         max_index1 = i;
      }
      else if (max_val2 < input[i])
      {
         max_val3 = max_val2; // Replace 3rd highest value with old second max value
         max_index3 = max_index2;
         max_val2 = input[i];
         max_index2 = i;
      }
      else if (max_val3 < input[i])
      {
         max_val3 = input[i];
         max_index3 = i;
      }
   }
   maxindex[0] = max_index1;
   maxindex[1] = max_index2;
   maxindex[2] = max_index3;
   return maxindex;
}