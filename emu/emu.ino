
#include "emu.h"
#include "cli.h"

void setup() {
  Serial.begin( 115200 );
  delay( 200 );
  
  Serial.println( "Setup ..." );

  setupEmu();
}

void loop() {
  ioLoop();
  loopEmu();
  delay( 1 );
}

