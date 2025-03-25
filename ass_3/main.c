#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"
#include "lcd.h"
#include "GPIO.h"
#include "uart0.h"


#define TIM_1_SEC 200

extern int ticks;

void timeIncrementer(char *str);

void checkComs(char * time);

int main(void) {
    init_systick();
    GPIO_init();
    lcd_init();
    uart0_init(19200, 8, 1, 0);
    delay(1000000);
    char Time[] = "00:00:00";
    ticks = 0;

    int aliveTimer = TIM_1_SEC;


    while(1){

        while(!uart0_rx_rdy()){
            if(ticks ){

              // decrements ticks
              ticks--;

              // alive timer times out go into if statement
              if(! --aliveTimer){
                  // reset alive timer
                  aliveTimer = TIM_1_SEC;
                  // toggles leds depending on state of traffic light
                  timeIncrementer(Time);
                  GPIO_PORTD_DATA_R ^= 0x40;
              }
              displayTime(Time);
            }
        }
        checkComs(Time);
    }
}

void timeIncrementer(char *str){
    if(str[7]++ == '9'){
        str[7] = '0';
        if(str[6]++ == '5'){
            str[6] = '0';
            if(str[4]++ == '9'){
                str[4] = '0';
                if(str[3]++ == '5'){
                    str[3] = '0';
                    if(((str[1]++ == '9') && (str[0] != '2')) || ((str[1] == '4') && (str[0] == '2'))){
                        str[1] = '0';
                        if(++str[0] == '3'){
                            str[0] = '0';
                        }
                    }
                }
            }
        }
    }
    if(str[2] == ':'){
        str[2] =  ' ';
        str[5] = ' ';
    } else {
        str[2] = ':';
        str[5] = ':';
    }
}

void checkComs(char * time){
      char c = uart0_getc();


      if(c == '1'){
          int i;
          for (i = 0; i < 8; i++){
              if((i != 2) && (i != 5)){
                  while(!uart0_rx_rdy());
                  time[i] = uart0_getc();
              }
          }

      }else if (c == '2'){
          int i;
          for(i = 0; i < 8; i++){
              while(!uart0_tx_rdy());
              uart0_putc(time[i]);
          }
          uart0_putc(0x0B);
          uart0_putc('c');
      }

      else if(c == 't'){
          GPIO_PORTD_DATA_R ^= 0x40;
      }else{
          while(!uart0_tx_rdy());
          uart0_putc(c);
      }
      while(uart0_rx_rdy()){
          c = uart0_getc();
      }
      ticks = 0;
}
