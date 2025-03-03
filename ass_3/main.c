#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

void GPIO_init(void);

void lcd_init(void);

void Printdata(unsigned char data);

void lcd_cmd(unsigned char cmd);

void lcd_string(char *str, unsigned char len);

void lcd_data(unsigned char data);

void delay(long val);

int main(void)
{
    GPIO_init();
    lcd_init();
    delay(10000000);

    while(1){
        lcd_cmd(0x80);
        lcd_string("Embedded", 8);
        lcd_cmd(0xC0);
        lcd_string("Hejsa", 5);
        while(1){
            lcd_cmd(0x1C);
            delay(1000000);
        }
    }

	return 0;
}


void GPIO_init(void){
    int dummy;

    // Enable the GPIO port that is used for the on-board LEDs and switches
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOF;

    // Do a dummy read to insert a few cycles after enabling the peripheral
    dummy = SYSCTL_RCGC2_R;

    // allow changes to PC4-7
    GPIO_PORTC_CR_R = 0xF0;
    // allow changes to PD2-3
    GPIO_PORTD_CR_R = 0x0C;

    // Set the direction as output (PC4 - PC7)
    GPIO_PORTC_DIR_R = 0xF0;
    // Set the direction as output (PD2 - PD3)
    GPIO_PORTD_DIR_R = 0x0C;

    // Enable the GPIO pins for digital function (PC4 - PC7)
    GPIO_PORTC_DEN_R = 0xF0;
    // Enable the GPIO pins for digital function (PD2 - PD3)
    GPIO_PORTD_DEN_R = 0x0C;
}

void lcd_init(void){
    lcd_cmd(0x28); // 4bit mode utilising 16 columns and 2 rows
    lcd_cmd(0x06); // autoincrementing the cursor when prints the data
    lcd_cmd(0x0E); // Cursor blinking on and display on
    lcd_cmd(0x01); // Clear screen
}

void Printdata(unsigned char data){
    //Fourth bit = D4 = PC4
    if(data & 0x10){
        GPIO_PORTC_DATA_R |= 0x10;
    } else {
        GPIO_PORTC_DATA_R &= ~(0x10);
    }

    //Fifth bit = D5 = PC5
    if(data & 0x20){
        GPIO_PORTC_DATA_R |= 0x20;
    } else {
        GPIO_PORTC_DATA_R &= ~(0x20);
    }

    //Sixth bit = D6 = PC6
    if(data & 0x40){
        GPIO_PORTC_DATA_R |= 0x40;
    } else {
        GPIO_PORTC_DATA_R &= ~(0x40);
    }

    //Seventh bit = D7 = PC7
    if(data & 0x80){
        GPIO_PORTC_DATA_R |= 0x80;
    } else {
        GPIO_PORTC_DATA_R &= ~(0x80);
    }
}

void lcd_cmd(unsigned char cmd){
    Printdata(cmd);
    GPIO_PORTD_DATA_R &= ~(0x04);
    GPIO_PORTD_DATA_R |= 0x08;
    delay(10000);
    GPIO_PORTD_DATA_R &= ~(0x08);

    Printdata(cmd << 4);
    GPIO_PORTD_DATA_R &= ~(0x04);
    GPIO_PORTD_DATA_R |= 0x08;
    delay(10000);
    GPIO_PORTD_DATA_R &= ~(0x08);
}

void lcd_string(char *str, unsigned char len){
    char i;
    for(i = 0; i < len; i++){
        lcd_data(str[i]);
    }
}

void lcd_data(unsigned char data){
    Printdata(data);
    GPIO_PORTD_DATA_R |= 0x0C;
    delay(10000);
    GPIO_PORTD_DATA_R &= ~(0x08);

    Printdata(data << 4);
    GPIO_PORTD_DATA_R |= 0x0C;
    delay(10000);
    GPIO_PORTD_DATA_R &= ~(0x08);
}

void delay(long val){
    while(val--);
}
