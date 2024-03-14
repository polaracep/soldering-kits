# Bat 

A bat with red glowing eyes


**Difficulty:** :star:  :black_circle: :black_circle: 

### About

The circuit consists of an astable multivibrator with a low-pass RC filter driving two LEDs through a transistor.
The astable multivibrator generates a distorted ramp wave, suitable for driving LEDs.
The low-pass filter smoothens the voltage, giving the impression of the bat's eyes slowly blinking ominously.


When ordering the parts on LCSC

### Quirks:

The circuit can take up to 10 seconds before it starts blinking. Maybe could be fixed?

### BOM

| Ref | Value | Package | 
| --- | --- | --- |
| R1, R2 | 10k, 0207 | 110-097
| R3, R4 | 47k, 0207 | 110-113 
| R5 | 330R, 0207 | 110-061
| R6 | 300k, 0207 | 110-132 
| C1, C2, C3 | 47u, 6.3x5 | 123-373 
| Q1, Q2, Q3 | 2N3904, TO92 | 215-003 
| D1, D2 | LED, red 5 mm | R: 518-278, FIAL: 518-519
| BT1 | 2032 Holder | 819-157 

