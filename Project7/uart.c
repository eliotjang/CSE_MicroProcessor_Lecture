// uart.c
#include <stdio.h> // for FILE
#include <avr/io.h> // FOR UBRR0H, UBRR0L.. I guess
#include <avr/interrupt.h> // for sei(), cli(), ISR()
#include <compat/deprecated.h>
#include <util/delay.h> // for sbi(), _delay_ms()
#include "uart.h"
#include "queue.h"
#define ETX 0x04
int uart_putchar(char ch, FILE *stream);

FILE Mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE Mystdin = FDEV_SETUP_STREAM (NULL, uart_getchar, _FDEV_SETUP_READ);
char volatile uart_busy; // for always access memory
// uart_busy가 int이던 char이던 상관없음.
void uart_init()
{
	stdin = &Mystdin; stdout = &Mystdout;
	uart_busy = 0;
	q_init();
	UBRR0H = 0x00; UBRR0L = 0x07; // 115.2K
	sbi(UCSR0A, U2X0);
	sbi(UCSR0B, TXEN0);
	sbi(UCSR0B, TXCIE0);
	sbi(UCSR0B, RXEN0);
	sbi(UCSR0B, RXCIE0);
}

ISR(USART0_TX_vect)
{
	char ch;
	if((ch = qo_delete()) == 0){
		uart_busy = 0;
	}
	else
		UDR0 = ch;
}

ISR(USART0_RX_vect)
{
	char ch;
	ch = UDR0;
	if (ch != ETX) {
		if (ch == '\r')
			ch = '\n';
		uart_echo(ch);
	qi_insert(ch);
}

int uart_putchar(char ch, FILE *stream)
{
	if(ch == '\n') uart_putchar('\r', stream);

	cli();
	if(!uart_busy){
		UDR0 = ch;
		uart_busy = 1;
	}
	else{
		while(qo_insert(ch) == 0){
			sei();
			_delay_us(100);
			cli();
		}
	}
	sei();
	
	return(1);
}

int uart_getchar(FILE *stream)
{
	char ch;
	do {
		cli();
		ch = qi_delete();
		sei();
	} while(ch == 0);
	
	if (ch == ETX) return(-1);
	else return(ch);
}

void uart_echo(char ch)
{
	if (ch == '\n') uart_echo('\r');
	if (!uart_busy) {
		UDR0 = ch;
		uart_busy = 1;
	}else
		qo_insert(ch);
}
