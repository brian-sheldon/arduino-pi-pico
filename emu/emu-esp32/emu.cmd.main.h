
#include "emu.cmd.h"

static void words();

cmd_entry_t cmds[] = {
  { "words", words, "" },
  { NULL, NULL, NULL }
};

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
  exec( "notcmd" );
  Serial.println( "test cmd.main ends ..." );
}
