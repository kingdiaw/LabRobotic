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
#define trig    PIN_RA4
#define echo    PIN_RA5

byte buf[32];

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
    pinMode(trig,OUTPUT);
    pinMode(echo,INPUT);
    TRISD &= 0b11110011;
    TRISC &= 0b10001111;
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
//==============================

void loop() // run repeatedly
{
    unsigned int distance_cm;
    
    //Prog. to read HC-SR04 every 500ms
    if(timerUp(0))
    {
        timerSet(0,500);
        distance_cm = read_distance();      
        sprintf(buf,"Distance:%d CM",distance_cm);
        Serial2_println(buf);
    }
    
    if(distance_cm > 10){
        //Do somethng when distance >10cm
        
    }
    else if(distance_cm <10){
        //Do something else when distance <10cm
        
    }
}

unsigned int read_distance(void){
    long duration;
    unsigned int distance;    
   //Clear the trig pin
    digitalWrite(trig,LOW);
    __delay_us(2);

    //Set Trig Pin to HIGH State for 10 micro sec
    digitalWrite(trig,HIGH);
    __delay_us(10);
    digitalWrite(trig,LOW);

    //Read the echo Pin,return the sound wave travel in microSecond
    duration = pulseIn(echo,HIGH);

    //Calculate the distance
    distance = (float)duration*0.034/2;  
    
    return distance;
}
void forward(void) {
    //Right (M1)
    IN1 = 1;
    IN2 = 0;
    pwm_out(PWM1, 255);	//Speed Right
    //Left (M2)
    IN3 = 1;
    IN4 = 0;
    pwm_out(PWM2, 255); //Speed Left
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
    pwm_out(PWM1, 255);	//Speed Right
    IN3 = 0;
    IN4 = 1;
    pwm_out(PWM2, 255); //Speed Left
}
