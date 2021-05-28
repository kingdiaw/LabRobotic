#include <pic18f45k22.h>

#include "Wire.h"
#include "picuno.h"

unsigned char error_flag;



//Middle Level Function
//==============================================================================
void Wire_begin(unsigned long c){
    //Slew rate control disabled for standard speed mode (100 kHz and 1 MHz)
    SSP1STATbits.SMP = 1;
    
    //Select I2C Master Mode, Clock = Fosc/(4*(SSP1ADD + 1))
    SSP1CON1bits.SSPM3 = 1;
    SSP1CON1bits.SSPM2 = 0;
    SSP1CON1bits.SSPM1 = 0;
    SSP1CON1bits.SSPM0 = 0;
    
    SSP1ADD = (_XTAL_FREQ /(4*c))-1;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;

    //Clear the Write Collision Detect bit
    SSP1CON1bits.WCOL = 0;
    
    //Clear the Receive Overflow Indicator bit
    SSP1CON1bits.SSPOV = 0;
        
    //Enable the MSSP module
    SSP1CON1bits.SSPEN = 1;

    //Send Stop condition, stop any previous communication
    Wire_stop();
}

void Wire_beginTransmission(uint8_t address){
    
	//Clear the error flag before we start a new I2C operation
    error_flag = 0;

	//Send START bit
    Wire_start();

	//Send Slave address and indicate to write
	SSP1BUF = (address << 1) & 0xFE;

	//wait for message sending process complete
	Wire_wait();    
    
    //If it got problem, assign error flag to 1
    Wire_status(1); 
}

void Wire_write(uint8_t data){
    
    //Send data
    SSP1BUF = data;

    //wait for message sending process complete
    Wire_wait();	

    //If it got problem, assign error flag to 2
    Wire_status(2);
}

void Wire_writeNBytes(const uint8_t* data, uint8_t data_len){
    
    for(unsigned char i=0;i < data_len ;i++){
        //Send data
        SSP1BUF = data[i];

        //wait for message sending process complete
        Wire_wait();	

        //If it got problem, assign error flag to 2
        Wire_status(2);
    }    
}

uint8_t Wire_endTransmission(){
	//Send STOP bit
    Wire_stop();
    return error_flag;
}

uint8_t Wire_readByte(uint8_t address){
	uint8_t rx_data;
	unsigned long count = 90000;
    error_flag = 0;  

	//Send RESTART bit
    Wire_restart();

	//Send slave address and indicate to read
	SSP1BUF = (address << 1) | 0x01;

	//wait for message sending process complete
	Wire_wait();	

    //If it got problem, assign error flag to 1
    if(Wire_status(1))return; 

	//Enable receive
	SSP1CON2bits.RCEN = 1;
	
	//wait until the data received
	while(SSP1STATbits.BF == 0)
	{
		//if timeout...
		if(--count == 0 )
		{
			//Send Stop bit
			Wire_stop();
			
			//3: Timeout
			error_flag = 3;
			return 0;
		}
	}

	//Read the received data
	rx_data = SSP1BUF;

	//Send Not Acknowledge
    Wire_NACK();
	return rx_data;	    
}

/*
 
1.The user generates a Start condition by setting
the SEN bit of the SSPxCON2 register.
 * SSP1CON2bits.SEN = 1;

2. SSPxIF is set by hardware on completion of the
Start.
 * while(PIR1bits.SSP1IF == 0);

3. SSPxIF is cleared by software.
 * PIR1bits.SSP1IF = 0;
4. User writes SSPxBUF with the slave address to
transmit and the R/W bit set.
 * SSP1BUF = address << 1 | 0x01;
 
5. Address is shifted out the SDAx pin until all 8 bits
are transmitted. Transmission begins as soon
as SSPxBUF is written to.
 
6. The MSSPx module shifts in the ACK bit from
the slave device and writes its value into the
ACKSTAT bit of the SSPxCON2 register.

 * if(SSP1CON2bits.ACKSTAT==1)stop 
 
7. The MSSPx module generates an interrupt at
the end of the ninth clock cycle by setting the
SSPxIF bit.
 * while(PIR1bits.SSP1IF == 0);
 
8. User sets the RCEN bit of the SSPxCON2 regis-
ter and the Master clocks in a byte from the slave.
 * SSP1CON2bits.RCEN=1;

9. After the 8th falling edge of SCLx, SSPxIF and
BF are set.
 * while(BF==0);
10. Master clears SSPxIF and reads the received
byte from SSPxUF, clears BF.
 * PIR1bits.SSP1IF =0;
 * data = SSP1BUF;
 * BF = 0;
11. Master sets ACK value sent to slave in ACKDT
bit of the SSPxCON2 register and initiates the
ACK by setting the ACKEN bit.
 * SSP1CON2bits.ACKDT = 0;
 * ACKEN=1;
12. Masters ACK is clocked out to the slave and
SSPxIF is set.
 * while(PIR1bits.SSP1IF==0);
13. User clears SSPxIF.
 * PIR1bits.SSP1IF = 0;
14. Steps 8-13 are repeated for each received byte
from the slave.
15. Master sends a not ACK or Stop to end
communication.
 * 
 */
void Wire_readNBytes(uint8_t address, uint8_t* rx_data, uint8_t data_len){

	unsigned long count;
    error_flag = 0;  
    
    //Send RESTART bit
    Wire_restart();

	//Send slave address and indicate to read
	SSP1BUF = (address << 1) | 0x01;

	//wait for message sending process complete
	Wire_wait();	

    //If it got problem, assign error flag to 1
    if(Wire_status(1))return; 


    
    for(unsigned char i=0;i < data_len; i++){
    count = 10000;
    
    //Enable receive
	SSP1CON2bits.RCEN = 1;
    
	//wait until the data received
	while(SSP1STATbits.BF == 0)
	{
		//if timeout...
		if(--count == 0 )
		{
			//Send Stop bit
			Wire_stop();
			
			//3: Timeout
			error_flag = 3;
			return;
		}
	} 
        rx_data[i] = SSP1BUF;
        SSP1STATbits.BF = 0;
        
        if(i < (data_len - 1))
            Wire_ACK();
        else
            Wire_NACK();
    }
    
}

//LOW Level Function
//==============================================================================
void Wire_wait(){
    while(SSP1CON2 & 0b00011111 | (SSP1STATbits.R_W));
}

uint8_t Wire_status(uint8_t status){   
    if(SSP1CON2bits.ACKSTAT == 1){
    Wire_stop();
    //2:received NACK on transmit of data
    error_flag = status;
    } 
    else{
        error_flag = 0;
    }
    return error_flag;
}

void Wire_NACK(void){
  //---[ Send NACK - For Master Receiver Mode ]---
	SSP1CON2bits.ACKDT = 1;
	SSP1CON2bits.ACKEN = 1;
	while(SSP1CON2bits.ACKEN == 1);
}

void Wire_ACK(void){
    SSP1CON2bits.ACKDT = 0; 
    SSP1CON2bits.ACKEN = 1;
	while(SSP1CON2bits.ACKEN == 1);    
}

void Wire_stop(){
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN == 1);
}

void Wire_restart(void){
    SSP1CON2bits.RSEN = 1;
	while(SSP1CON2bits.RSEN == 1);
}

void Wire_start(void){
    SSP1CON2bits.SEN = 1;
	while(SSP1CON2bits.SEN == 1);
}