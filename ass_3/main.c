#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"
#include "lcd.h"
#include "GPIO.h"

#define TIM_1_SEC 200

extern int ticks;

void timeIncrementer(char *TimeBuffer);

int main(void) {
    init_systick();
    GPIO_init();
    lcd_init();
    delay(10000000);
    char Time[] = "00:00:00";
    ticks = 0;

    int aliveTimer = TIM_1_SEC;


    while(1){
        while( !ticks );
              // decrements ticks
              ticks--;

              // alive timer times out go into if statement
              if(! --aliveTimer){
                  // reset alive timer
                  aliveTimer = TIM_1_SEC;
                  // toggles leds depending on state of traffic light
                  timeIncrementer(Time);
              }
        displayTime(Time);
    }
}

void timeIncrementer(char *str){
    if(str[7]++ == '9'){
        str[7] = '0';
        if(str[6]++ == '6'){
            str[6] = '0';
            if(str[4]++ == '9'){
                str[4] = '0';
                if(str[3]++ == '6'){
                    str[3] = '0';
                    if(((str[1]++ == '9') && (str[0] != '2')) || ((str[1] == '4') && (str[0] == '2'))){
                        str[1] = '0';
                        if(str[0]++ == '2'){
                            str[0] = '0';
                        }
                    }
                }
            }
        }
    }
    if(str[2] == ":"){
        str[2] = " ";
        str[5] = " ";
    } else {
        str[2] = ":";
        str[5] = ":";
    }
}
