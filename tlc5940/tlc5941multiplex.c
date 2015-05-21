#include <avr/interrupt.h>
#include <stdlib.h>

#define F_CPU 20000000UL            // CPU clock
#define BAUD 9600                   // USART Baud rate
#define MYUBRR F_CPU/16/BAUD-1      // Divider for USART
#define FIELDS 4
#include <util/delay.h>

#define TLCPORT PORTC
#define DATAPORT PORTB
#define SCLK_PIN PC0
#define XLAT_PIN PC1
#define BLANK_PIN PC2
#define GSCLK_PIN PD6     // OC0A
#define MODE_PIN PC4
#define SIN_PIN PC5
#define TRUE 1
#define FALSE 0
#define INIT 0
#define R 1
#define G 2
#define B 3
#define L 4
#define E 5
#define D 6

static volatile uint8_t tlc5940_needpulse = FALSE;
static volatile uint8_t tlc5940_transferdone = FALSE;
static volatile uint8_t rs232mode = INIT;
static volatile uint16_t rs232byte = 0;
static volatile uint8_t field = 0;

volatile uint8_t frame[24*FIELDS*8];

void display(void);
void init(void);
void setLED(uint8_t,uint16_t);
void clear(void);


// If this is commented out, slow PWM mode is enabled for testing
#define FAST

void USART_Init( unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
    /* Set frame format: 8data, 1stop bit */
    UCSR0C = (1<<UMSEL00)|(3<<UCSZ00);
}

void usart_putc(unsigned char c) {
   // wait until UDR ready
    while ( !( UCSR0A & (1<<UDRE0)) )
        ;
    UDR0 = c;    // send character
}

void usart_puts (char *s) {
    //  loop until *s != NULL
    while (*s) {
        usart_putc(*s);
        s++;
    }
}

unsigned char USART_Receive( void )
{
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
    ;
    /* Get and return received data from buffer */
    return UDR0;
}


int main(){
    clear();
    init();
    USART_Init(MYUBRR);
    while(1){
        while (tlc5940_transferdone) {}
        TLCPORT &= ~_BV(MODE_PIN); // GS mode
        if(field == 4){
            field = 0;
            usart_putc('F');
        }
        for (uint8_t i=0;i<192;i++){
            DATAPORT = frame[i+192*field];
            TLCPORT |= _BV(SCLK_PIN);
            TLCPORT &= ~_BV(SCLK_PIN);
        }
        field++;
        tlc5940_transferdone = field;
    }
    return 0;
}

/*!
  Temporary method for setting global dot correction.
  It is initialized by default to 63.
 */
void
setGlobalDC(uint8_t dcval)
{
  while (tlc5940_transferdone) {}
  TLCPORT |= _BV(MODE_PIN);
  for (uint16_t i=0;i<192;i++){
            DATAPORT = 0b00000111;
            TLCPORT |= _BV(SCLK_PIN);
            TLCPORT &= ~_BV(SCLK_PIN);
  }

  TLCPORT |= _BV(XLAT_PIN);
  TLCPORT &= ~_BV(XLAT_PIN);
  TLCPORT &= ~_BV(MODE_PIN);

  tlc5940_needpulse = TRUE;
}

/*!
  Initialized drivers and starts interrupts.
 */
void
init()
{
  cli();

  // Set pins to output
  DDRC |= 
    _BV(BLANK_PIN) | _BV(XLAT_PIN) | _BV(SCLK_PIN) | 
    _BV(SIN_PIN) | _BV(GSCLK_PIN) | _BV(MODE_PIN);
    
  DDRB |= 0b00000111;       // SOUTx3
  DDRD |= 0b01111100;       // OC0A + FIELDS
 
  TLCPORT &= ~_BV(BLANK_PIN);   // blank everything until ready
  TLCPORT &= ~_BV(XLAT_PIN);
  TLCPORT &= ~_BV(SCLK_PIN);
  TLCPORT &= ~_BV(GSCLK_PIN);

  setGlobalDC(63); // Max intensity.

  // PWM timer
  TCCR0A = (_BV(WGM01) |   // CTC 
            _BV(COM0A0));   // toggle OC0A on match -> GSCLK
  TCCR0B = (_BV(CS00));      // No prescaler
#ifdef FAST
  OCR0A = 1;               // toggle every timer clock cycle -> 4 MHz
#else
  OCR0A = 32;               // toggle every 16 timer clock cycle -> 4/16 MHz
#endif
  TCNT0 = 0;

  // Latch timer
  TCCR1A = (_BV(WGM10));   // Fast PWM 8-bit
#ifdef FAST
  TCCR1B = (_BV(CS11) |    // /64 prescaler     =>
            _BV(CS10) |
#else
  TCCR1B = (_BV(CS12) |    // /1024 prescaler     =>
            _BV(CS10) |
#endif
            _BV(WGM12));   // Fast PWM 8-bit  =>  1/4096th of OC2A

  TIMSK1 = _BV(TOIE1);     // Enable overflow interrupt
  TCNT1 = 0;

  sei();
}


ISR(USART_RX_vect ){
    uint8_t indata = UDR0;
    switch(rs232mode){
        case INIT:
            if(indata == 'R'){
                rs232mode = R;
            }
            break;
        case R:
            if(indata == 'G'){
                rs232mode = G;
            } else {
                rs232mode = INIT;
            }
            break;
        case G:
            if(indata == 'B'){
                rs232mode = B;
            } else {
                rs232mode = INIT;
            }
            break;
        case B:
            if(indata == 'L'){
                rs232mode = L;
            } else {
                rs232mode = INIT;
            }
            break;
        case L:
            if(indata == 'E'){
                rs232mode = E;
            } else {
                rs232mode = INIT;
            }
            break;
        case E:
            if(indata == 'D'){
                rs232mode = D;
                rs232byte = 0;
            } else {
                rs232mode = INIT;
            }
            break;
        case D:
            frame[rs232byte++] = indata;
            if(rs232byte == 768){
                rs232mode = INIT;
            }
            break;
        
    }
}

/*!
  Overflow interrupt. Handles BLANK and latches on demand.
*/
ISR(TIMER1_OVF_vect)
{
  TLCPORT |= _BV(BLANK_PIN);

  // Stop timers
#ifdef FAST
  TCCR1B &= ~_BV(CS11);
#else
  TCCR1B &= ~_BV(CS12);
#endif

  // Latch only if new data is available
  if (tlc5940_transferdone) {
    TLCPORT |= _BV(XLAT_PIN); // latch
    TLCPORT &= ~_BV(XLAT_PIN);
    switch(tlc5940_transferdone){
        case 1:
            PORTD = 0b00000100;
            break;
        case 2:
            PORTD = 0b00001000;
            break;
        case 3:
            PORTD = 0b00010000;
            break;
        case 4:
            PORTD = 0b00100000;
            break;
    }
    tlc5940_transferdone = FALSE;
    
    // Extra SCLK pulse according to Datasheet p.18
    if (tlc5940_needpulse) {
      TLCPORT |= _BV(SCLK_PIN);
      TLCPORT &= ~_BV(SCLK_PIN);
      tlc5940_needpulse = FALSE;
    } 
  }
  TLCPORT &= ~_BV(BLANK_PIN);

  // Restart timers
  TCNT1 = 0;
#ifdef FAST
  TCCR1B |= _BV(CS11);
#else
  TCCR1B |= _BV(CS12);
#endif
  
}


void clear()
{
  for (uint16_t i=0;i<24*FIELDS*8;i++) {
    //frame[i] = 0b00000000;
    frame[i] = 0b11111111;
  }
}

