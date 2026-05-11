
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
  
  M5Cardputer.Display.setRotation(1); // Landscape
  M5Cardputer.Display.fillScreen(BLACK);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.setTextColor(GREEN);
  M5Cardputer.Display.setTextSize( 2 );
  M5Cardputer.Display.setTextScroll(true);
  M5.Display.println( "Starting ..." );

  Serial.begin( 115200 );
  delay( 2000 );
  
  Serial.println( "Starting ..." );
  
  setupFs();

  setupEmu();
  
  setupCmd();

  cliSetup();
}

void loop() {
  M5Cardputer.update();
  
  ioLoop();
  //loopEmu();
  cpu_frame();
  delay( 1 );
}

