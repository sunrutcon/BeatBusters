
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <MIDI.h>
//#include "printf.h"


//MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midi_out);
MIDI_CREATE_DEFAULT_INSTANCE();
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

char  message_out[32]    = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
char  message_in[5][32]     = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
char  message_in_old[5][32] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

const char red = 0;
const char green = 1;
const char blue = 2;

uint8_t last_pipe_num = 0;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const uint64_t talking_pipes[5] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL };

//const uint64_t talking_pipes[5] = { 0xC2C2C2C2C2LL, 0xC2C2C2C2C3LL, 0xC2C2C2C2C4LL, 0xC2C2C2C2C5LL, 0xC2C2C2C2C6LL };

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role;

void setup(void)
{
  MIDI.begin();
  role = role_ping_out;

 //Serial.begin(31250);
  //Serial.begin(57600);
  //printf_begin();
//  printf("\n\rRF24/examples/pingpair/\n\r");
//  printf("ROLE: %s\n\r",role_friendly_name[role]);

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  //radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);

  //radio.openWritingPipe(pipes[0]);
  
  radio.openReadingPipe(1,talking_pipes[0]);
  radio.openReadingPipe(2,talking_pipes[1]);
  radio.openReadingPipe(3,talking_pipes[2]);
//  radio.openReadingPipe(4,talking_pipes[3]);
//  radio.openReadingPipe(5,talking_pipes[4]);
  
  radio.startListening();

  //radio.printDetails();
  for(int j=0; j<5; j++){
    for(int i=0; i<4; i++){
      message_in_old[j][i] = 0;
      message_in[j][i] = 0;
    }
  }
}

void loop(void)
{

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
  for(int pipe_num=0; pipe_num<3; pipe_num++){
    for(int i=0; i<4; i++)
      if(message_in[pipe_num][i] != message_in_old[pipe_num][i]){
        if(message_in[pipe_num][i]){ 
          // MIDI.sendNoteOn(47+i,127,2);
          noteOn(0x91,1+i+(last_pipe_num*5),127);
         }
         else { 
           //MIDI.sendNoteOff(42,0,2);
           noteOn(0x91,1+i+(last_pipe_num*5),0x00);
         }
         message_in_old[pipe_num][i] = message_in[pipe_num][i];
      }
  }
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
