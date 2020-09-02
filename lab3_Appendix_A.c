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

//===== Change Following Parameter According to your ROBOT =====//
const unsigned char speedL = 190;
const unsigned char speedR = 190;

//=============================================================//

//====== Global Variable ======//
byte buf[32];
byte sensorArray, sensorArrayOld;

//=============================//

void setup() // run once
{
    Serial2_begin(9600);
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
    Serial2_println("Setup Done");
    Serial2_println("Press PB1(RB6) to START");
    while (PB1);
}

//Prototype Function
//==============================
void forward(void);
void stop(void);
void reverse(void);
unsigned int read_distance(void);
byte linefollower(byte target_number_junc);
byte turn_left(void);
byte turn_right(void);
//==============================

void loop() // run repeatedly
{
//    if(timerUp(0)){
//        timerSet(0,500);
//        sensorArray = 0;
//        sensorArray |=(INP_RE1<<4)|(INP_RE0<<3)|(INP_RA3<<2)|(INP_RA2<<1)|(INP_RA1); 
//        //sprintf(buf,"Sensor Array: %d\r\n",sensorArray);
//        //Serial2_println(buf);
//        //sprintf(buf,"Sensor:%d%d%d%d%d",INP_RE1,INP_RE0,INP_RA3,INP_RA2,INP_RA1);
//        //Serial2_println(buf);
//    }
    
    while(linefollower(3));
    forward();
    delay(600);
    stop();
    while(turn_left());
    while(linefollower(4));
    forward();
    delay(600);
    stop();
    while(turn_right());
    while(linefollower(2));
    forward();
    delay(600);
    stop();
    while(1);
    
}

void forward(void) {
    //Right (M1)
    IN1 = 1;
    IN2 = 0;
    pwm_out(PWM1, speedR);  //Speed Right
    //Left (M2)
    IN3 = 1;
    IN4 = 0;
    pwm_out(PWM2, speedL); //Speed Left
}

void stop(void) {
    pwm_out(PWM1, 0); //Speed Right
    IN1 = 1;
    IN2 = 1;
    pwm_out(PWM2, 0); //Speed Left
    IN3 = 1;
    IN4 = 1;
}

void reverse(void) {
    IN1 = 0;
    IN2 = 1;
    pwm_out(PWM1, speedR);
    IN3 = 0;
    IN4 = 1;
    pwm_out(PWM2, speedL); //Speed Left
}


byte linefollower(byte target_number_junc){  
    static unsigned char number_junc;
    sensorArray = 0;
    sensorArray |=(INP_RE1<<4)|(INP_RE0<<3)|(INP_RA3<<2)|(INP_RA2<<1)|(INP_RA1);
    
    //Right (M1)
    IN1 = 1;
    IN2 = 0;
    //Left (M2)
    IN3 = 1;
    IN4 = 0;
    
    switch(sensorArray){
        case 27:    //11011 - forward Straight 
            if(sensorArrayOld != sensorArray){                
                pwm_out(PWM1, speedR); //Speed Right
                pwm_out(PWM2, speedL); //Speed Left  
                sensorArrayOld = sensorArray;
            }
            break;
            
        case 29:    //11101 - forward with a little turn Right
            if(sensorArrayOld != sensorArray){ 
                pwm_out(PWM1, speedR-5);
                pwm_out(PWM2, speedL+5); //Speed Left  
                sensorArrayOld = sensorArray;                
            }          
            break;
            
        case 30:    //11110 - forward with more turn Right
            if(sensorArrayOld != sensorArray){ 
                pwm_out(PWM1, speedR-10);
                pwm_out(PWM2, speedL+10); //Speed Left              
                sensorArrayOld = sensorArray;                
            }
            break;
            
        case 23:    //10111 - forward with a little turn Left
            if(sensorArrayOld != sensorArray){ 
                pwm_out(PWM1, speedR+5);
                pwm_out(PWM2, speedL-5); //Speed Left 
                sensorArrayOld = sensorArray;                
            }               
            break;
            
        case 15:    //01111 - forward with more turn Left
            if(sensorArrayOld != sensorArray){
                pwm_out(PWM1, speedR+10);
                pwm_out(PWM2, speedL-10); //Speed Left                 
                sensorArrayOld = sensorArray;                
            }             
            break;
//        case 31:    //11111 - No line then STOP
//            if(sensorArrayOld != sensorArray){
//                stop();                  
//                sensorArrayOld = sensorArray;
//            }            
//            break;
        case 0:     //00000 - Junction
            if(sensorArrayOld != sensorArray){
                sensorArrayOld = sensorArray;
                if(++number_junc >= target_number_junc){
                    number_junc = 0;
                    stop();
                    return 0;
                }                 
                sprintf(buf,"Number of Junction:%d",number_junc);
                Serial2_println(buf);                                             
            }            
            break;
    }
    return 1;
}

byte turn_left(){
    //Pivot Left
    //Right (M1)
    IN1 = 1;
    IN2 = 0;
    //Left (M2)
    IN3 = 0;
    IN4 = 1;  
    pwm_out(PWM1,speedR);
    pwm_out(PWM2,speedL);
    if(!digitalRead(PIN_RE1)){  //wait until 0xxxx
        stop();
        return 0;    
    }
    return 1;
}

byte turn_right(){
    //Pivot Right
    //Right (M1)
    IN1 = 0;
    IN2 = 1;
    //Left (M2)
    IN3 = 1;
    IN4 = 0;  
    pwm_out(PWM1,speedR);
    pwm_out(PWM2,speedL); 
    if(!digitalRead(PIN_RA1)){  //wait until xxxx0
        stop();
        return 0;
    }    
    return 1;
}
