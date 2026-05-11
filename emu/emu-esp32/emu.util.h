
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <stdlib.h>

void print( char *str ) {
  //printf( "%s", str );
  Serial.print( str );
}

void print( int v ) {
  //printf( "%d", v );
  Serial.print( v );
}

void println() {
  print( "\n" );
}

void println( char *str ) {
  print( str );
  println();
}

void println( int v ) {
  print( v );
  println();
}

int dec2int( char *str ) {
  char *endptr;
  int v = strtol( str, &endptr, 10 );
  if ( str == endptr ) {
    return 0;
  } else {
    return v;
  }
}

int hex2int( char *str ) {
  char *endptr;
  int v = strtol( str, &endptr, 16 );
  if ( str == endptr ) {
    return 0;
  } else {
    return v;
  }
}

char *hex0( int v, int width = 0 ) {
  static char buffer[ 20 ];
  snprintf( buffer, sizeof( buffer ), "%0*x", width, v );
  return buffer;
}

char *hex2( int v ) {
  return hex0( v, 2 );
}

char *hex4( int v ) {
  return hex0( v, 4 );
}

char *ascLine( uint8_t *data, int pos, int cols ) {
  static char line[17];
  line[0] = '\0';
  if ( cols <= 16 ) {
    for ( int i = 0; i < cols; i++ ) {
      char ch = data[pos+i];
      int cc = (int)ch;
      if ( cc >= 0x20 && cc <= 0x7e ) {
        line[i] = ch;
      } else {
        line[i] = '.';
      }
    }
    line[ cols ] = '\0';
  }
  return line;
}

char *hexLine( uint8_t *data, int pos, int cols ) {
  static char line[50];
  line[0] = '\0';
  if ( cols <= 16 ) {
    for ( int i = 0; i < cols; i++ ) {
      strcat( line, hex2( data[pos+i] ) );
      if ( ( i % 2 ) == 1 ) strcat( line, " " );
    }
  }
  return line;
}

char *hexLines( int addr, uint8_t *data, int pos, int rows, int cols ) {
  char lf[] = "\r\n";
  static char lines[1000];
  lines[0] = '\0';
  for ( int row = 0; row < rows; row++ ) {
    if ( row != 0 ) strcat( lines, lf );
    strcat( lines, colors[color].dump_addr );
    strcat( lines, hex4( addr ) );
    strcat( lines, " " );
    strcat( lines, colors[color].dump_hex );
    strcat( lines, hexLine( data, pos, cols ) );
    //strcat( lines, "" );
    strcat( lines, colors[color].dump_asc );
    strcat( lines, ascLine( data, pos, cols ) );
    addr += cols;
    pos += cols;
  }
  return lines;
}

