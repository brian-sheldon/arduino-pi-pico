
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "fast.core1.h"

int input( int port ) {
  port = port & 0xff;
  return 0xff;
}

int outputCount = 0;
void output( int port, int data ) {
  port = port & 0xff;
  /*
  if ( ( outputCount++ % 82233 ) == 0 ) {
    Serial.print( "output port: " );
    Serial.print( port );
    Serial.print( " data: " );
    Serial.println( data );
  }
  */
}

void setup() {

  Serial.begin( 115200 );
  delay( 200 );
  
  Serial.println( "Setup ..." );
  
  pinMode( LED_BUILTIN, OUTPUT );
  digitalWrite( LED_BUILTIN, 1 );
  
  gpio_init_mask( gp27PinMask );
  gpio_set_dir_in_masked( gp27PinMask );
  pinMode( 27, INPUT_PULLUP );  // Used to control speed
  
}

//
// core1 setup
//

void setup1() {
  
  fast_core1_setup();

}

char buffer0[100];

void printState() {
  int address = addr;
  if ( reqId == 2 || reqId == 4 ) {
    address = addr & 0xff;
  }
  sprintf( buffer0, "mhz: %02.6f req: %d addr: %04x data: %02x", mhz, reqId, address, data );
  Serial.println( buffer0 );
}

//
// core0 loop
//

int lastAddr = 0xffff;
int fast = true;
int delayTime = 0;

void loop() {
  
  if ( fast ) {
    printState();
  }
  
  //
  // speed control, I started with using gp26, but it is defective on my board and quits working after a while,
  // not surprised as I discovered the analog function of these pins did not work a while ago.
  //

  if ( ( gpio_get_all() & gp27PinMask ) != 0 ) {
    if ( ! fast ) {
      Serial.println( "fast ..." );
      delayTime = 0;
      fast = true;
    }
  } else {
    if ( fast ) {
      Serial.println( "slow ..." );
      delayTime = 500;
      fast = false;
    }
  }

  delay( 100 );

}

//
// loop1 - runs on the second core
//

// used for mhz calculation

unsigned long lastTime;

void variable_core1_loop( int loops, int delayTime );

void loop1() {
  //
  // Run the processor for number of loops indicated by var loops
  //
  int loops;
  //
  // Note:  After a bit of time running, gpio 26 cannot be read.
  // This is used for speed control.  The default is now fast,
  // so if it fails it will run in fast mode.
  //
  if ( fast ) {
    loops = 1000000;  // 10,000 - 65537 : 1.717 Mhz, 66,000 - 1,000,000 : 1.785 Mhz, why roughly 66,000 and higher results in speed jump
    fast_core1_loop( loops );
  } else {
    loops = 1;
    fast_core1_loop( loops );
    if ( addr != lastAddr ) {
      printState();
      lastAddr = addr;
    }
    delay( delayTime );
  }
  //
  // calculate the Mhz
  //
  unsigned long currTime = micros();
  unsigned long diffTime = currTime - lastTime;
  if ( diffTime > 0.0 ) {
    mhz = 1.0 * loops / diffTime;
  }
  lastTime = currTime;
}

