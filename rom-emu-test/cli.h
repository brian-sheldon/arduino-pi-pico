



int delayTime = 0;
bool fast = true;
bool showState = false;

String padLeft( String str, int width, char ch = ' ' ) {
  while ( str.length() < width ) {
    str = ch + str;
  }
  return str;
}

String hex0( int v, int width = 0, char ch = '0' ) {
  String hx = String( v, HEX );
  hx = padLeft( hx, width, '0' );
  return hx;
}

String hex2( int v ) {
  return hex0( v, 2 );
}

char cmd[100];
int cmdPos = 0;

/*
void doCmd( String cmd ) {
  cmd = cmd.trim();
  String p0 = "";
  if ( p0 == "" ) {
    //
  } else if ( p0 == "" ) {
    //
  }
}
*/

//
// cmd line loop
//

void cmdLine( String cmd ) {
  Serial.println();
  Serial.print( cmd );
  Serial.print( " not recognized ... " );
  Serial.println( "No commands defined yet ..." );
}

//
// line editing loop
//

void lineEdit( int len, char ch, int cc, String hexStr ) {
  if ( hexStr == "08" || hexStr == "7f" ) {
    cmdPos--;
    if ( cmdPos < 0 ) {
      cmdPos = 0;
    } else {
      Serial.print( "" );
      cmd[cmdPos] = '\0';
    }
  } else if ( hexStr == "0d" ) {
    cmdLine( cmd );
    cmdPos = 0;
    cmd[cmdPos] = '\0';
    // Printable characters and not used
  } else {
    if ( len == 1 ) {
      if ( cc >= 0x20 && cc <= 0x7e ) {
        cmd[cmdPos++] = ch;
        if ( cmdPos >= 100 ) {
          cmdPos = 99;
        }
      }
    }
  }
  Serial.print( "\r" );
  Serial.print( cmd );
}

//
// io redirection and cpu control cmd loop
//

bool io2cli = true;

void ctrlLoop( int len, char ch, int cc, String hexStr ) {
  if ( hexStr == "1b5b357e" ) {         // PageUp
    Serial.println( "io directed to cpu ..." );
    io2cli = false;
  } else if ( hexStr == "1b5b367e" ) {  // PageDown
    Serial.println( "io directed to cli ..." );
    io2cli = true;
  } else if ( hexStr == "13" ) {
    showState = ! showState;
  } else if ( hexStr == "06" ) {
    fast = ! fast;
    delayTime = 500;
    if ( fast ) {
      delayTime = 0;
    }
  } else {
    if ( io2cli ) {
      lineEdit( len, ch, cc, hexStr );
    } else {
      // io2cpu queue
    }
  }
}

void ioLoop() {
  String ansiEnd = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~";
  if ( Serial.available() ) {
    int len = 0;
    char ch;
    int cc;
    String hx;
    bool ansi = false;
    String hexStr = "";
    bool cont = true;
    while ( cont ) {
      ch = Serial.read();
      cc = (int)ch;
      hx = hex2( cc );
      hexStr += hx;
      len++;
      if ( cc == 0x1b ) {
        ansi = true;
        cont = true;
      }
      if ( ansi ) {
        if ( ansiEnd.indexOf( ch ) != -1 ) {
          ansi = false;
          cont = false;
        }
      } else {
        cont = false;
      }
    }
    ctrlLoop( len, ch, cc, hexStr );
  }

}

