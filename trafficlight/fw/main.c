/*
 * File:   main.c
 * Author: Adam Beyer
 *
 * Created on July 4, 2024, 6:16 PM
 */

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

#define _XTAL_FREQ  2000000

/*
 * R: 1,74 V
 * Y: 1,8 V 
 * G: 2,2 V
 */

#define RED         PORTAbits.RA0
#define YELLOW      PORTAbits.RA1
#define GREEN       PORTAbits.RA2


/* Sekvence zelena:
 * 
 * (0)    (O)    (.)
 * (.) -> (O) -> (.)
 * (.)    (.)    (O)
 *
 */

/* Sekvence cervena:
 * 
 * (.)    (.)    (O)
 * (.) -> (O) -> (.)
 * (O)    (.)    (.)
 *
 */

void tmr2_init()
{
    PIR1bits.TMR2IF = 0;
    
    // Input frq: 7750 Hz
    // Prescaler: 5*155 = 775
    // period: 100 ms
    
    PR2 = 155;
    T2CON = ((5-1)<<3);
    
    T2CONbits.TMR2ON = 1;

}

// Blocking delay using Timer2 
void delay_100(uint8_t ms_100)
{
    for(uint8_t i = 0; i < ms_100; i++)
    {
        while(!PIR1bits.TMR2IF);
        PIR1bits.TMR2IF = 0;
    }
}

void main(void) {
    
    // Configure oscillator to 2MHz
    OSCCON = (0b000 << 4);
    // Configure oscillator to 31kHz

    
    // init gpio
    PORTA = 0x0;
    LATA = 0x0;
    ANSELA = 0x0;
    TRISA = (0x1 << 3);

    // init input
    INTCONbits.IOCIE = 1;
    IOCAPbits.IOCAP3 = 1;
            
    RED = 0;
    YELLOW = 0;
    GREEN = 0;
    
    tmr2_init();
    
    // starting on red
    RED = 1;

    // program loop
    while(1)
    {
        // waiting for button press
        while(!IOCAFbits.IOCAF3);
        // clear input interrupt
        IOCAFbits.IOCAF3 = 0;
        
        delay_100(10);
        YELLOW = 1;
        delay_100(10);
        RED = 0;
        YELLOW = 0;
        GREEN = 1;
        delay_100(20);
        // green here
        // waiting for button press
        while(!IOCAFbits.IOCAF3);
        // clear input interrupt
        IOCAFbits.IOCAF3 = 0;        
        delay_100(10);

        GREEN = 0;
        YELLOW = 1;
        delay_100(10);
        YELLOW = 0;
        RED = 1;
        // back to red
        
    }
    return;
}

