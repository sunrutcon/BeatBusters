#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <MIDI.h>

#define DEBUG 1

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

char  message_out[32]    = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
char  message_in[5][32]     = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
char  message_in_old[5][32] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

uint8_t last_pipe_num = 0;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const uint64_t talking_pipes[5] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL };

int led = 3;

int led_1 = A3;
int led_2 = A4;
int led_3 = A5;

void setup_leds () {
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);

  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, HIGH);
  digitalWrite(led_3, HIGH);
}

void setup(void) {
  setup_leds();
  pinMode(led, OUTPUT);
  
  // use this speed to debuh output messages on monitor, use noteOn1 to print notes, serial.print
  //Serial.begin(57600);
  //Serial.print("Start");
  
  // use speed 31250 for midi output noteOn with serial.write
  Serial.begin(31250);
  
  radio.begin();

  // improve reliability
  radio.setPayloadSize(8);
  
  radio.openReadingPipe(1,talking_pipes[0]);
  radio.openReadingPipe(2,talking_pipes[1]);
  radio.openReadingPipe(3,talking_pipes[2]);

  radio.startListening();
  
  for(int j=0; j<5; j++){
    for(int i=0; i<4; i++){
      message_in_old[j][i] = 0;
      message_in[j][i] = 0;
    }
  }
}

void loop(void) {
  handle_radio();
  //handle_serial();
  handle_midi();
}

void handle_radio(){
  uint8_t pipe_num;
  if(radio.available(&pipe_num)){
    radio.read( message_in[pipe_num-1], 8);
    for(int i=0; i<4; i++){
      if(message_in[pipe_num-1][i]) message_in[pipe_num-1][i] = 0; else message_in[pipe_num-1][i] = 1;
    }
    last_pipe_num = pipe_num-1;
  }
  delayMicroseconds(100);
}

void handle_midi(){
  static int sh_data = 0;
  for(int pipe_num=0; pipe_num<3; pipe_num++){
    for(int i=0; i<4; i++) {
      if(message_in[pipe_num][i] != message_in_old[pipe_num][i]){
        if(message_in[pipe_num][i]){
          noteOn(0x91,1+i+(last_pipe_num*5),127);
          sh_data|=(1<<pipe_num);
          write_leds(sh_data);
        }
        else { 
          noteOn(0x91,1+i+(last_pipe_num*5),0x00);
          sh_data&=~(1<<pipe_num);
          write_leds(sh_data);
        }
        message_in_old[pipe_num][i] = message_in[pipe_num][i];
      }
    }
  }
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

void noteOn1(int cmd, int pitch, int velocity) {
  Serial.print(cmd);
  Serial.print("\t");
  Serial.print(pitch);
  Serial.print("\t");
  Serial.println(velocity);
}

void write_leds(int sh_data) {
  digitalWrite(led_1, sh_data&1 ? LOW : HIGH);
  digitalWrite(led_2, sh_data&2 ? LOW : HIGH);
  digitalWrite(led_3, sh_data&4 ? LOW : HIGH);
}
