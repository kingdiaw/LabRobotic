#include "picuno.h"
#include <math.h>
//#include	<stdlib.h>
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
void moveRobot(void);
void disable_motor();
void enable_motor();
void stop();
//long map(long x, long in_min, long in_max, long out_min, long out_max);

//Global Variable
byte data,i=0;
byte dataStr[32];
byte buf[32];

void setup(){
    Serial2_begin(9600);
    Serial_begin(9600);
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

    while(Serial2_available()){
        byte a = Serial2_read();
        if(a == ')'){
            moveRobot();
            i=0;
            memset(dataStr,0,strlen(dataStr));
            return;
        }
        dataStr[i++]=a;
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
void moveRobot(void){
    unsigned char cnt=0;
    unsigned char analog;
    unsigned int A,S;   //A is angle, S is strength
    analog=0;
    for(cnt=0;cnt<strlen(dataStr);cnt++){
        if(dataStr[cnt]==','){
            analog=1;
            cnt=cnt+1;
            break;
        }
    }
    if(analog == 1){
        float theta,Vx,Vy;
        
        
        A = StrToUInt(dataStr);
        S = StrToUInt(dataStr+cnt);
        
        theta = radians(A);
        Vx = S * cos(theta);
        Vy = S * sin(theta);
        const float sqrt3o2 = 1.0*sqrt(3)/2;
        
            //STEP 3. Find wheel vecotrs
        float w1 = -Vx;                   // v dot [-1, 0] / 25mm
        float w2 = 0.5*Vx - sqrt3o2 * Vy; // v dot [1/2, -sqrt(3)/2] / 25mm
        float w3 = 0.5*Vx + sqrt3o2 * Vy; // v dot [1/2, +sqrt(3)/2] / 25mm
        
        bool w1_ccw = w1 < 0 ? true : false;
        bool w2_ccw = w2 < 0 ? true : false;
        bool w3_ccw = w3 < 0 ? true : false;     
        
        byte w1_speed = (byte) map(abs(w1), 0, 100, 0, 255);
        byte w2_speed = (byte) map(abs(w2), 0, 100, 0, 255);
        byte w3_speed = (byte) map(abs(w3), 0, 100, 0, 255);  
        
        enable_motor();
        
        digitalWrite(m1_dir,w1_ccw ? LOW : HIGH);
        digitalWrite(m2_dir,w2_ccw ? LOW : HIGH);
        digitalWrite(m3_dir,w3_ccw ? LOW : HIGH);
        
        pwm_out(PWM1,w1_speed);
        pwm_out(PWM2,w2_speed);
        pwm_out(PWM3,w3_speed);
        
        sprintf(buf,"w0:%d,w1=%d,w2=%d",w1_speed,w2_speed,w3_speed);
        Serial_println(buf);
    }
}
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
//long map(long x, long in_min, long in_max, long out_min, long out_max) {
//  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//}

