

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

void setup() {
  Serial.begin( 115200 );
  delay( 200 );
  
  initRom();

  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( 26, INPUT_PULLUP );

  initPins( addrPins, 11, INPUT );
  initPins( ctrlPins, 3, INPUT );
  initPins( dataPins, 8, INPUT );
  pinMode( clockPin, OUTPUT );
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
  // Clock is high
  if ( digitalRead( mreqPin ) == 0 ) {
    addr = getAddr();
    if ( digitalRead( rdPin ) == 0 ) {
      if ( first_req ) {
        //
        // Read request
        //
        first_req = false;
        data = rom[addr];
        initPins( dataPins, 8, OUTPUT );
        putData( data );
        //sprintf( buffer, "Reading: %04x %02x", addr, data );
        //Serial.println( buffer );
      }
    } else if ( digitalRead( wrPin ) == 0 ) {
      if ( first_req ) {
        //
        // Write request
        //
        first_req = false;
        data = getData();
        rom[addr] = data;
        if ( ( writecount % 2000 ) == 0 ) {
          sprintf( buffer, "Mhz: %02.3f Writing: %04x %02x", mhz, addr, data );
          Serial.println( buffer );
        }
        writecount++;
      }
    }
  } else {
    if ( ! first_req ) {
      first_req = true;
      initPins( dataPins, 8, INPUT );
    }
  }
  // set clock low
  digitalWrite( clockPin, 0 );
  //digitalWrite( LED_BUILTIN, LOW );
  //if ( delayTime > 0 ) {
    delay( delayTime );
  //}
  // set clock high
  if ( ( count % 100 ) == 0 ) {
    if ( ( count % 10000 ) == 0 ) {
      unsigned long currTime = micros();
      unsigned long diffTime = currTime - lastTime;
      //Serial.println( currTime );
      if ( diffTime > 0.0 ) {
        mhz = 10000.0 / diffTime;
      }
      lastTime = currTime;
    }
    if ( digitalRead( 26 ) == 0 ) {
      delayTime = 0;
    } else {
      delayTime = 0;
    }
  }
  digitalWrite( clockPin, 1 );
  //digitalWrite( LED_BUILTIN, HIGH );
  //if ( delayTime > 0 ) {
    delay( delayTime );
  //}
  count++;
}

void oldloop() {
  //Serial.println( ++count );
  //romLoop();
}
