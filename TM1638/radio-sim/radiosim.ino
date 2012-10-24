#include <InvertedTM1638.h>
#include <TM1638.h>
#include <TM1640.h>
#include <TM16XX.h>
#include <TM16XXFonts.h>

#define STATE_FIND 0
#define STATE_FIRSTB 1
#define STATE_SECONDB 2
#define STATE_THIRDB 3
#define STATE_STRING 4

int button[8];
TM1638 module(12, 11, 10);
char display[2][10];
int curdisp;
int ss;
int dots[2];
int green[2];
int red[2];
 
void setup() {
  Serial.begin(9600);
  module.clearDisplay();
  module.setupDisplay(true, 0);
  char* name = "HELLO";
  module.setDisplayToString(name, 0b00000000);
  /*
  module.setLED(TM1638_COLOR_GREEN, 0);
  module.setLED(TM1638_COLOR_RED, 1);
  module.setLED(TM1638_COLOR_GREEN + TM1638_COLOR_RED, 2);
  */
}

void loop() {
  // read the state of the switch into a local variable:

  int reading = module.getButtons();
  for(int i = 0; i < 8; i++){
    if (reading & (1<<i)){
      button[i]++;
    } else {
      button[i] = 0;
    }
	// arduino sucks balls!
    
    if(button[i] == 50) {
		char command[5];
		sprintf(command, "%d", i);
		Serial.print(command);
    }
    if(button[i] == 70) {
      button[i] = 51;
    }
  }
  int incomingByte;
	static int cur = 0;
	// send data only when you receive data:
	while (Serial.available() > 0) {
		// read the incoming byte:
		incomingByte = Serial.read();
		switch (ss){
			case STATE_FIND:
				if(incomingByte == 0xFF)
					ss = STATE_FIRSTB;
				break;
			case STATE_FIRSTB:
				dots[curdisp] = incomingByte;
				ss = STATE_SECONDB;
				break;
			case STATE_SECONDB:
				red[curdisp] = incomingByte;
				ss = STATE_THIRDB;
				break;
			case STATE_THIRDB:
				green[curdisp] = incomingByte;
				ss = STATE_STRING;
				break;
			case STATE_STRING:
				display[curdisp][cur] = incomingByte;
				cur++;
				if(cur == 8){
					cur = 0;
					display[curdisp][8] = 0;
					module.setDisplayToString(display[curdisp], dots[curdisp]);
					module.setLEDs(0x0000);
					for(int i = 0; i<8;i++){
						int c = 0;
						if((red[curdisp] & (1 << i)))
							c |= TM1638_COLOR_RED;
						if((green[curdisp] & (1 << i)))
							c |= TM1638_COLOR_GREEN;
						if(c)
							module.setLED(c, i);
						
					}
					curdisp = curdisp ^ 1;
					ss = STATE_FIND;
				}
				break;
		}
		
	}
  
}
