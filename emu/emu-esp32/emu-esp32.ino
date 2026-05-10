
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "M5Cardputer.h"

#include "emu.cfg.h"
#include "emu.util.h"
#include "emu.state.h"
#include "emu.cmd.main.h"
#include "cli.h"

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  
  Serial.begin( 115200 );
  delay( 2000 );
  
  Serial.println( "Starting ..." );
  
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

