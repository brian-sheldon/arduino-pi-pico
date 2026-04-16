
#include "fast.core1.h"

void setup() {

  Serial.begin( 115200 );
  delay( 200 );
  
  Serial.println( "Setup ..." );
  
  pinMode( LED_BUILTIN, OUTPUT );
  digitalWrite( LED_BUILTIN, 1 );
  pinMode( 26, INPUT_PULLUP );
  
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

void loop() {
  
  sprintf( buffer0, "mhz: %02.6f addr: %04x data: %02x", mhz, addr, data );
  Serial.println( buffer0 );

  delay( 100 );

}

//
// loop1 - runs on the second core
//

// used for mhz calculation

unsigned long lastTime;

void loop1() {
  //
  // Run the processor for number of loops indicated by var loops
  //
  int loops = 1000000;  // 10,000 - 65537 : 1.717 Mhz, 66,000 - 1,000,000 : 1.785 Mhz, why roughly 66,000 and higher results in speed jump
  fast_core1_loop( loops );
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
