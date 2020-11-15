#include "picuno.h"
#define     PB1     PIN_RA0
#define     PB2     PIN_RA1
#define     M1      PIN_RA2
#define     M2      PIN_RA3
#define     M3      PIN_RB1
#define     M4      PIN_RB2
#define     LED     PIN_RD2
#define     BUZ     PIN_RD0
void beep(void);
void settingMode(void);
uint8_t buf[32];
byte i=0;
byte c;
uint32_t x=0;

void setup(void)
{
    Serial_begin(9600);
    Serial2_begin(9600);
    pinMode(PB1,INPUT);
    pinMode(PB2,INPUT);
    pinMode(M1,OUTPUT);
    pinMode(M2,OUTPUT);
    pinMode(M3,OUTPUT);
    pinMode(M4,OUTPUT);  
    pinMode(LED,OUTPUT);
    pinMode(BUZ,OUTPUT);
    pwm_on();
    for(i=1;i<5;i++)pwm_out(i,0);
    Serial2_println("Setup Done");
    Serial2_println("Menu:");
	Serial2_println("T-SETTING");
	Serial2_println("In Setting Mode, Speed must be END with #");
	Serial2_println("Speed MIN=0, MAX=255");
	Serial2_println("F-FORWARD");
	Serial2_println("B-BACKWARD");
	Serial2_println("R-RIGHT");
	Serial2_println("L-LEFT");
}
unsigned char speed[4]={0};

void loop(void)
{
    if(Serial2_available()){
        beep();
        c = Serial2_read();
    }
    if(c == 'T'){
        settingMode();
    }
    if(c == 'F'){
        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);        
        digitalWrite(M3,LOW);        
        digitalWrite(M4,LOW);        
        for(i=1;i<5;i++)pwm_out(i,speed[i-1]);        
    }
    else if (c == 'B'){
        digitalWrite(M1,HIGH);
        digitalWrite(M2,HIGH);        
        digitalWrite(M3,HIGH);        
        digitalWrite(M4,HIGH);        
         for(i=1;i<5;i++)pwm_out(i,speed[i-1]);                   
    }
    else if(c == 'L'){
        digitalWrite(M1,HIGH);
        digitalWrite(M2,LOW);        
        digitalWrite(M3,HIGH);        
        digitalWrite(M4,LOW);        
         for(i=1;i<5;i++)pwm_out(i,speed[i-1]);            
    }
    else if(c == 'R'){ //right
        digitalWrite(M1,LOW);
        digitalWrite(M2,HIGH);        
        digitalWrite(M3,LOW);        
        digitalWrite(M4,HIGH);        
         for(i=1;i<5;i++)pwm_out(i,speed[i-1]);          
    }
    else if (c == 'S'){
        for(i=1;i<5;i++)pwm_out(i,0);        
    }
    c=0;
}

void beep(void)
{
    digitalWrite(BUZ,HIGH);delay(150);
    digitalWrite(BUZ,LOW);
}
void settingMode(void){
    char str[32];
    
    Serial2_println("SETTING MODE ACTIVE\r\n\r\n");
    i=0;
    memset(buf,0,sizeof(buf));
    Serial2_print("SPEED M1:");
    while(1){
        if(Serial2_available()){
            c=Serial2_read();
            if(c == '#') break;
            buf[i++]=c;                          
        }
    }
    speed[0]=StrToUInt(buf);
    sprintf(str,"%d",speed[0]);
    Serial2_println(str);
    
    i=0;
    memset(buf,0,sizeof(buf));
    Serial2_print("SPEED M2:");
    while(1){
        if(Serial2_available()){
            c=Serial2_read();
            if(c == '#') break;
            buf[i++]=c;                          
        }
    }
    speed[1]=StrToUInt(buf);   
    sprintf(str,"%d",speed[1]);
    Serial2_println(str);

    i=0;
    memset(buf,0,sizeof(buf));
    Serial2_print("SPEED M3:");
    while(1){
        if(Serial2_available()){
            c=Serial2_read();
            if(c == '#') break;
            buf[i++]=c;                          
        }
    }
    speed[2]=StrToUInt(buf);    
    sprintf(str,"%d",speed[2]);
    Serial2_println(str);   
    
    i=0;
    memset(buf,0,sizeof(buf));
    Serial2_print("SPEED M4:");
    while(1){
        if(Serial2_available()){
            c=Serial2_read();
            if(c == '#') break;
            buf[i++]=c;                          
        }
    }
    speed[3]=StrToUInt(buf);  
    sprintf(str,"%d",speed[3]);
    Serial2_println(str);  
    
    beep();    
}