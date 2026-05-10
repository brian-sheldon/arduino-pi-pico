
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
  char *dump_label;
  char *dump_value;
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
  {"","","","","","","","",""},
  {
    "\x1b[0;32m",  // reset, cmdline
    "\x1b[0;31m",  // prompt
    "\x1b[0;36m",  // ls_dir
    "\x1b[0;33m",  // ls_file
    "\x1b[0;34m",  // dump_addr
    "\x1b[0;36m",  // dump_hex
    "\x1b[0;37m",  // dump_asc
    "\x1b[0;33m",  // dump_label
    "\x1b[0;31m"   // dump_value
  }
};
