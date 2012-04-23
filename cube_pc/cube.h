
#define AXIS_X 0x78
#define AXIS_Y 0x79
#define AXIS_Z 0x7a

#define CUBE_SIZE 8

int LED_PORT;
int LED_RED;
int LED_GREEN;

volatile unsigned char cube[8][8];
volatile unsigned char rs232_cube[8][8];
unsigned char fb[8][8];

int tty;

// FILE *ftty;

// func dsfs
void cube_putchar (unsigned char data);
int cube_init (void);
void cube_push (unsigned char data[8][8]);


