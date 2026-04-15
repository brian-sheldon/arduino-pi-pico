
# Experimenting with using a Raspberry Pi Pico 2 W to emulate ROM, RAM and CLOCK

### Current status: speed 2.28 Mhz, moved main loop into loop1 ( runs on second core ), removed more unnecessary code, all status is printed in loop ( runs on first core )

A test of using a Pico to emulate ROM, RAM and CLOCK.  I decided to quickly try this myself before I receive my ONE-ROMs ( Fire 28 and Fire 40 ) by Piers Rocks, just received them but not tested.  One disadvantage of the using the stand Pi Pico is that not all the gpio pins are made available for use.  So even though the ONE ROM has the same pico chip, it is using more of the gpios available on this chip.  I am only using 11 address lines for this reason, whereas the ONE-ROM Fire 28 that I orders can handle 16 address lines I believe.  I was also limited as I am using a gpio as a clock instead of an external clock and at least one additional gpio as I am not using any external logic chips to reduce the number of gpios needed.  By also controlling the clock, it does make some aspects of doing this a little easier.  This is just meant to be a quick experiment, so not worried to get this to work with more memory.  Note:  Actually, the current full speed version does not have write protection, so it technically only emulates RAM.  It would be an easy change, but that would slow it down a bit.

I wanted to initially start with the commonly used digitalio arduino functions to get an idea of the speed.  After getting it working, the max speed was 0.123 mhz.

The next step was to use the gpio register functions to read/write multiple pins at once.  After gradually replacing these functions in the code, the speed gradually increased from 0.123 mhz to 1.170 mhz.  The code indicates the speed increase during various changes.

Some additional code changes and particularly placing a while loop inside the loop() function made a significant difference.

The setup is using a Raspberry Pi Pico 2 W connected to a Z80 cpu.  The pins are connected to the Z80 cpu as follows.

- Z80 address pins 0 - 10 --> gpio 0 - 10, allows for up to 2048k
- Z80 wr pin --> gpio 11
- Z80 rd pin --> gpio 12
- Z80 mreq pin --> gpio 13
- Z80 data pins --> gpio 14 - 21
- Z80 clock pin --> gpio 22

![pi.pico.z80.jpg](pi.pico.z80.jpg)

I uploaded the original python version of this.  Not much time was put into it, I basically got it working and wanted to try an Arduino version.  The python and the arduino versions were all done yesterday with a few minor changes today, so I haven't put any time into refining the code.


