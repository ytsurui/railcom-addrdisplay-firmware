

#define rx_bufsize (36)
#define tx_bufsize (4)

#define UBRR_Value(b)       ((unsigned long)(F_CPU)-8*(unsigned long)b)/(16*(unsigned long)b)
//#define UBRR_Value(u)     (F_CPU/(16*(u+1)))
//#define UBRR_U2XValue(u)  (F_CPU/( 8*(u+1)))
#define UBRR_U2XValue(b)    ((unsigned long)F_CPU-4*(unsigned long)b)/( 8*(unsigned long)b)-1

void uart_putchar(char c);
//void uart_putstr(char *s);
//void uart_putstr_p(const prog_char *pgm_s);
uint8_t uart_getch(void);
uint8_t uart_rcv_size(void);
void init_uart(uint32_t baud);
//void uart_puthex(uint8_t);
//void uart_putdec(uint16_t data,uint8_t d);
