
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
  
  int steps = 1;
  int ticks = 0;
  if ( cmdline.plen > 1 ) {
    steps = dec2int( cmdline.p1 );
  }
  while ( steps-- > 0 ) {
    ticks += z80_step(&cpu);
  }
  print( "pc: " );
  print( cpu.pc );
  print( " ticks: " );
  println( ticks );
  //defcmd = p0;
  
}

cmd_entry_t cmds_cpu[] = {
  { "mhz", cpu_mhz, "", "cpu mhz" },
  { "d", cpu_d, "[addr]", "mem dump at next addr or given addr" },
  { "step", cpu_step ,"[steps]" , "cpu step once or given steps" },
  { "pc", cpu_pc ,"[addr]" , "show or set reg pc" },
  { "on", cpu_on ,"" , "turn cpu on" },
  { "1b5b317e", cpu_on ,"" , "end key - turn cpu on" },
  { "off", cpu_off ,"" , "turn cpu off" },
  { "1b5b347e", cpu_off ,"" , "home key - turn cpu off" },
  { NULL, NULL, NULL, NULL }
};
