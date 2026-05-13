# Trace Functions

The trace functions are currently under development.  They have already been written and used in the original javascript version.

Due to the limited memory of microcontrollers, the amount of memory used for the trace functions will not cover the full 64k address space of 8 bit microprocessors.  I have therefore reduced the trace arrays to one quarter of the size, 16k.  It is therefore necessary to trace one portion at a time.  The start mem address for the trace can be set, then all addresses from this point to 4k above it will be traced.

- cclr    ; clear the trace data
- cset    ; set/show the trace start address
- cshow   ; display the trace data

The trace output has not yet been fully implemented as I am still working on the disassembler translation.


