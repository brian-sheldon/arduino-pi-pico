import time
import digitalio

#import microcontroller
import memorymap

import board



pins = [
    board.GP0, board.GP1, board.GP2, board.GP3,     #addr
    board.GP4, board.GP5, board.GP6, board.GP7,     #addr
    board.GP8, board.GP9, board.GP10, board.GP11,   #addr
    board.GP12, board.GP13, board.GP14, board.GP15, #data
    board.GP16, board.GP17, board.GP18, board.GP19, #data
    board.GP20, board.GP21, board.GP22,             #x,x,clock
    board.GP26, board.GP27, board.GP28
]

def set_pins( beg, end, dir = 0 ):
    for p in range( beg, end ):
        pin = pins[ p ]
        print( p )
        pp = digitalio.DigitalInOut( pin )
        if dir == 0:
            pp.direction = digitalio.Direction.INPUT
            pp.pull = digitalio.Pull.UP
        else:
            pp.direction = digitalio.Direction.OUTPUT

set_pins( 0, 14, 0 )
set_pins( 14, 23, 1 )

base = 0xd0000000

gpio_in = base + 0x04
gpio_out = base + 0x10
gpio_out_set = base + 0x18
gpio_out_clr = base + 0x20
gpio_out_xor = base + 0x28
gpio_hi_out_clr = base + 0x24
gpio_oe = base + 0x30
gpio_oe_set = base + 0x38
gpio_oe_clr = base + 0x40

def binunder( v ):
    s = f"{v:032b}"
    u = ""
    out = ""
    i = 0
    for i in range( len( s ), 0, -4 ):
        out = u + s[i:i+4] + out
        u = "_"
    out = s[0:i] + out
    return out

print( binunder( 0 ) )

def read_mem( addr ):
    mem = memorymap.AddressRange( start=addr, length=4 )
    return int.from_bytes( mem[ 0 : 4 ], "little" )

def read2( addr ):
    mem = memorymap.AddressRange( start=addr, length=2 )
    return mem[4]
    #return int.from_bytes( mem[ 0 : 4 ], "little" )

def write_mem( addr, v ):
    if addr == gpio_out:
        v = v | 0b0000_0000_1000_0000_0000_0000_0000_0000
        #print( f"{addr:08x} " + binunder( v ) )
    mem = memorymap.AddressRange( start=addr, length=4 )
    mem[0:4] = v.to_bytes( 4, "little" )

def get_addr():
    return read_mem( gpio_in ) & 0b0000_1111_1111_1111

def get_mreq():
    return ( read_mem( gpio_in ) & 0b0010_0000_0000_0000 ) != 0

def get_rd():
    return ( read_mem( gpio_in ) & 0b0001_0000_0000_0000 ) != 0


oe = read_mem( gpio_oe )
print( binunder( oe ) )

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT



#write_mem( gpio_out_set, 0b0011_0000_0000_0000 )
v = read_mem( gpio_out )
print( binunder( v ) )

oe = read_mem( gpio_oe )
print( binunder( oe ) )

ba = bytearray( 4096 )
for i in range( 4096 ):
    ba[i] = 0

ba[0] = 0x21
ba[1] = 0x00
ba[2] = 0x01
ba[3] = 0x00
ba[4] = 0x00
ba[5] = 0x77
ba[6] = 0x3c
ba[7] = 0xc3
ba[8] = 0x00
ba[9] = 0x00

#          c00|   data  |     addr     |

lastaddr = -1
last_mreq = 0
addr = 0
data = 0
clock = 0
clockmask =             0b0000_0000_0100_0000_0000_0000_0000_0000
write_mem( gpio_oe_set, 0b0000_0010_0100_0000_0000_0000_0000_0000 ) # disable output of data
#write_mem( gpio_oe_set, 0b0000_0010_0111_1111_1100_0000_0000_0000 ) # enable output of data
delay = 0.0001
doit = True
while True:
    # clock is high
    if not get_mreq():
        #print( "mreq active ..." )
        addr = get_addr()
        if not get_rd():
            # read
            if doit:
                doit = False
                data = ba[addr]
                #print( f"read addr: {addr:04x} data: {data:02x}" )
                write_mem( gpio_oe_set, 0b0000_0010_0111_1111_1100_0000_0000_0000 ) # enable output
                data = data << 14
                v = clock | data
                write_mem( gpio_out, v )
        else:
            # write
            if doit:
                doit = False
                #write_mem( gpio_oe_set,     0b0000_0010_0100_0000_0000_0000_0000_0000 ) # enable input
                write_mem( gpio_oe_clr,     0b0000_0000_0011_1111_1100_0000_0000_0000 ) # enable input
                v = ( read_mem( gpio_in ) & 0b0000_0000_0011_1111_1100_0000_0000_0000 ) >> 14
                ba[addr] = v
                if ( v % 16 ) == 0:
                    print( f"write addr: {addr:04x} data: {v:02x}" )
    else:
        #print( "mreq not active ..." )
        #write_mem( gpio_oe_set, 0b0000_0010_0100_0000_0000_0000_0000_0000 ) # disable output
        write_mem( gpio_oe_clr,     0b0000_0000_0011_1111_1100_0000_0000_0000 ) # enable input
        doit = True
    # set clock low
    clock = clockmask
    v = clock | data
    #print( f"{addr:08x}  " + binunder( v ) )
    write_mem( gpio_out, v )
    led.value = True
    time.sleep( delay )
    # set clock high
    clock = 0
    v = clock | data
    #print( f"{addr:08x}  " + binunder( v ) )
    write_mem( gpio_out, v )
    led.value = False
    #time.sleep( delay )

while True:
    addr = get_addr()
    if addr != lastaddr:
        data = ba[addr] << 14
        v = data | clock
        write_mem( gpio_out, v )
        addr_ = binunder( v )
        print( f"{addr_:s} {addr:04x} {v:02x}" )
        lastaddr = addr
    led.value = True
    #write_mem( gpio_out, 0b1111_1111_0000_0000_0000 )
    time.sleep(0.5)
    led.value = False
    #write_mem( gpio_out, 0b0000_0000_0000_0000_0000 )
    time.sleep(0.5)
    #write_mem( gpio_oe_set, 0b0000_0010_0000_1111_1111_0000_0000_0000 )
    clock = clock | clockmask
    v = clock | data
    print( f"{addr:08x}  " + binunder( v ) )
    write_mem( gpio_out, v )
    #write_mem( gpio_oe_set, 0b0000_0010_0000_0000_0000_0000_0000_0000 )




