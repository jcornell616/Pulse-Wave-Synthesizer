/*
 * File:   main.c
 * Author: jacks
 *
 * Created on February 16, 2021, 1:19 PM
 */
#pragma config RSTOSC = HFINTOSC_64MHZ
#pragma config WDTE = OFF

#include <pic18f47k40.h>
#include <xc.h>

#define _XTAL_FREQ 64000000

#define CL          PORTCbits.RC5
#define C_          PORTBbits.RB1
#define D           PORTCbits.RC6
#define D_          PORTBbits.RB2
#define E           PORTDbits.RD4
#define F           PORTBbits.RB3
#define F_          PORTCbits.RC7
#define G           PORTDbits.RD5
#define G_          PORTBbits.RB4
#define A           PORTDbits.RD6
#define A_          PORTBbits.RB5
#define B           PORTDbits.RD7
#define CH          PORTDbits.RD3
#define peak        PORTCbits.RC2
#define octave_sel  PORTBbits.RB0


//constants
const char message1[] = "Jackson's Synth";
const char oct_mssg[] = "Octave: ";

//variables
volatile char high;
volatile char low;
volatile char on;
volatile char cnt;
volatile char amp;
volatile char period;
volatile char duty_cyc;
volatile char octave;
volatile char prev_octave;

//function declarations
void InitGPIO(void);
void dac_init(void);
void ADC_C0_init(void);
void ADC_C1_init(void);
void dac_write(char data);
int get_ADC_C0(void);
int get_ADC_C1(void);
void GetNote(void);
void AudioOut(void);
void lcd_command(char x);
void lcd_char(char x);
void lcd_init(void);
void print(const char str[], int size);
void PrintOctave(void);

/**  MAIN PROGRAM **/
void main(void) {
    
    //initialize peripherals
    InitGPIO();
    dac_init();
    ADC_C0_init();
    ADC_C1_init();
    lcd_init();
    
    //initialize variables
    high = 0;
    low = 1;
    on = 0;
    cnt = 0;
    amp = 0;
    period = 0;
    duty_cyc = 0;
    octave = 0;
    prev_octave = 0xFF;
    
    //print to LCD
    print(message1, 15);
    __delay_ms(2500);
    __delay_ms(2500);
    
    while(1) {\
        //read note
        GetNote();
        
        //read amplitude input
        amp = 0xFF - (char) (get_ADC_C0()>>2);
        duty_cyc = (char)((get_ADC_C1()*period)>>10);
        //read octave
        octave = octave_sel;
        
        //calculate audio out
        if ((on) && (cnt == duty_cyc<<octave)) {
            low = 1;
            on = 0;
            cnt++;
        }
        else if ((!on) && (cnt == period<<octave)) {
            high = 1;
            on = 1;
            cnt = 0;
        }
        else {
            cnt++;
        }
        AudioOut();
        
        //set LCD status
        if (octave != prev_octave) {
            PrintOctave();
            prev_octave = octave;
        }
    }
    
    return;
}

//functions

//configure GPIO pins
void InitGPIO(void) {
    //Set pins as digital
    ANSELBbits.ANSELB0 = 0;
    ANSELBbits.ANSELB1 = 0;
    ANSELBbits.ANSELB2 = 0;
    ANSELBbits.ANSELB3 = 0;
    ANSELBbits.ANSELB4 = 0;
    ANSELBbits.ANSELB5 = 0;
    ANSELCbits.ANSELC5 = 0;
    ANSELCbits.ANSELC6 = 0;
    ANSELCbits.ANSELC7 = 0;
    ANSELDbits.ANSELD3 = 0;
    ANSELDbits.ANSELD4 = 0;
    ANSELDbits.ANSELD5 = 0;
    ANSELDbits.ANSELD6 = 0;
    ANSELDbits.ANSELD7 = 0;
    ANSELEbits.ANSELE0 = 0;
    ANSELEbits.ANSELE1 = 0;
    ANSELEbits.ANSELE2 = 0;
    //Set pins as inputs
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;
    TRISCbits.TRISC5 = 1;
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    TRISDbits.TRISD3 = 1;
    TRISDbits.TRISD4 = 1;
    TRISDbits.TRISD5 = 1;
    TRISDbits.TRISD6 = 1;
    TRISDbits.TRISD7 = 1;
    TRISEbits.TRISE0 = 1;
    TRISEbits.TRISE1 = 1;
    TRISEbits.TRISE2 = 1;
    //Set pins as outputs
    TRISCbits.TRISC2 = 0;
}
//configure DAC
void dac_init(void) {
    ANSELCbits.ANSELC3 = 0; //Digital pin
    ANSELCbits.ANSELC4 = 0; //Digital pin
    TRISCbits.TRISC3 = 0; // set direction register
    TRISCbits.TRISC4 = 0;
    TRISDbits.TRISD2 = 0;
    RC3PPS = 0x0F; //map pins
    RC4PPS = 0x10;
    PORTDbits.RD2 = 1;   // enable to 1
    //SPI master
    SSP1CON1 = 0b101010;
    SSP1STATbits.CKE = 1; //data transition from active to idle
    //baud rate
    SSP1ADD = 71;
    PMD4 &= 0b01111;
}
//init C0 as ADC
void ADC_C0_init(void) {
    ADCON0bits.ADFM = 1; //right justify
    ADCON0bits.ADCS = 1; //FRC Clock
    TRISCbits.TRISC0 = 1; //Set RC0 to input
    ANSELCbits.ANSELC0 = 1; //Set RC0 to analog
    ADCON0bits.ADON = 1; //Turn ADC On
}
//init C1 as ADC
void ADC_C1_init(void) {
    ADCON0bits.ADFM = 1; //right justify
    ADCON0bits.ADCS = 1; //FRC Clock
    TRISCbits.TRISC1 = 1; //Set RC1 to input
    ANSELCbits.ANSELC1 = 1; //Set RC1 to analog
    ADCON0bits.ADON = 1; //Turn ADC On
}
//get ADC value
int get_ADC_C0(void) {
    ADPCH = 0b010000; //RC4 is Analog channel
    ADCON0bits.GO = 1; //Start conversion (ADGO = 1)
    while (ADCON0bits.GO); //Poll ADG0 bit
    char resultHigh = ADRESH; //Retrieve results
    char resultLow = ADRESL;
    return (int) (resultHigh<<8) | resultLow;
}
//get ADC value
int get_ADC_C1(void) {
    ADPCH = 0b010001; //RC4 is Analog channel
    ADCON0bits.GO = 1; //Start conversion (ADGO = 1)
    while (ADCON0bits.GO); //Poll ADG0 bit
    char resultHigh = ADRESH; //Retrieve results
    char resultLow = ADRESL;
    return (int) (resultHigh<<8) | resultLow;
}
//write serially to DAC
void dac_write(char data){
    char data_hi;
    char data_lo;
    //enable low
    PORTDbits.RD2 = 0;
    data_hi = 0xF0 | ((data>>4) & 0x0F);
    data_lo = 0xF0 & (data<<4);
    SSP1BUF = data_hi; //send data
    while(!PIR3bits.SSP1IF);
    PIR3bits.SSP1IF = 0;
    SSP1BUF = data_lo;
    while(!PIR3bits.SSP1IF);
    PIR3bits.SSP1IF = 0;
    //enable high
    PORTDbits.RD2 = 1;
}
//set timer based off note being played
void GetNote(void) {
    if (CH) {
        period = 23;
    }
    else if (B) {
        period = 24;    
    }
    else if (A_) {
        period = 25;    
    }
    else if (A) {
        period = 27;    
    }
    else if (G_) {
        period = 28;    
    }
    else if (G) {
        period = 30;
    }
    else if (F_) {
        period = 35;    
    }
    else if (F) {
        period = 31;    
    }
    else if (E) {
        period = 33;    
    }
    else if (D_) {
        period = 37;    
    }
    else if (D) {
        period = 39;    
    }
    else if (C_) {
        period = 42;    
    }
    else if (CL) {
        period = 44;    
    }
    else {
        period = 255;
    }
}
//calculate audio out
void AudioOut(void) {
    //INTCONbits.GIE = 0;
    if (high) {
        if (period != 255) {
            dac_write(amp);
        }
        if ((amp > 0xF0) && (period != 255)) {
            peak = 1;
        }
        else {
            peak = 0;
        }
        high = 0;
    }
    else if (low) {
        dac_write(0);
        low = 0;
    }
}
//send command to LCD
void lcd_command(char x){
    char temp;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA6 = 0;
    temp = x;
    PORTA = 0;
    __delay_ms(5);
    x = x >>4;
    x = x & 0xF;
    x = x | 0x40;
    PORTA = x;
    __delay_ms(5);
    x = x & 0xF;
    PORTA = x;
    __delay_ms(5);
    PORTA = 0;
    //__delay_ms(5);
    x = temp;
    x = x & 0xF;
    x = x | 0x40;
    PORTA = x;
    __delay_ms(5);
    x = x & 0xF;
    PORTA = x;
}
//send character to LCD
void lcd_char(char x){
    char temp;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA6 = 0;
    temp = x;
    PORTA = 0x10;
    __delay_ms(5);
    x = x >>4;
    x = x & 0xF;
    x = x | 0x50;
    PORTA = x;
    __delay_ms(5);
    x = x & 0x1F;
    PORTA = x;
    __delay_ms(5);
    PORTA = 0x10;
    __delay_ms(5);
    x = temp;
    x = x & 0xF;
    x = x | 0x50;
    PORTA = x;
    __delay_ms(5);
    x = x & 0x1F;
    PORTA = x;
}
//initialize LCD
void lcd_init(void){
    lcd_command(0x33);
    lcd_command(0x32);
    lcd_command(0x2C);
    lcd_command(0x0C);
    lcd_command(0x01);
}
//print string to LCD
void print(const char str[], int size) {
    for (int i = 0; i < size; i++) {
        lcd_char(str[i]);
        lcd_command(0x06);
    }
}
//print octave
void PrintOctave(void) {
    char temp = !(octave) + 0x31;
    lcd_command(0x01);
    print(oct_mssg, 8);
    lcd_char(temp);
}