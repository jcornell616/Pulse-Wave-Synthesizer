# Pulse-Wave Synthesizer

## Summary

The design implements a two-octave, 13 key synthesizer that outputs a square wave
with adjustable pulse width, volume, and gain. The keys are implemented using pushbuttons,
and the octave is selected using a switch. The octave being played is outputted to the LCD, and
an LED turns on if the signal is at risk of clipping. The sound is amplified to a speaker using the
LM386. 

## Design

The design uses a PIC18F47 microprocessor to process all digital inputs and outputs.
Two potentiometers are used to interface with the processor’s ADC, 13 pushbuttons and a
switch are digital inputs, and an LED, the LTC1661 DAC, and an LCD are digital outputs. The
board can be powered either by a 9V battery or by the DAD. An LM386 op-amp amplifies the
output of the LTC1661 to an 8 ohm speaker, with a third potentiometer used to control the
gain.

The software continuously checks which buttons are being pressed, which position the
switch is in, and the analog values of the potentiometers. These inputs are used to calculate the
output frequency, pulse width, and amplitude. The software ensures that the signal goes high
for ‘pulse-width’ amount of time and low for ‘period – pulse-width’ amount of time. When high,
the value written to the DAC is a value between 0x00 and 0xFF as controlled by an ADC input. If
the octave select changes, the new octave value (either 1 or 2) is outputted to the LCD.

## Bugs

The header for the snap-in programmer has a pin not connected to ground, and the data
pins for the 16x2 LCD were routed to the wrong pins. In both cases, the correct pins were wire
wrapped to the correct locations, and in the case for the LCD, the incorrect PCB traces were cut
with a knife to disconnect the incorrect pins. Additionally, a voltage divider was used instead of
a voltage regulator to step down the 9V battery to 5V, which is poor design practice. However, this
did not cause any problems in functionality of the design. 
