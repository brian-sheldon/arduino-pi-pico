
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void trace_cpu_on() {}

void trace_cpu_off() {}



void trace_mem_clr() {}

//
// Cpu Trace Functions
//

void trace_cpu_clr() {
  for ( int addr = 0; addr < traceCpuLen; addr++ ) {
    traceCpu[addr] = 0;
  }
  println( "cpu trace data cleared ..." );
}

void trace_cpu_set() {
  if ( cmdline.plen > 1 ) {
    traceCpuStart = hex2int( cmdline.p1 );
  }
  print( "trace cpu start: " );
  println( hex4( traceCpuStart ) );
}

void trace_cpu_show() {
  int beg = 0x0000;
  int end = 0xffff;
  int over = 0;
  if ( cmdline.plen > 1 ) {
    beg = hex2int( cmdline.p1 ) & 0xffff;
  }
  if ( cmdline.plen > 2 ) {
    end = hex2int( cmdline.p2 ) & 0xffff;
  }
  if ( cmdline.plen > 3 ) {
    over = dec2int( cmdline.p3 ) & 0xffff;
  }
  for ( int addr = 0; addr < traceCpuLen; addr++ ) {
    if ( addr >= beg && addr <= end ) {
      int count = traceCpu[addr];
      if ( count > over ) {
        print( "count: " );
        print( dec0( count, 5 ) );
        print( "  addr: " );
        print( hex4( traceCpuStart + addr ) );
        print( "  byte: " );
        println( hex2( mem[addr] ) );
      }
    }
  }
}

//
//
//

void trace_mem_save() {}

void trace_cpu_save() {}

void trace_mem_array() {}

void trace_cpu_array() {}




cmd_entry_t cmds_trace[] = {
  { "cclr", trace_cpu_clr, "", "clear cpu trace data" },
  { "cset", trace_cpu_set, "", "set cpu trace addr start" },
  { "show", trace_cpu_show, "[beg] [end] [min]", "display num of op fetches at addr over min" },
  { NULL, NULL, NULL, NULL }
};
