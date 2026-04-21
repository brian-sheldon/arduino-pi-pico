
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

//
// ram/rom buffer, technically only a rom as I have no write protection as
// it would slow down loop.  Will replace speed with functionality once the
// code is sufficiently clean and efficient.  Mind you, for write protection
// external logic would avoid any slow downs.  This is only for testing, so
// it is more an exercise in discovering code efficiency techniques.  To do
// this long term, the pios would probably be a better solution.
//
// Thoughts on improving this, are that the next steps would be to one, try
// accessing memory directly to avoid the function calls.  Next would be to
// consider imbedding assembly within the code.  At which point, only using
// pios would be the only other technique I can think of.  Given that there
// is already the One Roms by Piers Rocks, this is more about learning without
// first seeing already built code, plus this would also be useful for emulating
// hardware other than rams and roms, such as io devices.
//

byte rom[65536];

byte ops[] = {
  0x01, 0x00, 0x00,       // 10 ld bc,0
  0x11, 0x00, 0x00,       // 10 ld de,0
  0x21, 0x00, 0x00,       // 10 ld hl,0
  // loop avgs ~ 122 ticks, which at 5.20 mhz = a count of about $1000000 every 6 min 33 sec
  // so doing cli cmd d at this time will show 00f0  xxxx 0001 0000, xxxx as this portion changes fast
  // displaying state regularly will show a slightly higher mhz as this value is based only on time
  // running the while loop surrounding the emulation code
  0xed, 0x43, 0xf0, 0x00, // 20 ld ($00f0),bc
  0xed, 0x53, 0xf2, 0x00, // 20 ld ($00f2),de
  0xed, 0x63, 0xf4, 0x00, // 20 ld ($00f4),hl
  0x03,                   //  6 inc bc
  0x78,                   //  4 ld a,b
  0xb1,                   //  4 or c
  0x20, 0x03,             // 12/7 jr nz,6
  0x13,                   //  6 inc de
  0x7a,                   //  4 ld a,d
  0xb3,                   //  4 or e
  0x20, 0x01,             // 12/7 jr nz,1
  0x23,                   //  6 inc hl
  0xdb, 0x01,             // 11 in a,(0x01)
  0xd3, 0x02,             // 11 out (0x02),a
  0xc3, 0x09, 0x00,       // 10 jp 9
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00
};

void initRom() {
  int i;
  for ( int i = 0; i < 65536; i++ ) {
    if ( i < 40 ) {
      rom[i] = ops[i];
    } else {
      rom[i] = 0;
    }
  }
}

//
// ram/rom emulation vars
//

volatile int port = 0;
volatile int addr = 0;
volatile int data = 0;
volatile int wrdata = 0;

volatile float mhz = 0;

//
// clock, ram, rom emulation setup
//

// core1 sprintf buffer for core1

char buffer[100];

//
// gpio pin vars
//

byte mreqPin = 13;
byte wrPin = 11;
byte rdPin = 12;
byte clockPin = 22;

int portPinMask, addrPinMask, wrPinMask, rdPinMask, mreqPinMask;
int dataPinMask, clkPinMask, rwPinMask;
int gp26PinMask, gp27PinMask, gp28PinMask;
int specPinMask, ledPinMask;

void fast_core1_setup() {
  
  //
  // Init memory and add some z80 code
  //

  initRom();
  
  //
  // gpio pin masks and config
  //
  // pin masks    3322222222221111111111
  //              10987654321098765432109876543210
  portPinMask = 0b00000000000000000000000011111111;  // gp0-7
  addrPinMask = 0b00000000000000000000011111111111;  // gp0-10
  wrPinMask   = 0b00000000000000000000100000000000;  // gp11
  rdPinMask   = 0b00000000000000000001000000000000;  // gp12
  mreqPinMask = 0b00000000000000000010000000000000;  // gp13
  dataPinMask = 0b00000000001111111100000000000000;  // gp14-21
  clkPinMask  = 0b00000000010000000000000000000000;  // gp22
  specPinMask = 0b00000000100000000000000000000000;  // gp23 - must not be changed
  ledPinMask  = 0b00000010000000000000000000000000;  // gp25 - builtin led
  gp26PinMask = 0b00000100000000000000000000000000;  // gp26
  gp27PinMask = 0b00001000000000000000000000000000;  // gp27
  gp28PinMask = 0b00010000000000000000000000000000;  // gp28
  
  rwPinMask = rdPinMask | wrPinMask;  // used to check if either is active

  gpio_init_mask( addrPinMask );
  gpio_init_mask( wrPinMask );
  gpio_init_mask( rdPinMask );
  gpio_init_mask( mreqPinMask );
  gpio_init_mask( dataPinMask );
  gpio_init_mask( clkPinMask );
  //gpio_init_mask( gp26PinMask );
  //gpio_init_mask( gp27PinMask );
  //gpio_init_mask( gp28PinMask );
  
  gpio_set_dir_in_masked( addrPinMask );
  gpio_set_dir_in_masked( wrPinMask );
  gpio_set_dir_in_masked( rdPinMask );
  gpio_set_dir_in_masked( mreqPinMask );
  gpio_set_dir_in_masked( dataPinMask );  // initially start as inputs, but change to outputs during mem rd requests
  gpio_set_dir_out_masked( clkPinMask );
  //gpio_set_dir_in_masked( gp26PinMask );
  
}

int input( int port );
void output( int port, int data );

//
// clock, ram, rom emulation loop
//

int reqId = 0;

void __not_in_flash_func( fast_core1_loop )( int loops ) {
  bool processingRequest = false;
  while ( loops-- > 0 ) {
    // Clock is always high at this point high
    int all = gpio_get_all();
    //
    // Check for 
    //
    if ( ( all & rwPinMask ) != rwPinMask ) {
      if ( ! processingRequest ) {
        processingRequest = true;
        addr = all & addrPinMask;
        //
        // check if rd low
        //
        if ( ( all & rdPinMask ) == 0 ) {
          //
          // rd request
          //
          gpio_set_dir_out_masked( dataPinMask );
          if ( ( all & mreqPinMask ) == 0 ) {
            //
            // process mem rd request
            //
            reqId = 1;
            data = rom[addr];
            //gpio_put_masked( dataPinMask, data << 14 );
          } else {
            //
            // process io rd request
            //
            reqId = 2;
            data = input( addr );
            //gpio_put_masked( dataPinMask, data << 14 );  // Having this twice cuts speed in half
          }
          gpio_put_masked( dataPinMask, data << 14 );
        } else {  // wr is the only other option
          //
          // wr request
          //
          reqId = 3;
          data = ( all & dataPinMask ) >> 14;
          if ( ( all & mreqPinMask ) == 0 ) {
            //
            // process mem wr request
            //
            rom[addr] = data;
          } else {
            //
            // process io wr request
            //
            reqId = 4;
            output( addr, data );
          }
        }
      }
    } else {
      if ( processingRequest ) {
        processingRequest = false;
        gpio_set_dir_in_masked( dataPinMask );
      }
    }
    //
    // set clock low
    //
    gpio_put_masked( clkPinMask, 0 );  // replacing digitalWrite in this and the below case increased speed from 0.439 mhz to 0.798 mhz
    //if ( delayTime != 0 ) delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
    //
    // set clock high
    //
    gpio_put_masked( clkPinMask, clkPinMask );  // replacing digitalWrite in this and the above case increased speed from 0.439 mhz to 0.798 mhz
    //if ( delayTime != 0 ) delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
  }
  
}

//
// A variable speed loop, max speed is slower due to speed control if statements
//

void variable_core1_loop( int loops, int delayTime ) {
  /*
  while ( loops-- > 0 ) {  // increases speed to 1.785 Mhz
    // Clock is high
    int all = gpio_get_all();
    if ( ( all & mreqPinMask ) == 0 ) {  // up to 1.163 mhz
      if ( ( all & rdPinMask ) == 0 ) {  // up to 0.993 mhz
        if ( first_req ) {
          //
          // Read request
          //
          first_req = false;
          addr = all & addrPinMask;
          data = rom[addr];
          gpio_set_dir_out_masked( dataPinMask );  // using this instead of initPins in both places increased speed from 0.123 mhz to 0.208 mhz
          gpio_put_masked( dataPinMask, data << 14 );  // using this instead of putData increased speed from 0.208 mhz to 0.279 mhz
          //sprintf( buffer, "Reading: %04x %02x", addr, data );
          //Serial.println( buffer );
        }
      } else if ( ( all & wrPinMask ) == 0 ) {  // up to 0.993 mhz
        if ( first_req ) {
          //
          // Write request
          //
          first_req = false;
          addr = all & addrPinMask;
          data = ( all & dataPinMask ) >> 14;
          rom[addr] = data;
          //sprintf( buffer, "Mhz: %02.6f Writing: %04x %02x", mhz, addr, data );
          //Serial.println( buffer );
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
    if ( delayTime != 0 ) delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
    //
    // set clock high
    //
    gpio_put_masked( clkPinMask, clkPinMask );  // replacing digitalWrite in this and the above case increased speed from 0.439 mhz to 0.798 mhz
    if ( delayTime != 0 ) delay( delayTime );  // commented out both delays increased speed from 0.798 mhz to 0.872 mhz
  }
  */
}
