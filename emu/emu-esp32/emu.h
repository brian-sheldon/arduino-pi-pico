extern "C" {
  #include "z80.h"
}

static Z80 cpu;

byte mem[65536];
byte ports[256];

struct EmuDrive {
  int sides = 1;
  int tracks = 77;
  int sectors = 26;
  int secsize = 128;
  int drv = 0;
  int track = 0;
  int sector = 1;
  int dmalow = 0;
  int dmahigh = 0;
};

EmuDrive drive;

uint8_t mem_read( void *ctx, uint16_t addr ) {
  (void)ctx;
  return mem[addr];
}

void mem_write( void *ctx, uint16_t addr, uint8_t val ) {
  (void)ctx;
  mem[addr] = val;
}

uint8_t io_read( void *ctx, uint16_t port ) {
  (void)ctx;
  switch ( port ) {
    case 0: // console status input available 0xff input not 0x00
      break;
    case 1:
      break; // console input
    case 2:
      return 0xff;
      break;
    case 4:
      return 0xff;
      break;
    case 5: // aux in
      return 0xff;
      break;
    case 10: // FDC drive
      return drive.drv;
      break;
    case 11: // FDC track
      return drive.track;
      break;
    case 12: // FDC sector
      return drive.sector;
      break;
    case 13: // FDC command IO ready?
      break;
    case 14: // FDC status
      break;
    case 15: // FDC DMA low
      return drive.dmalow;
      break;
    case 16: // FDC DMA high
      return drive.dmahigh;
      break;
    case 0x42: // 0x00 when cmd is finished executing
      break;
    default:
      return port & 0xff;
      break;
  }
  return port & 0xff;
}

void io_write( void *ctx, uint16_t port, uint8_t val ) {
  switch ( port & 0xff ) {
    case 1:
      break;
    case 10: // FDC drive
      drive.drv = val;
      break;
    case 11: // FDC track
      drive.track = val;
      break;
    case 12: // FDC sector
      drive.sector = val;
      break;
    case 15:
      drive.dmalow = val;
      break;
    case 16:
      drive.dmahigh = val;
      break;
    default:
      break;
  }
}

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

void setupMem() {
  int i;
  for ( int i = 0; i < 65536; i++ ) {
    if ( i < 40 ) {
      mem[i] = ops[i];
    } else {
      mem[i] = 0;
    }
  }
}

//extern "C" void z80_init( Z80 *cpu );
//extern "C" int Z80_step( Z80 *cpu );

void setupCpu() {
  z80_init( &cpu );
  cpu.mem_read = mem_read;
  cpu.mem_write = mem_write;
  cpu.io_read = io_read;
  cpu.io_write = io_write;
  cpu.ctx = NULL;
  cpu.a = 0;
  cpu.f = 0;
  cpu.sp = 0xfffe;
  cpu.pc = 0x0000;
}

long long ticks = 0;
void setupEmu() {
  setupMem();
  setupCpu();
}

unsigned long lastTime = 0;
long long lastTicks = 0;
float mhz = 0.0;
bool running = false;

String status() {
  String s = "";
  s += "status mhz: ";
  s += String( mhz );
  s += " running: ";
  s += String( running );
  return s;
}

void loopEmu() {
  int loops = 10000;
  if ( running ) {
    while ( loops-- > 0 ) {
      ticks += z80_step(&cpu);
    }
  }
  //
  // calculate the Mhz
  //
  unsigned long currTime = micros();
  unsigned long diffTime = currTime - lastTime;
  long diffTicks = ticks - lastTicks;
  if ( diffTime > 0.0 ) {
    mhz = 1.0 * diffTicks / diffTime;
  }
  lastTime = currTime;
  lastTicks = ticks;
}



