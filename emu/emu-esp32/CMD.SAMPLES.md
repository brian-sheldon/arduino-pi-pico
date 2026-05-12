# Command Samples

### Startup a CP/M Session, run Star Trek

```
>>> b
>>> on
>>> (PageUp key)
A: dir
...
D:
D: dir
...
mbasic t.bas
note: "system" exits basic
```

### Do some cpu stepping

```
>>> off             ; turn off cpu
cpu: off
>>> cclr            ; clears any cpu trace data
cpu trace data cleared ...
>>> pc 0            ; set program counter to 0
0
>>> b               ; load boot sector into page zero of mem
boot sector loaded to zero page ...
>>> state           ; check current cpu state
PC 0000 | SP fffe A 00 F 00     | AF_ 0000 |
BC 0000 | IR 0000 s 0  z 0  h 0 | BC_ 0000 |
DE 0000 | IX 0000 p 0  n 0  c 0 | DE_ 0000 |
HL 0000 | IY 0000 iff1 0 iff2 0 | HL_ 0000 |
running 0 on 0 wait 0 stop 0 halt 0
ticks 0 steps 0
>>> step            ; exec one cpu instruction
PC 0019 | SP fffe A 00 F 00     | AF_ 0000 |
BC 0000 | IR 0001 s 0  z 0  h 0 | BC_ 0000 |
DE 0000 | IX 0000 p 0  n 0  c 0 | DE_ 0000 |
HL 0000 | IY 0000 iff1 0 iff2 0 | HL_ 0000 |
running 1 on 0 wait 0 stop 0 halt 0
ticks 10 steps 1
>>> stopat fa00     ; set stop addr to CP/M start - fa00
stopat set: 1 addr: fa00
>>> on              ; turn cpu on
cpu: on
>>> state           ; check current cpu state
PC fa00 | SP 0080 A 00 F 42     | AF_ 0000 |
BC 0119 | IR 001d s 0  z 1  h 0 | BC_ 0000 |
DE 0000 | IX 0000 p 0  n 1  c 0 | DE_ 0000 |
HL fc80 | IY 0000 iff1 0 iff2 0 | HL_ 0000 |
running 0 on 1 wait 0 stop 1 halt 0
ticks 8030 steps 1053
>>> stopclr         ; clears the stop state
>>> on              ; on will start cpu again
64K CP/M Vers. 2.2 (Z80 CBIOS V1.2 for Z80SIM, Copyright 1988-2007 by Udo Munk)

A>
>>> state
PC fbc1 | SP ef2f A ff F 44     | AF_ 0000 |
BC 007f | IR 000e s 0  z 1  h 0 | BC_ 0000 |
DE e406 | IX 0000 p 1  n 0  c 0 | DE_ 0000 |
HL ef06 | IY 0000 iff1 0 iff2 0 | HL_ 0000 |
running 0 on 1 wait 1 stop 0 halt 0
ticks 363840 steps 49422
note: The cpu is not running due to being in wait state.
This is because CP/M is waiting for the user to type a
key.  This emulator automatically stops the cpu emulation
when this happens.  The wait will automatically clear as
soon as a key is typed and the cpu will run again till the
next wait ( full name is iowait ).
>>> cover           ; display cpu trace data
...
count:     1  addr: fbf4  byte: 11
count:     1  addr: fbf7  byte: 19
count:     1  addr: fbf8  byte: c9
count:    17  addr: fc05  byte: 79
count:    17  addr: fc06  byte: d3
count:    17  addr: fc08  byte: c9
count:    16  addr: fc09  byte: 79
count:    16  addr: fc0a  byte: d3
count:    16  addr: fc0c  byte: c9
count:    16  addr: fc0d  byte: 7a
count:    16  addr: fc0e  byte: b3
count:    16  addr: fc0f  byte: c2
count:    16  addr: fc18  byte: eb
count:    16  addr: fc19  byte: 09
count:    16  addr: fc1a  byte: 6e
count:    16  addr: fc1b  byte: 26
count:    16  addr: fc1d  byte: c9
count:    34  addr: fc1e  byte: 79
count:    34  addr: fc1f  byte: d3
...
note: A small section of the cpu trace.  The cpu trace
output is not complete as I haven't finished translating
the z80 disassembler from javascript to c.  Once done,
this will also be used by the state, step and disass cmds.
```

### Examine the disk images

```
>>> drvs         ; list configured disks
drv: 0 img: 0 path: /emu/disks/cpm22-1.dsk
drv: 1 img: 1 path: /emu/disks/cpm22-2.dsk
drv: 2 img: 2 path: /emu/disks/8080tools.cpm
drv: 3 img: 3 path: /emu/disks/trek.cpm
drv: 4 img: 4 path:
drv: 5 img: 5 path:
drv: 6 img: 6 path:
drv: 7 img: 7 path:
drv: 8 img: 8 path:
drv: 9 img: 9 path:
>>> drv 0        ; set current drive
drv: 0
>>> disk 0 1     ; view trk 0 sec 1 where boot sec is located
path: /emu/disks/cpm22-1.dsk
drv: 0  trk: 0  log: 1  sec: 1
blk: --:--  op: read  size: 128  chksum: ---
0000 c319 0042 4f4f 543a 2065 7272 6f72 2062 ...BOOT: error b
0010 6f6f 7469 6e67 0d0a 0001 0200 1632 2100 ooting.......2!.
0020 e43e 00d3 0a78 d30b 79d3 0c7d d30f 7cd3 .>...x..y..}..|.
0030 10af d30d db0e b7ca 4a00 2103 007e b7ca ........J.!..~..
0040 4800 d301 23c3 3d00 f376 15ca 00fa 3180 H...#.=..v....1.
0050 0039 0c79 fe1b da25 000e 0104 c325 0000 .9.y...%.....%..
0060 0000 0000 0000 0000 0000 0000 0000 0000 ................
0070 0000 0000 0000 0000 0000 0000 0000 0000 ................
>>> disk 2 1       ; view sector 2 1 where dir is located
path: /emu/disks/cpm22-1.dsk
drv: 0  trk: 2  log: 1  sec: 1
blk: 0:1  op: read  size: 128  chksum: ---
0000 0044 554d 5020 2020 2043 4f4d 0000 0003 .DUMP    COM....
0010 0200 0000 0000 0000 0000 0000 0000 0000 ................
0020 0053 4449 5220 2020 2043 4f4d 0000 0077 .SDIR    COM...w
0030 0817 1a1b 1ca8 c7c8 c9ca cbce e2e3 eb00 ................
0040 0053 5542 4d49 5420 2043 4f4d 0000 000a .SUBMIT  COM....
0050 0b0c 0000 0000 0000 0000 0000 0000 0000 ................
0060 0045 4420 2020 2020 2043 4f4d 0000 0034 .ED      COM...4
0070 1011 1213 1415 1600 0000 0000 0000 0000 ................
note: typing in the disk cmd without any parameter will view
the next relevant sector.  For example, if at boot, the next
sector will be the next physical sector.  If at directory,
the next sector will be the next logical sector as trks above
2 use a translate table that help with disk performance, so
the sectors are not in physical order.  Plus this is also the
start of where disk blocks for file blocks.  This is why the
blk: 0:1 is shown, this is block 0 sector 1.  In this disk,
there are 8 sectors per block.  i haven't added the chksum
function yet.
```


