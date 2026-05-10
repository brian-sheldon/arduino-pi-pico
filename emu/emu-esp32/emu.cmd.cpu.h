
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void cpu_mhz() {
  print( "mhz: " );
  println( mhz );
}

void cpu_on() {
  running = true;
  println( "cpu: on" );
}

void cpu_off() {
  running = false;
  println( "cpu: off" );
}

void cpu_pc() {
  if ( cmdline.plen > 1 ) {
    cpu.pc = hex2int( cmdline.p1 ) & 0xffff;
  }
  Serial.println( cpu.pc );
}

void regStr( char *buffer, char *reg, int val, int spcs = 1, bool bar = false, bool lf = false ) {
  char hex[5];
  if ( strcmp( reg, "s" ) == 0 || strcmp( reg, "z" ) == 0 || strcmp( reg, "h" ) == 0 || strcmp( reg, "p" ) == 0 || strcmp( reg, "n" ) == 0 || strcmp( reg, "c" ) == 0 || strcmp( reg, "iff1" ) == 0 || strcmp( reg, "iff2" ) == 0 ) {
    snprintf( hex, sizeof( hex ), "%1x", val );
  } else if ( strcmp( reg, "A" ) == 0 || strcmp( reg, "F") == 0 ) {
    snprintf( hex, sizeof( hex ), "%02x", val );
  } else {
    snprintf( hex, sizeof( hex ), "%04x", val );
  }
  strcat( buffer, colors[color].reg_label );
  strcat( buffer, reg );
  strcat( buffer, " " );
  strcat( buffer, colors[color].reg_value );
  strcat( buffer, hex );
  for ( int i = 0; i < spcs; i++ ) {
    strcat( buffer, " " );
  }
  if ( bar ) {
    strcat( buffer, "|" );
    if ( lf ) {
      strcat( buffer, "\r\n" );
    } else {
      strcat( buffer, " " );
    }
  }
}

void cpu_state() {
  char buffer[600];
  buffer[0] = '\0';
  //
  regStr( buffer, "PC", cpu.pc, 1, true );
  regStr( buffer, "SP", cpu.sp );
  regStr( buffer, "A", cpu.a );
  regStr( buffer, "F", cpu.f, 5, true );
  regStr( buffer, "AF_", cpu.a_<<8 | cpu.f_, 1, true, true );
  //
  regStr( buffer, "BC", cpu.b<<8 | cpu.c, 1, true );
  regStr( buffer, "IR", cpu.i<<8 | cpu.r );
  regStr( buffer, "s", cpu.f>>7 & 1, 2 );
  regStr( buffer, "z", cpu.f>>6 & 1, 2 );
  regStr( buffer, "h", cpu.f>>4 & 1, 1, true );
  regStr( buffer, "BC_", cpu.b_<<8 | cpu.c_, 1, true, true );
  //
  regStr( buffer, "DE", cpu.d<<8 | cpu.e, 1, true );
  regStr( buffer, "IX", cpu.ix );
  regStr( buffer, "p", cpu.f>>2 & 1, 2 );
  regStr( buffer, "n", cpu.f>>1 & 1, 2 );
  regStr( buffer, "c", cpu.f & 1, 1, true );
  regStr( buffer, "DE_", cpu.d_<<8 | cpu.d_, 1, true, true );
  //
  regStr( buffer, "HL", cpu.h<<8 | cpu.l, 1, true );
  regStr( buffer, "IY", cpu.iy );
  regStr( buffer, "iff1", cpu.iff1 );
  regStr( buffer, "iff2", cpu.iff2, 1, true );
  regStr( buffer, "HL_", cpu.h_<<8 | cpu.l_, 1, true );
  //
  println( buffer );
}

void cpu_d() {
  int addr = dumpaddr;
  if ( cmdline.plen > 1 ) {
    addr = hex2int( cmdline.p1 ) & 0xffff;
  }
  Serial.println( hexLines( addr, mem, addr, 16, 16 ) );
  dumpaddr = addr + 256;
  //defcmd = p0;
}

void cpu_step() {
  bool each = false;
  int steps = 1;
  int ticks = 0;
  if ( cmdline.plen > 1 ) {
    steps = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    each = true;
  }
  while ( steps-- > 0 ) {
    ticks += z80_step(&cpu);
    if ( each ) {
      cpu_state();
      if ( steps > 0 ) {
        print( colors[color].prompt );
        println( ">>> " );
      }
    }
  }
  if ( ! each ) cpu_state();
  print( "ticks: " );
  println( ticks );
  //defcmd = p0;
  
}

cmd_entry_t cmds_cpu[] = {
  { "mhz", cpu_mhz, "", "cpu mhz" },
  { "d", cpu_d, "[addr]", "mem dump at next addr or given addr" },
  { "step", cpu_step ,"[steps]" , "cpu step once or given steps" },
  { "state", cpu_state ,"" , "cpu state" },
  { "pc", cpu_pc ,"[addr]" , "show or set reg pc" },
  { "on", cpu_on ,"" , "turn cpu on" },
  { "1b5b317e", cpu_on ,"" , "end key - turn cpu on" },
  { "off", cpu_off ,"" , "turn cpu off" },
  { "1b5b347e", cpu_off ,"" , "home key - turn cpu off" },
  { NULL, NULL, NULL, NULL }
};
