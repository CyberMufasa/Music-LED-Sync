/************************************************************************
	main.c

    16-Bit Fast Hartley Transform - Arduino Uno test
	Main function
    Copyright (C) 2013 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com (please use the forum for questions)
	Forum: http://www.waitingforfriday.com/forum

************************************************************************/

// Include configuration and headers
#include "wff_fht/fhtConfig.h"

#include <stdio.h>
#include "usart.h"
#include "sampling.h"

// Define an array for storing the input data to the FHT
#ifndef GENERATE_TABLES
int16_t fx[FHT_LEN];
#endif

void drawGraph64(int16_t *fx, int16_t len)
{
	int16_t i;

	// Draw a scaled graph of the output (y resolution = 0-63)
	for (i = 0; i < len; i++)
	{
		int16_t y;
		printf("%3d |", i);
		for (y = 0; y < fx[i]; y++) printf("*");
		printf("\r\n");
	}
}

void runTests(int16_t frequency, int16_t amplitude)
{
	#ifndef GENERATE_TABLES
	printf("WFF_FHT Test: Pure sine wave with frequency = %d Hz and amplitude of +-%d\r\n", frequency, amplitude);
	printf("              FHT_LEN = %d, N_DB = %d\r\n\r\n", FHT_LEN, N_DB);

	// Test 1 - Rectangular window, linear output
	printf("Test 1 - Rectangular window, linear output\r\n");
	generateSample(fx, frequency, amplitude);
	fhtDitInt(fx);
	complexToReal(fx, 7);
	drawGraph64(fx, FHT_LEN/2);

	// Test 2 - Rectangular window, decibel output
	printf("\r\nTest 2 - Rectangular window, decibel output\r\n");
	generateSample(fx, frequency, amplitude);
	fhtDitInt(fx);
	complexToDecibel(fx);
	drawGraph64(fx, FHT_LEN/2);

	// Test 3 - Hamming window, linear output
	printf("\r\nTest 3 - Hamming window, linear output\r\n");
	generateSample(fx, frequency, amplitude);
	applyHammingWindow(fx);
	fhtDitInt(fx);
	complexToReal(fx, 7);
	drawGraph64(fx, FHT_LEN/2);

	// Test 4 - Hamming window, decibel output with gain
	printf("\r\nTest 4 - Hamming window, decibel output with gain\r\n");
	generateSample(fx, frequency, amplitude);
	applyHammingWindow(fx);
	fhtDitInt(fx);
	complexToDecibelWithGain(fx);
	drawGraph64(fx, FHT_LEN/2);

	// Test 5 - Hann window, linear output
	printf("\r\nTest 5 - Hann window, linear output\r\n");
	generateSample(fx, frequency, amplitude);
	applyHannWindow(fx);
	fhtDitInt(fx);
	complexToReal(fx, 7);
	drawGraph64(fx, FHT_LEN/2);

	// Test 6 - Hann window, decibel output
	printf("\r\nTest 6 - Hann window, decibel output with gain\r\n");
	generateSample(fx, frequency, amplitude);
	applyHannWindow(fx);
	fhtDitInt(fx);
	complexToDecibelWithGain(fx);
	drawGraph64(fx, FHT_LEN/2);

	// All done
	printf("\r\nTests completed...\r\n\r\n");
	#endif
}

int main(void)
{
	setupUsart();
	
	printf("www.waitingforfriday.com - FHT Library automatic testing (ATmega328P Arduino Uno Board) - (c)2013 Simon Inns\r\n");
	printf("FHT library version %d_%d\r\n\r\n", FHT_MAJOR_VERSION, FHT_MINOR_VERSION);
	
	#ifdef GENERATETABLES_H_
	printf("Generating the pre-calculated tables:\r\n");
	generateTables();
	#else
	printf("\r\nRunning automated testing:\r\n");

	// Run the tests...
	runTests(2500, 16383);
	runTests(5000, 16383);
	runTests(7500, 16383);
	runTests(1990, 16383);
	runTests(9490, 16383);
	#endif

	return 0;
}