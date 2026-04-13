

byte addrPins[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

byte ctrlPins[] = {
  11, 12, 13
};

byte dataPins[] = {
  14, 15, 16, 17, 18, 19, 20, 21
};

byte mreqPin = 13;
byte wrPin = 11;
byte rdPin = 12;
byte clockPin = 22;

void initPins( byte* pins, int len, int inout ) {
  for ( int i = 0; i < len; i++ ) {
    pinMode( pins[i], inout );
  }
}

int getAddr() {
  int v = 0;
  for ( int i = 10; i >= 0; i-- ) {
    int p = digitalRead( i );
    v = ( v << 1 ) + p;
  }
  return v;
}

int getData() {
  int v = 0;
  for ( int i = 7; i >= 0; i-- ) {
    v = ( v << 1 ) + digitalRead( dataPins[i] );
  }
  return v;
}

void putData( int v ) {
  for ( int i = 0; i < 8; i++ ) {
    digitalWrite( dataPins[i], v & 1 );
    v = v >> 1;
  }
}

byte rom[65536];

byte ops[] = {
  0x3e, 0x00,         // ld a,0
  0x21, 0x00, 0x01,   // ld hl,0x100
  0x77,               // ld (hl),a
  0x3c,               // inc a
  0xc3, 0x02, 0x00    // jp 2
};
void initRom() {
  int i;
  for ( int i = 0; i < 65536; i++ ) {
    if ( i < 10 ) {
      rom[i] = ops[i];
    } else {
      rom[i] = 0;
    }
  }
}

int addrPinMask, wrPinMask, rdPinMask, mreqPinMask;
int dataPinMask, clkPinMask;
int specPinMask, ledPinMask;

void setup() {
  Serial.begin( 115200 );
  delay( 200 );
  
  initRom();

  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( 26, INPUT_PULLUP );
  
  
  // pin masks    3322222222221111111111
  //              10987654321098765432109876543210
  addrPinMask = 0b00000000000000000000011111111111;  // gp0-10
  wrPinMask   = 0b00000000000000000000100000000000;  // gp11
  rdPinMask   = 0b00000000000000000001000000000000;  // gp12
  mreqPinMask = 0b00000000000000000010000000000000;  // gp13
  dataPinMask = 0b00000000001111111100000000000000;  // gp14-21
  clkPinMask  = 0b00000000010000000000000000000000;  // gp22
  specPinMask = 0b00000000100000000000000000000000;  // gp23 - must not be changed
  ledPinMask  = 0b00000010000000000000000000000000;  // gp25 - builtin led
  
  gpio_init_mask( addrPinMask );
  gpio_init_mask( wrPinMask );
  gpio_init_mask( rdPinMask );
  gpio_init_mask( mreqPinMask );
  gpio_init_mask( dataPinMask );
  gpio_init_mask( clkPinMask );
  
  gpio_set_dir_in_masked( addrPinMask );
  gpio_set_dir_in_masked( wrPinMask );
  gpio_set_dir_in_masked( rdPinMask );
  gpio_set_dir_in_masked( mreqPinMask );
  gpio_set_dir_in_masked( dataPinMask );  // initially start as inputs, but change to outputs during writes
  gpio_set_dir_out_masked( clkPinMask );

  //initPins( addrPins, 11, INPUT );
  //initPins( ctrlPins, 3, INPUT );
  //initPins( dataPins, 8, INPUT );
  //pinMode( clockPin, OUTPUT );
}

int clk = 0;
int addr = 0;
int data = 0;

int delayTime = 0;

char buffer[100];

unsigned long lastTime;
float mhz = 0;
int count = 0;
int writecount = 0;
bool first_req = true;

//
// loop
//

void loop() {
  int loops = 100000;  // 10,000 - 65537 : 1.717 Mhz, 66,000 - 1,000,000 : 1.785 Mhz, why roughly 66,000 and higher results in speed jump
  while ( loops-- > 0 ) {  // increases speed to 1.785 Mhz
    // Clock is high
    if ( ( gpio_get_all() & mreqPinMask ) == 0 ) {  // up to 1.163 mhz
      addr = gpio_get_all() & addrPinMask;  // using this instead of getAddr increased speed from 0.279 mhz to 0.439 mhz
      if ( ( gpio_get_all() & rdPinMask ) == 0 ) {  // up to 0.993 mhz
        if ( first_req ) {
          //
          // Read request
          //
          first_req = false;
          data = rom[addr];
          gpio_set_dir_out_masked( dataPinMask );  // using this instead of initPins in both places increased speed from 0.123 mhz to 0.208 mhz
          gpio_put_masked( dataPinMask, data << 14 );  // using this instead of putData increased speed from 0.208 mhz to 0.279 mhz
          //sprintf( buffer, "Reading: %04x %02x", addr, data );
          //Serial.println( buffer );
        }
      } else if ( ( gpio_get_all() & wrPinMask ) == 0 ) {  // up to 0.993 mhz
        if ( first_req ) {
          //
          // Write request
          //
          first_req = false;
          data = getData();
          rom[addr] = data;
          if ( ( writecount++ % 16000 ) == 0 ) {
            sprintf( buffer, "Mhz: %02.3f Writing: %04x %02x", mhz, addr, data );
            Serial.println( buffer );
          }
        }
      }
    } else {
      if ( ! first_req ) {
        first_req = true;
        gpio_set_dir_in_masked( dataPinMask );  // using this instead of initPins in both places increased speed from 0.123 mhz to 0.208 mhz
      }
    }
    //
    // set clock low
    //
    gpio_put_masked( clkPinMask, 0 );  // replacing digitalWrite in this and the below case increased speed from 0.439 mhz to 0.798 mhz
    //delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
    //
    // calculate the Mhz
    //
    if ( ( count++ % 1000000 ) == 0 ) {  // change count from 100,000 1,000,000 1.171 mhz
      unsigned long currTime = micros();
      unsigned long diffTime = currTime - lastTime;
      if ( diffTime > 0.0 ) {
        mhz = 1000000.0 / diffTime;
      }
      lastTime = currTime;
    }
    //
    // set clock high
    //
    gpio_put_masked( clkPinMask, clkPinMask );  // replacing digitalWrite in this and the above case increased speed from 0.439 mhz to 0.798 mhz
    //delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
  }
}

//
// this loop has both the original and new calls, kept to show the changes made
//

void fastslowloop() {
  // Clock is high
  if ( ( gpio_get_all() & mreqPinMask ) == 0 ) {  // up to 1.163 mhz
  //if ( digitalRead( mreqPin ) == 0 ) {
    addr = gpio_get_all() & addrPinMask;  // using this instead of getAddr increased speed from 0.279 mhz to 0.439 mhz
    //addr = getAddr();
    if ( ( gpio_get_all() & rdPinMask ) == 0 ) {  // up to 0.993 mhz
    //if ( digitalRead( rdPin ) == 0 ) {
      if ( first_req ) {
        //
        // Read request
        //
        first_req = false;
        data = rom[addr];
        gpio_set_dir_out_masked( dataPinMask );  // using this instead of initPins in both places increased speed from 0.123 mhz to 0.208 mhz
        //initPins( dataPins, 8, OUTPUT );
        gpio_put_masked( dataPinMask, data << 14 );  // using this instead of putData increased speed from 0.208 mhz to 0.279 mhz
        //putData( data );
        //sprintf( buffer, "Reading: %04x %02x", addr, data );
        //Serial.println( buffer );
      }
    } else if ( ( gpio_get_all() & wrPinMask ) == 0 ) {  // up to 0.993 mhz
    //} else if ( digitalRead( wrPin ) == 0 ) {
      if ( first_req ) {
        //
        // Write request
        //
        first_req = false;
        data = getData();
        rom[addr] = data;
        if ( ( writecount++ % 16000 ) == 0 ) {
          sprintf( buffer, "Mhz: %02.3f Writing: %04x %02x", mhz, addr, data );
          Serial.println( buffer );
        }
      }
    }
  } else {
    if ( ! first_req ) {
      first_req = true;
      gpio_set_dir_in_masked( dataPinMask );  // using this instead of initPins in both places increased speed from 0.123 mhz to 0.208 mhz
      //initPins( dataPins, 8, INPUT );
    }
  }
  // set clock low
  gpio_put_masked( clkPinMask, 0 );  // replacing digitalWrite in this and the below case increased speed from 0.439 mhz to 0.798 mhz
  //digitalWrite( clockPin, 0 );
  //digitalWrite( LED_BUILTIN, LOW );
  //if ( delayTime > 0 ) {
    //delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
  //}
  // set clock high
  //if ( ( count % 100 ) == 0 ) {
    if ( ( count % 1000000 ) == 0 ) {  // change count from 100,000 1,000,000 1.171 mhz
      unsigned long currTime = micros();
      unsigned long diffTime = currTime - lastTime;
      //Serial.println( currTime );
      if ( diffTime > 0.0 ) {
        mhz = 1000000.0 / diffTime;
      }
      lastTime = currTime;
    }
    /*
    if ( digitalRead( 26 ) == 0 ) {
      delayTime = 0;
    } else {
      delayTime = 0;
    }
    */
  //}
  gpio_put_masked( clkPinMask, clkPinMask );  // replacing digitalWrite in this and the above case increased speed from 0.439 mhz to 0.798 mhz
  //digitalWrite( clockPin, 1 );
  //digitalWrite( LED_BUILTIN, HIGH );
  //if ( delayTime > 0 ) {
    //delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
  //}
  count++;
}

