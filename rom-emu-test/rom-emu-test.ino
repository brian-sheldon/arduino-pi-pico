
#include "fast.core1.h"

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

//
// core0 loop
//

// buffer0 is for core0 sprintf calls

char buffer0[100];

int lastAddr = 0xffff;
int fast = true;
int delayTime = 0;

void loop() {
  
  if ( fast ) {
    sprintf( buffer0, "mhz: %02.6f addr: %04x data: %02x", mhz, addr, data );
    Serial.println( buffer0 );
  }
  
  //
  // speed control, I started with using gp26, but it is defective on my board and quits working after a while,
  // not surprised as I discovered the analog function of these pins did not work a while ago.
  //

  if ( ( gpio_get_all() & gp27PinMask ) != 0 ) {
    delayTime = 0;
    fast = true;
  } else {
    delayTime = 500;
    fast = false;
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
      sprintf( buffer0, "mhz: %02.6f addr: %04x data: %02x", mhz, addr, data );
      Serial.println( buffer0 );
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

