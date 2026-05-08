
#include "emu.cmd.h"

static void test() {
  // do nothing for now
}

static void help();
static void words();

cmd_entry_t cmds[] = {
  { "test", test, "count val", "misc testing" },
  { "help", help, "", "detailed help for all commands" },
  { "words", words, "", "list all commands" },
  { NULL, NULL, NULL, NULL }
};

static void help() {
  Serial.println( "help ..." );
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    Serial.print( cmds[i].name );
    Serial.print( " " );
    Serial.print( cmds[i].params );
    Serial.print( "   ; " );
    Serial.println( cmds[i].desc );
  }
}

static void words() {
  Serial.print( "[ " );
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    Serial.print( cmds[i].name );
    Serial.print( " " );
  }
  Serial.println( "]" );
}

static bool exec( char *cmd ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    if ( strcmp( cmds[i].name, cmd ) == 0 ) {
      cmds[i].func();
      return true;
    }
  }
  Serial.print( "Command not found: " );
  Serial.println( cmd );
  return false;
}

void setupCmd() {
  Serial.println();
  Serial.println( "test cmd.main begs ..." );
  exec( "words" );
  exec( "help" );
  exec( "notcmd" );
  Serial.println( "test cmd.main ends ..." );
}
