/************************************************************************
	usart.c

    16-Bit Fast Hartley Transform
	USART debug handler
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

// Include configuration
#include "wff_fht/fhtConfig.h"

#ifdef AVR_GCC
#ifdef PRINTF_DEBUG

#include "usart.h"

#define UART_BAUD_RATE  38400
#define UART_BAUD_REGISTERS  (F_CPU / 16 / UART_BAUD_RATE - 1)

FILE uart_str = FDEV_SETUP_STREAM(printChar, NULL, _FDEV_SETUP_RW);

int printChar(char character, FILE *stream)
{
	while ((UCSR0A & (1 << UDRE0)) == 0);

	UDR0 = character;

	return 0;
}

void setupUsart(void)
{
	/*Set baud rate */
	UBRR0H = UART_BAUD_REGISTERS >> 8;
	UBRR0L = UART_BAUD_REGISTERS;

	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	stdout = &uart_str;
}

#endif
#endif
