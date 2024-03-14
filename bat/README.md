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
| R1, R2 | 10k | 0207 
| R3, R4 | 47k | 0207
| R5 | 330R | 0207 
| R6 | 300k | 0207 
| C1, C2, C3 | 47u | 6.3x5mm
| Q1, Q2, Q3 | 2N3904 | TO92 
| D1, D2 | LED, red / purple | 3 mm 
| BT1 | 2032 Battery holder | DS1092-04-B6P
