
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "emu.fs.h"

#include <tuple>

int delayTime = 0;
bool fast = true;
bool showState = false;
bool showHex = false;

std::string trim( std::string str ) {
  const std::string whitespace = " \t\n\r\f\v";
  size_t start = str.find_first_not_of( whitespace );
  if ( start != std::string::npos ) {
    str.erase( 0, start );
  } else {
    str.clear();
  }
  size_t end = str.find_last_not_of( whitespace );
  if ( end != std::string::npos ) {
    str.erase( end + 1 );
  }
  return str;
}

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

String hex4( int v ) {
  return hex0( v, 4 );
}

String ascLine( uint8_t *data, int pos, int cols ) {
  String line = "";
  for ( int i = 0; i < cols; i++ ) {
    char ch = data[pos+i];
    int cc = (int)ch;
    if ( cc >= 0x20 && cc <= 0x7e ) {
      line += ch;
    } else {
      line += '.';
    }
  }
  return line;
}

String hexLine( uint8_t *data, int pos, int cols ) {
  String line = "";
  for ( int i = 0; i < cols; i++ ) {
    line += hex2( data[pos+i] );
    if ( ( i % 2 ) == 1 ) line += " ";
  }
  return line;
}

String hexLines( int addr, uint8_t *data, int pos, int rows, int cols ) {
  String lf = "\r\n";
  String lines = "";
  for ( int row = 0; row < rows; row++ ) {
    if ( row != 0 ) lines += lf;
    lines += hex4( addr ) + "  " + hexLine( data, pos, cols ) + "  " + ascLine( data, pos, cols );
    addr += cols;
    pos += cols;
  }
  return lines;
}

//
// cmd line loop
//

//string status();

int drv = 0;
int drvs[] = {0,1,2,3,4,5,6,7,8,9};
EmuDiskImg imgs[] = {
  EmuDiskImg( "/emu/disks/cpm22-1.dsk" ),
  EmuDiskImg( "/emu/disks/cpm22-2.dsk" ),
  EmuDiskImg( "/emu/disks/8080tools.cpm" ),
  EmuDiskImg( "/emu/disks/trek.cpm" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" )
};

String defcmd = "";
void cmdLine( String cmd ) {
  
  cmd.trim();
  while ( cmd.indexOf( "  " ) >= 0 ) {
    cmd.replace( "  ", " " );
  }
  
  String p0 = cmd;
  String p1 = "";
  String p2 = "";
  String p3 = "";
  String p4 = "";
  int beg = 0;
  int end = 0;
  if ( ( end = cmd.indexOf( " " ) ) >= 0 ) {
    p0 = cmd.substring( beg, end );
    beg = end + 1;
    if ( ( end = cmd.indexOf( " ", beg ) ) >= 0 ) {
      p1 = cmd.substring( beg, end );
      beg = end + 1;
      if ( ( end = cmd.indexOf( " ", beg ) ) >= 0 ) {
        p2 = cmd.substring( beg, end );
        beg = end + 1;
        if ( ( end = cmd.indexOf( " ", beg ) ) >= 0 ) {
          p3 = cmd.substring( beg, end );
          beg = end + 1;
          if ( ( end = cmd.indexOf( " ", beg ) ) >= 0 ) {
            p4 = cmd.substring( beg, end );
            beg = end + 1;
          } else {
            p4 = cmd.substring( beg );
          }
        } else {
          p3 = cmd.substring( beg );
        }
      } else {
        p2 = cmd.substring( beg );
      }
    } else {
      p1 = cmd.substring( beg );
    }
  }
  //Serial.println( p0 );
  //Serial.println( p1 );
  //Serial.println( p2 );
  
  if ( p0 == "" ) {
    p0 = defcmd;
  }

  if ( p0 == "off" || p0 == "1b5b347e" ) {
    Serial.println( "Cpu running: false" );
    running = false;
  } else if ( p0 == "on" || p0 == "1b5b317e" ) {
    Serial.println( "Cpu running: true" );
    running = true;
  } else if ( p0 == "d" ) {
    int addr = 0;
    if ( p1 != "" ) {
      addr = p1.toInt();
    }
    Serial.println( hexLines( addr, mem, addr, 16, 16 ) );
  } else if ( p0 == "mhz" ) {
    Serial.println( mhz );
  } else if ( p0 == "ls" ) {
    EmuFileUtil fsutil;
    if ( p1 == "" ) {
      p1 = "/emu/disks";
    }
    fsutil.ls( p1 );
  } else if ( p0 == "rm" ) {
    EmuFileUtil fsutil;
    fsutil.rm( "/emu/disks/cpm22-1.tst" );
  } else if ( p0 == "cp" ) {
    EmuFileUtil fsutil;
    fsutil.cp( "/emu/disks/cpm22-1.dsk", "/emu/disks/cpm22-1.tst" );
  } else if ( p0 == "cmp" ) {
    EmuFileUtil fsutil;
    fsutil.cmp( "/emu/disks/cpm22-1.dsk", "/emu/disks/cpm22-1.tst" );
  } else if ( p0 == "b" ) {
    EmuFile file = EmuFile( "/emu/disks/cpm22-1.tst" );
    file.open();
    file.seek(0);
    file.read( mem + 128, 128 );
    file.close();
  } else if ( p0 == "drv" ) {
    if ( p1 != "" ) {
      drv = p1.toInt();
    }
    if ( p2 != "" ) {
      int i = p2.toInt();
      drvs[drv] = i;
    }
    Serial.print( "drv: " );
    Serial.println( drv );
  } else if ( p0 == "disk" ) {
    defcmd = p0;
    int track = -1;
    int sector = -1;
    if ( p1 != "" ) {
      track = p1.toInt();
    }
    if ( p2 != "" ) {
      sector = p2.toInt();
    }
    uint8_t buffer[128];
    imgs[drv].readsec( buffer, 0, track, sector );
    Serial.println( hexLines( 0, buffer, 0, 8, 16 ) );
  } else if ( p0 == "drvs" ) {
    for ( int i = 0; i < 10; i++ ) {
      Serial.print( "drv: " );
      Serial.print( i );
      Serial.print( " img: " );
      Serial.print( drvs[i] );
      Serial.print( " path: " );
      Serial.println( imgs[drvs[i]].getPath() );
    }
  } else if ( p0 == "imgs" ) {
    for ( int i = 0; i < 10; i++ ) {
      Serial.print( "drv: " );
      Serial.print( drvs[i] );
      Serial.print( " img: " );
      Serial.print( i );
      Serial.print( " path: " );
      Serial.println( imgs[i].getPath() );
    }
  } else if ( p0 == "img" ) {
    int i = drv;
    if ( p1 != "" ) {
      i = p1.toInt();
    }
    if ( p2 != "" ) {
      imgs[i].config( p2 );
    }
    Serial.print( "img: " );
    Serial.print( i );
    Serial.print( " path: " );
    Serial.println( imgs[i].getPath() );
  } else if ( p0 == "blksec" ) {
    int blk, blksec;
    int trk, log, sec;
    if ( p1 != 0 ) {
      blk = p1.toInt();
      if ( p2 != "" ) {
        blksec = p2.toInt();
        auto [ trk, log, sec ] = imgs[drvs[drv]].blksec( blk, blksec );
        Serial.print( "trk: " );
        Serial.print( trk );
        Serial.print( " log: " );
        Serial.print( log );
        Serial.print( " sec: " );
        Serial.print( sec );
        Serial.print( " blk: " );
        Serial.print( blk );
        Serial.print( " blksec: " );
        Serial.println( blksec );
      }
    }
  } else if ( p0 == "trksec" ) {
    int blk, blksec;
    int trk, log, sec;
    if ( p1 != 0 ) {
      trk = p1.toInt();
      if ( p2 != "" ) {
        log = p2.toInt();
        auto [ sec, blk, blksec ] = imgs[drvs[drv]].trksec( trk, log );
        Serial.print( "trk: " );
        Serial.print( trk );
        Serial.print( " log: " );
        Serial.print( log );
        Serial.print( " sec: " );
        Serial.print( sec );
        Serial.print( " blk: " );
        Serial.print( blk );
        Serial.print( " blksec: " );
        Serial.println( blksec );
      }
    }
  } else {
    Serial.print( "[" );
    Serial.print( cmd );
    Serial.println( "] not recognized ... " );
  }
}

//
// line editing loop
//
// This loop performs basic line editing functions such as backspace, left/right cursor movement and enter to
// submit the command.  Enter will send cmd buffer to the cmdLine loop function.
//

const int cmdBufferLen = 101;
char cmd[cmdBufferLen];
int cmdPos = 0;
int cmdEnd = 0;

void cursorLeft() {
  Serial.print( "\x1b[D" );
}

void cursorRight() {
  Serial.print( "\x1b[C" );
}

void cursorSave() {
  Serial.print( "\x1b[s" );
}

void cursorUnsave() {
  Serial.print( "\x1b[u" );
}

void delStrChar( char* str, int index ) {
  int len = strlen( cmd );
  if ( index >= 0 && index < len ) {
    for ( int i = index; i < len; i++ ) {
      str[i] = str[i+1];
    }
  }
  str[len-1] = '\0';
}

void insStrChar( char* str, int index, char ch ) {
  int len = strlen( cmd );
  if ( ( len + 2 ) < cmdBufferLen && index < ( cmdBufferLen - 2 ) ) {
    for ( int i = len; i >= index; i-- ) {
      str[i+1] = str[i];
    }
    str[index] = ch;
  }
}

void lineEdit( int len, char ch, int cc, String hexStr ) {
  if ( hexStr == "0d" ) {  // Enter
    Serial.println();
    //Serial.println( strlen( cmd ) );
    cmdLine( cmd );
    cmdPos = 0;
    cmd[cmdPos] = '\0';
  } else if ( hexStr == "7f" ) {  // Backspace
    if ( cmdPos > 0 ) {
      //String right = cmd.
      cmdPos--;
      delStrChar( cmd, cmdPos );
      cursorLeft();
      cursorSave();
      Serial.print( cmd + cmdPos );
      Serial.print( " " );
      cursorUnsave();
    }
  } else if ( hexStr == "1b5b44" ) {  // Left Arrow
    if ( cmdPos > 0 ) {
      cmdPos--;
      cursorLeft();
    }
  } else if ( hexStr == "1b5b43" ) {  // Right Arrow
    if ( cmdPos < ( strlen( cmd ) ) ) {
      cmdPos++;
      cursorRight();
    }
  } else {  // Printable character
    if ( len == 1 ) {
      if ( cc >= 0x20 && cc <= 0x7e ) {
        if ( cmdPos > ( cmdBufferLen - 3 ) ) {
          cmdPos = cmdBufferLen - 3;
          cursorLeft();
        }
        if ( strlen( cmd ) < ( cmdBufferLen - 2 ) ) {
          insStrChar( cmd, cmdPos++, ch );
          Serial.print( ch );
          cursorSave();
          Serial.print( cmd + cmdPos );
          cursorUnsave();
        }
      }
    }
  }
}

//
// A loop to receive hex data
//
// to be implemented
//

unsigned long rcvHexBeg = 0;
unsigned long rcvHexTimeout = 10000000;

int rcvHexCount = 0;

bool rcvHex = false;

void rcvHexLoop( char ch ) {
  if ( micros() > ( rcvHexBeg + rcvHexTimeout ) ) {
    rcvHex = false;
    Serial.println();
    Serial.print( "Receive Hex data timeout ..." );
  } else if ( ch == '.' ) {
    rcvHex = false;
    Serial.println();
    Serial.print( "Hex Bytes received: " );
    Serial.println( rcvHexCount / 2 );
  } else {
    rcvHexBeg = micros();
    if ( ( rcvHexCount++ % 64 ) == 0 ) {
      Serial.println();
    }
    Serial.print( ch );
  }
}

//
// io redirection and cpu control cmd loop
// This loop is used to perform basic io redirection, cpu control and toggle debugging functions on/off.
// All other characters are forwarded to the lineEdit loop.
//

bool io2cli = true;


void ctrlLoop( int len, char ch, int cc, String hexStr ) {
  if ( hexStr == "1b5b357e" ) {         // PageUp
    Serial.println( "io directed to cpu ..." );
    io2cli = false;
  } else if ( hexStr == "1b5b367e" ) {  // PageDown
    Serial.println( "io directed to cli ..." );
    io2cli = true;
  } else if ( hexStr == "1b5b347e" ) {
    Serial.println( "Cpu running: false" );
    running = false;
  } else if ( hexStr == "1b5b317e" ) {
    Serial.println( "Cpu running: true" );
    running = true;
  } else if ( hexStr == "1b5b34333231307e" || hexStr == "18" ) {  // 4 3 2 1 0 switch to hex mode
    rcvHex = true;
    Serial.println( "Waiting for Hex data ..." );
    rcvHexBeg = micros();
  } else if ( hexStr == "13" ) {
    showState = ! showState;
  } else if ( hexStr == "06" ) {
    fast = ! fast;
    delayTime = 500;
    if ( fast ) {
      delayTime = 0;
    }
  } else if ( hexStr == "1a" ) {  // ctrl-z
    showHex = ! showHex;
  } else {
    if ( rcvHex ) {
      rcvHexLoop( ch );
    } else if ( io2cli ) {
      lineEdit( len, ch, cc, hexStr );
    } else {
      // io2cpu queue
    }
  }
}

//
// ioLoop reads incoming characters from serial.  If it receives the ESC character, it tries to
// see if it is the start of an ansi key definition, such as 1b5b44, left arrow.  If ansi end
// character is received, it sends the entire ansi hex string to the ctrlLoop for further processing.
// Otherwise, it sends a single character, including a lone ESC character, to the crtlLoop for processing.
// I am not sure if this loop correctly captures all ansi keyboard characters, but it does capture the
// ones in use by the cli at this point.
//

void ioLoop() {
  String ansiEnd = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~";
  if ( Serial.available() ) {
    int len = 0;
    char ch;
    int cc;
    String hx;
    String hexStr = "";
    bool ansi = false;
    unsigned long end = 0;
    unsigned long timeout = 10000;
    do {
      ch = Serial.read();
      if ( ch != 0xff ) {
        cc = (int)ch;
        hx = hex2( cc );
        hexStr += hx;
        len++;
        if ( cc == 0x1b ) {
          ansi = true;
          end = micros() + timeout;
        }
        if ( ansiEnd.indexOf( ch ) != -1 ) {
          ansi = false;
        }
      }
      if ( micros() > end ) {
        ansi = false;
      }
    } while ( ansi );
    if ( showHex ) Serial.println( hexStr );
    ctrlLoop( len, ch, cc, hexStr );
  }
}

