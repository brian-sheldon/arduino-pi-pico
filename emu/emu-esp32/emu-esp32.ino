
#include "M5Cardputer.h"

#include "emu.cmd.main.h"
#include "cli.h"

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  
  Serial.begin( 115200 );
  delay( 200 );
  
  Serial.println( "Setup ..." );
  
  M5.Display.setTextSize( 2 );
  M5.Display.print( "Starting ..." );

  setupFs();

  setupEmu();
  
  setupCmd();

  cliSetup();
}

void loop() {
  M5Cardputer.update();
  ioLoop();
  loopEmu();
  delay( 1 );
}

