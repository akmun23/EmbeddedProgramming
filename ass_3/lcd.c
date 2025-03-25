#include "lcd.h"
/*
 * lcd.c
 *
 *  Created on: 3. mar. 2025
 *  Author: aksel
 */

/*
 *   0x30,      // Reset
  0x30,     // Reset
  0x30,     // Reset
  0x20,     // Set 4bit interface
  0x28,     // 2 lines Display
  0x0C,     // Display ON, Cursor OFF, Blink OFF
  0x06,     // Cursor Increment
  0x01,     // Clear Display
  0x02,     // Home
  0xFF      // stop
 *
 */

void lcd_init(void){

    lcd_cmd(setMode);                // 4bit mode utilising 16 columns and 2 rows
    delay(100);                   // Delay
    lcd_cmd(autoIncrement);          // autoincrementing the cursor when prints the data
    delay(100);                   // Delay
    lcd_cmd(cursorAndDisplay);       // Cursor on and display on
    delay(100);                   // Delay
    clearDisplay();                  // Clear screen

}

void Printdata(unsigned char data){
    //Fourth bit = D4 = PC4
    if(data & D4){
        GPIO_PORTC_DATA_R |= D4;
    } else {
        GPIO_PORTC_DATA_R &= ~D4;
    }

    //Fifth bit = D5 = PC5
    if(data & D5){
        GPIO_PORTC_DATA_R |= D5;
    } else {
        GPIO_PORTC_DATA_R &= ~D5;
    }

    //Sixth bit = D6 = PC6
    if(data & D6){
        GPIO_PORTC_DATA_R |= D6;
    } else {
        GPIO_PORTC_DATA_R &= ~D6;
    }

    //Seventh bit = D7 = PC7
    if(data & D7){
        GPIO_PORTC_DATA_R |= D7;
    } else {
        GPIO_PORTC_DATA_R &= ~D7;
    }
}

void lcd_cmd(unsigned char cmd){
    Printdata(cmd);                 // Outputs 4 MSB on pins
    GPIO_PORTD_DATA_R &= ~RS;   // Sets instruction register to write as internal operation
    GPIO_PORTD_DATA_R |= EN;      // Starts data R/W
    delay(100);                   // Delay
    GPIO_PORTD_DATA_R &= ~EN;   // Stop R/W

    Printdata(cmd << 4);            // Outputs 4 LSB on pins
    GPIO_PORTD_DATA_R &= ~RS;   // Sets instruction register to write as internal operation
    GPIO_PORTD_DATA_R |= EN;      // Starts data R/W
    delay(100);                   // Delay
    GPIO_PORTD_DATA_R &= ~EN;   // Stop R/W
}

void lcd_string(char *str, unsigned char len){
    char i;                         // Char variable
    for(i = 0; i < len; i++){       // Loops through the string
        lcd_data(str[i]);           // Calls lcd_data for each char in the string
    }
}

void lcd_data(unsigned char data){
    Printdata(data);                // Outputs 4 MSB of the char
    GPIO_PORTD_DATA_R |= (EN|RS);   // Enables Write and writes data
    delay(100);                   // Delay
    GPIO_PORTD_DATA_R &= ~EN;       // Turns off enable

    Printdata(data << 4);           // Outputs 4 LSB of the char
    GPIO_PORTD_DATA_R |= (EN|RS);   // Enables Write and writes data
    delay(100);                   // Delay
    GPIO_PORTD_DATA_R &= ~EN;       // Turns off enable
}

void delay(long val){
    while(val--);
}

void clearDisplay(void){
    lcd_cmd(clear);
}

void setCursor(int row, int column){
    if(row == 1){
        lcd_cmd(row1 + column);
    } else {
        lcd_cmd(row2 + column);
    }
}

void displayTime(char *Time){
    setCursor(1,4);
    lcd_string(Time, 8);
}
