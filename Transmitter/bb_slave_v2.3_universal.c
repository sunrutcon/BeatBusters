#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define STATIC_ADDRESS 1
#define STATIC_DEVICE_NO 2

RF24 radio(9,10);

unsigned long last_millis = millis();

int switch_pin[4] = {5,6,7,8};
int config_switch[2] = {2,3};

char  message_out[8] = {0,0,0,0,0,0,0,0};
char  message_in[8] = {0,0,0,0,0,0,0,0};

/*
#0  0xF0F0F0F0D2LL 
#1  0xF0F0F0F0C3LL 
#2  0xF0F0F0F0B4LL 
#3  0xF0F0F0F0A5LL 
#4  0xF0F0F0F096LL
*/

const uint64_t slave_address[5] = {0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL};

int device_no = STATIC_DEVICE_NO;
int new_device_no = STATIC_DEVICE_NO;

void setup(void)
{ 
  /* setup switch pins */
  for(int i=0; i<4; i++){
    pinMode(switch_pin[i], INPUT_PULLUP);
  }

  /* setup config switch pins */
  for(int i=0; i<2; i++){
    pinMode(config_switch[i], INPUT_PULLUP);
  }

  if(STATIC_ADDRESS!=1)
    device_no = digitalRead(config_switch[0]) | digitalRead(config_switch[1])<<1;
    
  Serial.begin(57600);
  printf_begin();

  radio.begin();

  radio.setRetries(1,1);

  radio.setPayloadSize(8);
  
  radio.openWritingPipe(slave_address[device_no]);

  radio.printDetails();

  Serial.print("\n\nRadio number #");
  Serial.println(device_no);
}

void loop(void)
{
  check_switches();
  write_message();
  delay(1);
}

void write_message() {
  if(new_device_no != device_no) {
    device_no = new_device_no;
    radio.openWritingPipe(slave_address[device_no]);
  }
  if(device_no==0)
    check_radio_0();
  else if(device_no==1)
    check_radio_1();
  else if(device_no==2)
    check_radio_2();
}

//radio #1
void check_radio_1(){
  //radio.stopListening();
  radio.write( &message_out, 8 );
  delayMicroseconds(10);
  radio.write( &message_out, 8 );
  delayMicroseconds(10);
  radio.write( &message_out, 8 );
  delayMicroseconds(10);
  radio.write( &message_out, 8 );
  delayMicroseconds(10);
  radio.write( &message_out, 8 );
  delayMicroseconds(800);
  //radio.startListening();
}

//radio #0
void check_radio_0(){
  //radio.stopListening();
  radio.write( &message_out, 8 );
  delayMicroseconds(123);
  radio.write( &message_out, 8 );
  delayMicroseconds(123);
  radio.write( &message_out, 8 );
  delayMicroseconds(800);
  //radio.startListening();
}

//radio #2
void check_radio_2(){
  //radio.stopListening();
  radio.write( &message_out, 8 );
  delayMicroseconds(323);
  radio.write( &message_out, 8 );
  delayMicroseconds(323);
  radio.write( &message_out, 8 );
  delayMicroseconds(323);
  //radio.startListening();
}

void check_switches(){
  //check sensors
  for(int i=0; i<4; i++){
    message_out[i] = digitalRead(switch_pin[i]);
    if(message_out[i] == 0){
     Serial.print("pressed ");
     Serial.println(i);
    }
  }

  //check config switches
  if(STATIC_ADDRESS!=1)
    new_device_no = digitalRead(config_switch[0]) | digitalRead(config_switch[1])<<1;
}
