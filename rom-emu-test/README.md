
# Experimenting with using a Raspberry Pi Pico 2 W to emulate a ROM

I wanted to initially start with the commonly used digitalio arduino functions to get an idea of the speed.  After getting it working, the max speed was 0.123 mhz.

The next step was to use the gpio register functions to read/write multiple pins at once.  After gradually replacing these functions in the code, the speed gradually increased from 0.123 mhz to 1.170 mhz.  The code indicates the speed increase during various changes.

The setup is using a Raspberry Pi Pico 2 W connected to a Z80 cpu.  The pins are connected to the Z80 cpu as follows.

- Z80 address pins 0 - 10 --> gpio 0 - 10, allows for up to 2048k
- Z80 wr pin --> gpio 11
- Z80 rd pin --> gpio 12
- Z80 mreq pin --> gpio 13
- Z80 data pins --> gpio 14 - 21
- Z80 clock pin --> gpio 22

![pi.pico.z80.jpg](pi.pico.z80.jpg)


