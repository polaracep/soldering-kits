
// CONFIG
#pragma config FOSC = INTOSC // Oscillator Selection bits (INTOSC oscillator:
// CLKIN function disabled)
#pragma config BOREN = OFF // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config WDTE = OFF  // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE =                                                         \
    OFF // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP =                                                            \
    OFF // Code Protection bit (Program memory code protection is disabled)
#pragma config LVP = OFF // Low-Voltage Programming Enable (High-voltage on
// MCLR/VPP must be used for programming)
#pragma config LPBOR = OFF // Brown-out Reset Selection bits (BOR disabled)
#pragma config BORV = LO   // Brown-out Reset Voltage Selection (Brown-out Reset
// Voltage (Vbor), low trip point selected.)
#pragma config WRT =                                                           \
    OFF // Flash Memory Self-Write Protection (Write protection off)

#include <builtins.h>
#include <pic.h> /* For true/false definition */
#include <xc.h>
// #include <pic10f322.h>
#include <stdbool.h>
#include <stdint.h> /* For uint8_t definition */

#define _XTAL_FREQ 2000000

// This means, that max ratio of on/off time is 64:1
// although 16 would be enough, I found it better with 64 and then div by 4
#define PWM_RESOLUTION 64

#define LED_RED(x) PWM2CONbits.PWM2OE = x
#define LED_GREEN(x) PWM1CONbits.PWM1OE = x
#define LED_BLUE(x) TRISAbits.TRISA2 = !x

// counts pwm
uint8_t sw_pwm_counter = 1;

// default duty cycle
uint8_t sw_pwm_duty = 1;

// ============
// Periph. init
// ============

void pwm_init(void) {

  PWM1CON = 0x0; // Clear PWM1CON
  PWM2CON = 0x0; // Clear PWM2CON

  PR2 = PWM_RESOLUTION - 1;
  // Enable PWM Module, Module Output
  PORTA = 0x3;
  PWM1CON = 0b11010000;
  PWM2CON = 0b11010000;

  // Clear duty cycle registers
  PWM1DCH = 0x0;
  PWM1DCL = 0x0;
  PWM2DCH = 0x0;
  PWM2DCL = 0x0;

  // Set prescaler to 1, turn timer on
  // T2CON = 0b100; //(1 << 2);
  T2CONbits.TMR2ON = 1;

  // Enable the pwm pin output
  TRISA = 0x0;

  // Set duty cycle to 40
  PWM1DCH = 0b00001010;
  PWM1DCL = 0b00000000;
  PWM2DCH = 0b00001010;
  PWM2DCL = 0b00000000;

  return;
}

void interrupt_init(void) {
  // for sw pwm
  PIE1bits.TMR2IE = 1;

  // button on positive edge
  IOCAPbits.IOCAP3 = 1;

  // Intcon: PEIE, IOCIE, GIE
  INTCON = 0b11001000;
  // INTCONbits.PEIE = 1;
  // INTCONbits.IOCIE = 1;
  // INTCONbits.GIE = 1;
}

// ====================
// RGB helper functions
// ====================

void blue_brightness(uint8_t b) { sw_pwm_duty = (b * 4); }

void red_brightness(uint8_t b) { PWM2DCH = (b * 4); }

void green_brightness(uint8_t b) { PWM1DCH = (b * 4); }

// ===================
// Animation variables
// ===================

// the number of current animation
uint8_t animation_index = 0;
uint8_t changed = 0;

// ========================
// Animation helper structs
// ========================

/* instruction_t is used for storing an instruction
 * R,G,B: (0-15) brightness of corresponding LED color (0=off, 15=max)
 * t: 'On' time period. from 100ms to 1500ms
 */
struct instruction_t {
  uint8_t t : 4;
  uint8_t b : 4;
  uint8_t g : 4;
  uint8_t r : 4;
};

/* animation_t is used for storing whole animations, so the intepreter
 * knows its size during runtime
 * size: number of instructions in an animation
 * start_ptr: pointer to the start of an animation
 */
struct animation_t {
  uint8_t size;
  const struct instruction_t *start_ptr;
};

// ==========
// Animations
// ==========

// changes R -> G -> B
/*
const struct instruction_t blink_src[6] = {
    {0b1011, 0b0000, 0b0000, 0b1000},
    {0b1011, 0b0000, 0b1000, 0b1000},
    {0b1011, 0b0000, 0b1000, 0b0000},
    {0b1011, 0b1000, 0b1000, 0b0000},
    {0b1011, 0b1000, 0b0000, 0b0000},
    {0b1011, 0b1000, 0b0000, 0b1000}
};
const struct animation_t blink = {6, &blink_src};
*/

// heartbeat animation
const struct instruction_t heart_src[12] = {
    {0b0001, 0b0000, 0b0000, 0b1001}, {0b0010, 0b0000, 0b0000, 0b1111},
    {0b0001, 0b0000, 0b0000, 0b0111}, {0b0010, 0b0000, 0b0000, 0b0010},
    {0b0001, 0b0000, 0b0000, 0b0100}, {0b0010, 0b0000, 0b0000, 0b1001},
    {0b0010, 0b0000, 0b0000, 0b1001}, {0b0001, 0b0000, 0b0000, 0b0110},
    {0b0001, 0b0000, 0b0000, 0b0100}, {0b0001, 0b0000, 0b0000, 0b0011},
    {0b0100, 0b0000, 0b0000, 0b0010}, {0b0001, 0b0000, 0b0000, 0b0001}};
const struct animation_t heart = {12, &heart_src};

// ~flowing~  r->g->b->r->...
const struct instruction_t liquid_src[48] = {
    {0b0001, 0b0000, 0b0000, 0b1111}, // 0
    {0b0001, 0b0000, 0b0001, 0b1110}, //
    {0b0001, 0b0000, 0b0010, 0b1101}, //
    {0b0001, 0b0000, 0b0011, 0b1100}, //
    {0b0001, 0b0000, 0b0100, 0b1011}, //
    {0b0001, 0b0000, 0b0101, 0b1010}, //
    {0b0001, 0b0000, 0b0110, 0b1001}, //
    {0b0001, 0b0000, 0b0111, 0b1000}, //
    {0b0001, 0b0000, 0b1000, 0b0111}, // 0
    {0b0001, 0b0000, 0b1001, 0b0110}, //
    {0b0001, 0b0000, 0b1010, 0b0101}, //
    {0b0001, 0b0000, 0b1011, 0b0100}, //
    {0b0001, 0b0000, 0b1100, 0b0011}, //
    {0b0001, 0b0000, 0b1101, 0b0010}, //
    {0b0001, 0b0000, 0b1110, 0b0001}, //
    {0b0001, 0b0000, 0b1111, 0b0000}, //
    {0b0001, 0b0000, 0b1110, 0b0000}, // 0
    {0b0001, 0b0001, 0b1101, 0b0000}, //
    {0b0001, 0b0010, 0b1100, 0b0000}, //
    {0b0001, 0b0011, 0b1011, 0b0000}, //
    {0b0001, 0b0100, 0b1010, 0b0000}, //
    {0b0001, 0b0101, 0b1001, 0b0000}, //
    {0b0001, 0b0110, 0b1000, 0b0000}, //
    {0b0001, 0b0111, 0b0111, 0b0000}, //
    {0b0001, 0b1000, 0b0110, 0b0000}, // 0
    {0b0001, 0b1001, 0b0101, 0b0000}, //
    {0b0001, 0b1010, 0b0100, 0b0000}, //
    {0b0001, 0b1011, 0b0011, 0b0000}, //
    {0b0001, 0b1100, 0b0010, 0b0000}, //
    {0b0001, 0b1101, 0b0001, 0b0000}, //
    {0b0001, 0b1110, 0b0000, 0b0000}, //
    {0b0001, 0b1111, 0b0000, 0b0000}, //
    {0b0001, 0b1110, 0b0000, 0b0000}, // 0
    {0b0001, 0b1101, 0b0000, 0b0001}, //
    {0b0001, 0b1100, 0b0000, 0b0010}, //
    {0b0001, 0b1011, 0b0000, 0b0011}, //
    {0b0001, 0b1010, 0b0000, 0b0100}, //
    {0b0001, 0b1001, 0b0000, 0b0101}, //
    {0b0001, 0b1000, 0b0000, 0b0110}, //
    {0b0001, 0b0111, 0b0000, 0b0111}, //
    {0b0001, 0b0110, 0b0000, 0b1000}, // 0
    {0b0001, 0b0101, 0b0000, 0b1001}, //
    {0b0001, 0b0100, 0b0000, 0b1010}, //
    {0b0001, 0b0011, 0b0000, 0b1011}, //
    {0b0001, 0b0010, 0b0000, 0b1100}, //
    {0b0001, 0b0001, 0b0000, 0b1101}, //
    {0b0001, 0b0000, 0b0000, 0b1110}, //
    {0b0001, 0b0000, 0b0000, 0b1111}  //
};
const struct animation_t liquid = {48, &liquid_src};

// ~flowing but blinking~  r->g->b->r->...
/*
const struct instruction_t flow_src[48] = {
    {0b0001, 0b0000, 0b0000, 0b0001}, // 0
    {0b0001, 0b0000, 0b0000, 0b0011}, //
    {0b0001, 0b0000, 0b0000, 0b0101}, //
    {0b0001, 0b0000, 0b0000, 0b0111}, //
    {0b0001, 0b0000, 0b0000, 0b1001}, // 0
    {0b0001, 0b0000, 0b0000, 0b1011}, //
    {0b0001, 0b0000, 0b0000, 0b1101}, //
    {0b0001, 0b0000, 0b0000, 0b1111}, //
    {0b0001, 0b0000, 0b0000, 0b1101}, // 0
    {0b0001, 0b0000, 0b0000, 0b1011}, //
    {0b0001, 0b0000, 0b0000, 0b1001}, //
    {0b0001, 0b0000, 0b0000, 0b0111}, //
    {0b0001, 0b0000, 0b0000, 0b0101}, // 0
    {0b0001, 0b0000, 0b0000, 0b0011}, //
    {0b0001, 0b0000, 0b0000, 0b0001}, //
    {0b0001, 0b0000, 0b0000, 0b0000}, //
    {0b0001, 0b0000, 0b0001, 0b0000}, //
    {0b0001, 0b0000, 0b0011, 0b0000}, //
    {0b0001, 0b0000, 0b0101, 0b0000}, //
    {0b0001, 0b0000, 0b0111, 0b0000}, //
    {0b0001, 0b0000, 0b1001, 0b0000}, //
    {0b0001, 0b0000, 0b1011, 0b0000}, //
    {0b0001, 0b0000, 0b1101, 0b0000}, //
    {0b0001, 0b0000, 0b1111, 0b0000}, //
    {0b0001, 0b0000, 0b1101, 0b0000}, //
    {0b0001, 0b0000, 0b1011, 0b0000}, //
    {0b0001, 0b0000, 0b1001, 0b0000}, //
    {0b0001, 0b0000, 0b0111, 0b0000}, // 0
    {0b0001, 0b0000, 0b0101, 0b0000}, //
    {0b0001, 0b0000, 0b0011, 0b0000}, //
    {0b0001, 0b0000, 0b0001, 0b0000}, //
    {0b0001, 0b0000, 0b0000, 0b0000}, //
    {0b0001, 0b0001, 0b0000, 0b0000}, //
    {0b0001, 0b0011, 0b0000, 0b0000}, //
    {0b0001, 0b0101, 0b0000, 0b0000}, //
    {0b0001, 0b0111, 0b0000, 0b0000}, //
    {0b0001, 0b1001, 0b0000, 0b0000}, //
    {0b0001, 0b1011, 0b0000, 0b0000}, //
    {0b0001, 0b1101, 0b0000, 0b0000}, // 0
    {0b0001, 0b1111, 0b0000, 0b0000}, //
    {0b0001, 0b1101, 0b0000, 0b0000}, //
    {0b0001, 0b1011, 0b0000, 0b0000}, //
    {0b0001, 0b1001, 0b0000, 0b0000}, // 0
    {0b0001, 0b0111, 0b0000, 0b0000}, //
    {0b0001, 0b0101, 0b0000, 0b0000}, //
    {0b0001, 0b0011, 0b0000, 0b0000}, //
    {0b0001, 0b0001, 0b0000, 0b0000}, //
    {0b0001, 0b0000, 0b0000, 0b0000}  //
};
*/
// const struct animation_t flow = {48, &flow_src};

// Trikolora, w->r->b
const struct instruction_t trikolora_src[48] = {
    {0b0001, 0b0001, 0b0001, 0b0001}, // 0
    {0b0001, 0b0011, 0b0011, 0b0011}, //
    {0b0001, 0b0101, 0b0101, 0b0101}, //
    {0b0001, 0b0111, 0b0111, 0b0111}, //
    {0b0001, 0b1001, 0b1001, 0b1001}, // 0
    {0b0001, 0b1011, 0b1011, 0b1011}, //
    {0b0001, 0b1101, 0b1101, 0b1101}, //
    {0b0001, 0b1111, 0b1111, 0b1111}, //
    {0b0001, 0b1101, 0b1101, 0b1101}, // 0
    {0b0001, 0b1011, 0b1011, 0b1011}, //
    {0b0001, 0b1001, 0b1001, 0b1001}, //
    {0b0001, 0b0111, 0b0111, 0b0111}, //
    {0b0001, 0b0101, 0b0101, 0b0101}, // 0
    {0b0001, 0b0011, 0b0011, 0b0011}, //
    {0b0001, 0b0001, 0b0001, 0b0001}, //
    {0b0001, 0b0000, 0b0000, 0b0000}, //
    {0b0001, 0b0000, 0b0000, 0b0001}, //
    {0b0001, 0b0000, 0b0000, 0b0011}, //
    {0b0001, 0b0000, 0b0000, 0b0101}, //
    {0b0001, 0b0000, 0b0000, 0b0111}, //
    {0b0001, 0b0000, 0b0000, 0b1001}, //
    {0b0001, 0b0000, 0b0000, 0b1011}, //
    {0b0001, 0b0000, 0b0000, 0b1101}, //
    {0b0001, 0b0000, 0b0000, 0b1111}, //
    {0b0001, 0b0000, 0b0000, 0b1101}, //
    {0b0001, 0b0000, 0b0000, 0b1011}, //
    {0b0001, 0b0000, 0b0000, 0b1001}, //
    {0b0001, 0b0000, 0b0000, 0b0111}, // 0
    {0b0001, 0b0000, 0b0000, 0b0101}, //
    {0b0001, 0b0000, 0b0000, 0b0011}, //
    {0b0001, 0b0000, 0b0000, 0b0001}, //
    {0b0001, 0b0000, 0b0000, 0b0000}, //
    {0b0001, 0b0001, 0b0000, 0b0000}, //
    {0b0001, 0b0011, 0b0000, 0b0000}, //
    {0b0001, 0b0101, 0b0000, 0b0000}, //
    {0b0001, 0b0111, 0b0000, 0b0000}, //
    {0b0001, 0b1001, 0b0000, 0b0000}, //
    {0b0001, 0b1011, 0b0000, 0b0000}, //
    {0b0001, 0b1101, 0b0000, 0b0000}, // 0
    {0b0001, 0b1111, 0b0000, 0b0000}, //
    {0b0001, 0b1101, 0b0000, 0b0000}, //
    {0b0001, 0b1011, 0b0000, 0b0000}, //
    {0b0001, 0b1001, 0b0000, 0b0000}, // 0
    {0b0001, 0b0111, 0b0000, 0b0000}, //
    {0b0001, 0b0101, 0b0000, 0b0000}, //
    {0b0001, 0b0011, 0b0000, 0b0000}, //
    {0b0001, 0b0001, 0b0000, 0b0000}, //
    {0b0001, 0b0000, 0b0000, 0b0000}  //
};
const struct animation_t trikolora = {48, &trikolora_src};

// number of animations
#define COUNT_ANIMATIONS 3 + 1

// from here are the animations pulled from
const struct animation_t *animations[COUNT_ANIMATIONS] = {&liquid, &trikolora,
                                                          &heart};

void __interrupt() isr(void) {

  if (IOCAFbits.IOCAF3) {
    IOCAFbits.IOCAF3 = 0;

    // advancing animations
    if (animation_index == COUNT_ANIMATIONS - 1) {
      animation_index = 0;
    } else {
      animation_index++;
    }
    changed = 1;

    // hard debounce delay
    __delay_ms(300);
    IOCAFbits.IOCAF3 = 0;

  } else {
    PIR1bits.TMR2IF = 0;

    // SW PWM
    if (sw_pwm_counter == 0) {
      sw_pwm_counter = PWM_RESOLUTION;
      return;
    }
    if (sw_pwm_counter == PWM_RESOLUTION) {
      PORTAbits.RA2 = 1;
    } else if (sw_pwm_counter == sw_pwm_duty) {
      PORTAbits.RA2 = 0;
    }
    sw_pwm_counter--;
  }
}

void main(void) {

  // Configure oscillator to 2MHz
  OSCCON = (0b100 << 4);

  // init gpio
  PORTA = 0x0;
  LATA = 0x0;
  ANSELA = 0x0;

  pwm_init();
  interrupt_init();

  while (1) {
    // animation interpreter
    for (uint8_t i = 0; i != animations[animation_index]->size; i++) {
      struct instruction_t *current =
          &animations[animation_index]->start_ptr[i];

      red_brightness(current->r);
      green_brightness(current->g);
      blue_brightness(current->b);

      // 30, cuz otherwise it's not 100ms (interrupts make delay slower)
      uint8_t d = (current->t);
      for (uint8_t x = 0; x != d; x++)
        __delay_ms(30);

      if (changed) {
        changed = 0;
        break;
      }
    }
    if (animation_index == COUNT_ANIMATIONS - 1) {
      SLEEP();
      NOP();
    }
  }
}
