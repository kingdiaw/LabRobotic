#include "picuno.h"
//IO Mapping
/*
 MOTOR DRIVER - PIC
 M1 Direction - RA1
 M1 PWM       - RC2 (CCP1)
 M2 Direction - RA2
 M2 PWM       - RB3 (CCP2)
 M3 Direction - RA5
 M3 PWM       - RB5 (CCP3)
 */

#define m1_dir      PIN_RA1
#define m2_dir      PIN_RA2
#define m3_dir      PIN_RD0
#define PB2         PIN_RC1
#define LED         PIN_RB6
#define BUZZER      PIN_RB7
#define SKPS_RST    PIN_RB2
#define PWM1        1
#define PWM2        2
#define PWM3        3

//Prototype Function
void disable_motor();
void enable_motor();
void stop();

//Global Variable
byte data;
byte buf[32];

void setup(){
    Serial2_begin(9600);
    pinMode(PB2,INPUT);
    pinMode(LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode (SKPS_RST, OUTPUT);
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER, LOW);
    digitalWrite(SKPS_RST, LOW);
    disable_motor();
    pwm_on();
    pwm_out(PWM1,0);
    pwm_out(PWM2,0);
    pwm_out(PWM3,0);
    Serial2_println("PRESS PB2 to START");
    while(digitalRead(PB2));
}

void loop(){

    if(Serial2_available()){
        data = Serial2_read();
        sprintf(buf,"Data Received: %c",data);
        Serial2_println(buf);
    }
    
    if(data == 'f'){
        data = 0;
        //Move to North
        enable_motor();
        digitalWrite(m1_dir,LOW);
        digitalWrite(m2_dir,HIGH);
        digitalWrite(m3_dir,HIGH);
        pwm_out(PWM1, 200);
        pwm_out(PWM2, 200);
        pwm_out(PWM3, 0);        
    }
    else if(data == 'b'){
        data = 0;
        //Move to South
        enable_motor();
        digitalWrite(m1_dir,HIGH);
        digitalWrite(m2_dir,LOW);
        digitalWrite(m3_dir,HIGH);
        pwm_out(PWM1, 200);
        pwm_out(PWM2, 200);
        pwm_out(PWM3, 0);        
    }    
    else if(data == 's'){
        data = 0;
        stop();
    }

    if(timerUp(9)){
        timerSet(9,250);
        digitalWrite(LED, digitalRead(LED)^1);
    }
}

//User Define Function Area----------------------------------------------------
void stop(){
    disable_motor();
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
}

void disable_motor(){
    pinMode(m1_dir,INPUT);
    pinMode(m2_dir,INPUT);
    pinMode(m3_dir,INPUT);    
}

void enable_motor(){
    pinMode(m1_dir,OUTPUT);
    pinMode(m2_dir,OUTPUT);
    pinMode(m3_dir,OUTPUT);    
}

