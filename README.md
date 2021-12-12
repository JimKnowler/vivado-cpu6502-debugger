Vivado Project for CPU6502 Debugger
=

Vivado project for a Verilog reproduction of the 6502 CPU and some block ram.

The CPU is wrapped in a debugger framework that can be used to:
- read / write memory
- read state of the CPU
- reset the CPU
- single-step clock cycles

The Debugger can be controlled over SPI from an Arduino Due running this sketch: https://github.com/JimKnowler/arduinodue-cpu6502-debugger

The verilog code for the 6502 CPU and Debugger are from https://github.com/JimKnowler/verilog-nes

The SPI Verilog code is from https://github.com/JimKnowler/verilog-spi

ARTY A7 Pinout
=

- SPI - clk, mosi, miso, ground
- IO0 - spi chipselect (low = selected)


