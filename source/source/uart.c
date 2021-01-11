#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "uart.h"

typedef struct
{
    char buf[tx_bufsize];  /*���M�o�b�t�@�T�C�Y*/
    uint8_t buf_in;     /* �o�b�t�@�ɕ������������ނƂ��̈ʒu*/
    uint8_t buf_size;   /*�o�b�t�@�ɒ��߂��Ă��镶���� */
} TxBuf_t;

typedef struct
{
    char buf[rx_bufsize];
    uint8_t buf_in;
	uint8_t buf_size;
} RxBuf_t;

volatile TxBuf_t TxBuf;
volatile RxBuf_t RxBuf;

/*
void uart_putchar(char c)
{
    while (TxBuf.buf_size > (tx_bufsize-1)) ;  // �o�b�t�@���󂭂܂ő҂� 
    cli(); //�o�b�t�@������������i�ǂݏo�����܂�)����͕K�����荞�݋֎~��!
    TxBuf.buf[TxBuf.buf_in++] = c;
    TxBuf.buf_size++;
    if (TxBuf.buf_in==tx_bufsize) TxBuf.buf_in=0;
    UCSRB = UCSRB | _BV(UDRIE); // UDR�󂫊��荞�݂�L���ɂ���B
    sei();
}
*/

uint8_t uart_getch(void)
{
    char c;
    uint8_t buf_out;
    while (!RxBuf.buf_size) ;
    cli();
    buf_out=RxBuf.buf_in-RxBuf.buf_size;
    if (RxBuf.buf_in<RxBuf.buf_size) {
        buf_out += rx_bufsize;
    }
    c = RxBuf.buf[buf_out];
    if (RxBuf.buf_size==0) {
        RxBuf.buf_size += rx_bufsize;
    }
    RxBuf.buf_size--;
    sei();
    return c;
}

uint8_t uart_rcv_size(void){
return RxBuf.buf_size;
}

void init_uart(uint32_t baud){
	UCSRA = 0b00000000;
	UCSRB = _BV(TXEN)|_BV(RXCIE)|_BV(RXEN);
	UBRRH=UBRR_Value(baud) >> 8;
	UBRRL=UBRR_Value(baud) &  0xFF;
	RxBuf.buf_in = RxBuf.buf_size = 0;
	TxBuf.buf_size = 0;     // �o�b�t�@����ɂ���
}


ISR(USART_UDRE_vect){
    uint8_t buf_out;
    buf_out=TxBuf.buf_in-TxBuf.buf_size;
    if (TxBuf.buf_in<TxBuf.buf_size) buf_out += tx_bufsize;
    UDR = TxBuf.buf[buf_out];
    TxBuf.buf_size--;
    if (TxBuf.buf_size==0) UCSRB = UCSRB & ~_BV(UDRIE);
}

ISR(USART_RX_vect){
	//loconet_active();
     uint8_t c;
     c = UDR;
     if (RxBuf.buf_size>=rx_bufsize) {
	 	 RxBuf.buf_size = 0;
         return; 
         //�t���[����Ȃ�
     }
     RxBuf.buf[RxBuf.buf_in++]=c;
     if (RxBuf.buf_in>=rx_bufsize) RxBuf.buf_in=0;
     RxBuf.buf_size++;
}

