#include <xc.h>  // Include device-specific header
#include <pic16f877a.h>
#include <string.h>
#define _XTAL_FREQ 20000000  // Define the oscillator frequency

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT/HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define RS PORTCbits.RC0
#define RW PORTCbits.RC4
#define EN PORTCbits.RC1

void lcd_cmd(unsigned char cmd)
{
    PORTD = (cmd & 0xF0);
    EN = 1;
    RW = 0;
    RS = 0;
    __delay_ms(10);
    EN = 0;
    PORTD = ((cmd<<4) & 0xF0);
    EN = 1;
    RW = 0;
    RS = 0;
    __delay_ms(10);
    EN = 0;
}

void lcd_data(unsigned char data)
{
    PORTD = (data & 0xF0);
    EN = 1;
    RW = 0;
    RS = 1;
    __delay_ms(10);
    EN = 0;
    PORTD = ((data<<4) & 0xF0);
    EN = 1;
    RW = 0;
    RS = 1;
    __delay_ms(10);
    EN = 0;
}

void lcd_init()
{
    lcd_cmd(0x02);
    lcd_cmd(0x28); //4bit mode and 16 columns and 2 rows of led
    lcd_cmd(0x0C);
    lcd_cmd(0x06); // auto increment
    lcd_cmd(0x01); // clear screen
    __delay_ms(20);
}

void lcd_string(const unsigned char *str, unsigned char num)
{
    unsigned char i;
    for(i=0;i<num;i++)
    {
        lcd_data(str[i]);
    }
}
void main() 
{
    TRISB = 0b00110000; //RB3 and 4 as Input PIN (ECHO)
    TRISC = 0x00;
    TRISD = 0x00;
    lcd_init();
    __delay_ms(10);
    
    int a[2] = {0,0};
    T1CON = 0x10;    
            
    while(1)    
    {       
        lcd_cmd(0x82);
        lcd_string("Distance:",9);
        __delay_ms(10);
        for (int i = 0; i < 2; i++)
        {
            TMR1H = 0;                //Sets the Initial Value of Timer
            TMR1L = 0;                //Sets the Initial Value of Timer
            
            PORTB = (1 << i); //i-th trigger
            __delay_us(10);
            PORTB = 0;
            while(!(PORTB & (0x10 <<i))); //4+i th echo
            TMR1ON = 1;
            while(PORTB & (0x10 <<i));
            TMR1ON = 0;
            a[i] = (TMR1L | (TMR1H<<8)); //Reads Timer Value
            a[i] = a[i]/50;              //Converts Time to Distance
            a[i] = a[i] + 1;                //Distance Calibration   
        
            if(i == 0){lcd_cmd(0xC0);
                lcd_string("L", 1);
                __delay_ms(5);
            lcd_cmd(0xC1);}
            if(i == 1){lcd_cmd(0xC8);
                lcd_string("R", 1);
                __delay_ms(5);
            lcd_cmd(0xC9);}
            
            if(a[i] >= 2 && a[i]<=15)        //less than 5 cm
                { 
                    //lcd_cmd(0xC0);
                    lcd_string(" < 5 cm", 7);
                    __delay_ms(50);

                    PORTCbits.RC2 = 1;
                    __delay_ms(1500);
                    PORTCbits.RC2 = 0;
                    __delay_ms(50);
                } 
            if(a[i]>=15 && a[i]<=30)        //5 - 10 cm
                {   
                    //lcd_cmd(0xC0);
                    lcd_string(" <10 cm",7);
                    __delay_ms(50);

                    PORTCbits.RC2 = 1;
                    __delay_ms(100);
                    PORTCbits.RC2 = 0;
                    __delay_ms(100);
                    PORTCbits.RC2 = 1;
                    __delay_ms(100);
                    PORTCbits.RC2 = 0;
                    __delay_ms(100);
                    PORTCbits.RC2 = 1;
                    __delay_ms(100);
                    PORTCbits.RC2 = 0;
                    __delay_ms(300);
                }

            if(a[i]>=30 && a[i]<=60)        //between 10 and 20 cm
                {   
                    //lcd_cmd(0xC0);
                    lcd_string("10-20cm",7);
                    __delay_ms(50);

                    PORTCbits.RC2 = 1;
                    __delay_ms(100);
                    PORTCbits.RC2 = 0;
                    __delay_ms(100);
                    PORTCbits.RC2 = 1;
                    __delay_ms(100);
                    PORTCbits.RC2 = 0;
                    __delay_ms(500);
                }

            if(a[i]>=60 && a[i]<=150)        //between 20 and 50 cm
                {   
                    //lcd_cmd(0xC0);
                    lcd_string("20-50cm",7);
                    __delay_ms(50);

                    PORTCbits.RC2 = 1;
                    __delay_ms(10);
                    PORTCbits.RC2 = 0;
                    __delay_ms(600);
                }

            if(a[i]>=150 || a[i]<=2)        //Further than 50 cm
                { 
                    //lcd_cmd(0xC0);
                    lcd_string("too far",7);
                    __delay_ms(50);

                }
            
        __delay_ms(400);
        }
        __delay_ms(10);
    }
    return;
}