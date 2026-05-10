
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "emu.cmd.h"

#include "emu.fs.h"
#include "emu.h"

#include <tuple>

#include "emu.cmd.fs.h"
#include "emu.cmd.disk.h"
#include "emu.cmd.cpu.h"

static void main_colors() {
  println( "\x1b[0;30mBright: 0 Color: 0" );
  println( "\x1b[0;31mBright: 0 Color: 1" );
  println( "\x1b[0;32mBright: 0 Color: 2" );
  println( "\x1b[0;33mBright: 0 Color: 3" );
  println( "\x1b[0;34mBright: 0 Color: 4" );
  println( "\x1b[0;35mBright: 0 Color: 5" );
  println( "\x1b[0;36mBright: 0 Color: 6" );
  println( "\x1b[0;37mBright: 0 Color: 7" );
  println( "\x1b[1;30mBright: 1 Color: 0" );
  println( "\x1b[1;31mBright: 1 Color: 1" );
  println( "\x1b[1;32mBright: 1 Color: 2" );
  println( "\x1b[1;33mBright: 1 Color: 3" );
  println( "\x1b[1;34mBright: 1 Color: 4" );
  println( "\x1b[1;35mBright: 1 Color: 5" );
  println( "\x1b[1;36mBright: 1 Color: 6" );
  println( "\x1b[1;37mBright: 1 Color: 7" );
}

static void help();
static void words();

cmd_entry_t cmds_main[] = {
  { "colors", main_colors, "", "display colors" },
  { "help", help, "", "detailed help for all commands" },
  { "words", words, "", "list all commands" },
  { NULL, NULL, NULL, NULL }
};

static void help_one( char *grp, cmd_entry_t *cmds ) {
  char dashs10[] = "----------";
  char dashs60[] = "------------------------------------------------------------";
  char buffer[80];
  snprintf( buffer, sizeof( buffer ), "%s %-7s %s", dashs10, grp, dashs60 );
  println( buffer );
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    snprintf( buffer, sizeof( buffer ), "%-10s  %-16s ; %s", cmds[i].name, cmds[i].params, cmds[i].desc );
    println( buffer );
  }
}

static void help() {
  help_one( "main", cmds_main );
  help_one( "cpu", cmds_cpu );
  help_one( "disk", cmds_disk );
  help_one( "fs", cmds_fs );
}

static void words_one( cmd_entry_t *cmds ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    Serial.print( cmds[i].name );
    Serial.print( " " );
  }
}

static void words() {
  Serial.print( "[ " );
  words_one( cmds_main );
  words_one( cmds_cpu );
  words_one( cmds_disk );
  words_one( cmds_fs );
  Serial.println( "]" );
}

static bool exec_one( cmd_entry_t *cmds, char *cmd ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    if ( strcmp( cmds[i].name, cmd ) == 0 ) {
      cmds[i].func();
      return true;
    }
  }
  return false;
}

static void exec( char *cmd ) {
  bool res = false;
  if ( ! res ) res = exec_one( cmds_cpu, cmd );
  if ( ! res ) res = exec_one( cmds_disk, cmd );
  if ( ! res ) res = exec_one( cmds_fs, cmd );
  if ( ! res ) res = exec_one( cmds_main, cmd );
  if ( ! res ) {
    if ( strcmp( cmd, "" ) ) {
      print( "[" );
      print( cmd );
      println( "] not recognized ..." );
    }
  }
}

void do_cmd( char *cmd ) {
  // process cmd line into args separated by \0
  // set pointer to each arg
  //int arg = 0;
  bool ws = true;
  int j = 0;
  cmdline.plen = 0;
  //int args[10];
  //char cmdargs[100];
  //for ( int i = 0; cmd[i] != '\0'; i++ ) {
  int i = 0;
  do {
    char ch = cmd[i];
    if ( ws ) {
      if ( ch != ' ' && ch != '\0' ) {
        cmdline.p[cmdline.plen] = j;
        cmdline.args[j++] = ch;
        //arg++;
        ws = false;
      }
    } else {
      if ( ch == ' ' || ch == '\0' ) {
        cmdline.args[j++] = '\0'; // will be \0 after testing done
        cmdline.plen++;
        ws = true;
      } else {
        cmdline.args[j++] = ch;
      }
    }
  } while ( cmd[i++] != 0 );
  cmdline.args[j] = '\0';
  for ( int i = cmdline.plen; i < 10; i++ ) {
    cmdline.p[i] = j;
  }
  // convenience defs
  cmdline.p0 = cmdline.args + cmdline.p[0];
  cmdline.p1 = cmdline.args + cmdline.p[1];
  cmdline.p2 = cmdline.args + cmdline.p[2];
  cmdline.p3 = cmdline.args + cmdline.p[3];
  cmdline.p4 = cmdline.args + cmdline.p[4];
  cmdline.p5 = cmdline.args + cmdline.p[5];
  exec( cmdline.p0 );
  //
  //Serial.println( cmdline.p0 );
  for ( int i = 0; i < 10; i++ ) {
    //Serial.println( cmdline.args + cmdline.p[i] );
  }
  //
  //Serial.println( cmdline.plen );
  //Serial.println( cmdline.args );
}

void setupCmd() {
  Serial.println();
  Serial.println( "test cmd.main begs ..." );
  
  Serial.println( "test cmd.main ends ..." );
}
