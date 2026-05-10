
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

struct Colors {
  char *reset;
  char *prompt;
  char *ls_dir;
  char *ls_file;
  char *dump_addr;
  char *dump_hex;
  char *dump_asc;
};

// 30 - black
// 31 - red
// 32 - green
// 33 - blue
// 34 - 
// 35 - 
// 36 - white
// 37 - 

int color = 1;
Colors colors[] = {
  {"","","","","","",""},
  {
    "\x1b[1;32m",  // reset, cmdline
    "\x1b[1;31m",  // prompt
    "\x1b[1;36m",  // ls_dir
    "\x1b[1;33m",  // ls_file
    "\x1b[1;34m",  // dump_addr
    "\x1b[1;36m",  // dump_hex
    "\x1b[1;37m"   // dump_asc
  }
};
