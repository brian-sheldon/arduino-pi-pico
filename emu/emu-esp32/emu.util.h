
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void print( String s = "" ) {
  Serial.print( s );
}

void print( int v ) {
  print( String( v ) );
}

void println( String s = "" ) {
  print( s + "\r\n" );
}

void println( int v ) {
  println( String( v ) );
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
    lines += colors[color].dump_addr;
    lines += hex4( addr ) + "  ";
    lines += colors[color].dump_hex;
    lines += hexLine( data, pos, cols ) + "  ";
    lines += colors[color].dump_asc;
    lines += ascLine( data, pos, cols );
    addr += cols;
    pos += cols;
  }
  return lines;
}

