#include "picuno.h"
#define PB1     INP_RB6
#define PB2     INP_RB7
#define IN1     OUT_RB4
#define IN2     OUT_RB2
#define IN3     OUT_RB0
#define IN4     OUT_RB1
#define LED5    OUT_RD2
#define LED4    OUT_RD3
#define LED3    OUT_RC4
#define LED2    OUT_RC5
#define LED1    OUT_RC6
#define PWM1    3
#define PWM2    2

//Global Variable
//=====================================
byte buf[32];

void setup(){
    Serial_begin(9600);
    pwm_on();
    pinMode(PIN_RB0, OUTPUT);
    pinMode(PIN_RB1, OUTPUT);
    pinMode(PIN_RB2, OUTPUT);
    pinMode(PIN_RB4, OUTPUT);
    pinMode(PIN_RB6, INPUT);
    pinMode(PIN_RB7, INPUT);
    TRISD &= 0b11110011;
    TRISC &= 0b10001111;
    TRISE |= 0b00000011;
    pwm_out(PWM1, 0);
    pwm_out(PWM2, 0);
    Serial_println("Setup Done,PRESS PB1 to continue");
    Serial_println(buf);
    while(PB1);
}

void loop(){
    unsigned int rv=analogRead(RV1);
    sprintf(buf,"analog reading:%d",rv);
    Serial_println(buf);
    delay(600);
    
}