//#include <reg51.h> // Assuming you're using 8051 microcontroller
//#include <intrins.h>

//sbit OUT_PIN = P2^0; // Replace this with your output pin

//void delay_ms(unsigned int times){
//	unsigned int i,j;
//	
//	for (i= 0; i<times; i++){
//		for (j=0; j<1275; j++);
//	}
//}
//	
//void delay_us(unsigned int us) {
//   unsigned int i;
//	
//	    for (i = 0; i < us; i++) {

//    TH0 = 0xFF;                       // Set the high byte of timer value
//    TL0 = 0xD2;                    // Set the low byte of timer value
//    TR0 = 1; // Start the timer

//        while (!TF0); // Wait for timer overflow
//        TF0 = 0; // Clear the overflow flag
//        TR0 = 0; // Stop the timer
//			}
//}

//void main() {
//	    TMOD = 0x01; // Timer 0, mode 1: 16-bit timer

//	      
//    
//	while (1) {	
//	
//		int n;
//		OUT_PIN = 0;
//		
//		 for (n=20; n<26; n=n+2){
//			 
//        OUT_PIN = 1; // Set the output pin high
//        delay_us(n); // Delay for 500 microseconds
//        OUT_PIN = 0; // Set the output pin low
//        delay_us(260); // Delay for 19500 microseconds
////			 
//			 delay_ms(200);
//		 }
//    }
//}


#include <reg51.h>
#include <stdio.h>
#include <intrins.h>

sbit servo_pin = P2^0;
void delay(unsigned int);
void servo_delay(unsigned int);
void main()
{
  servo_pin = 0;
  do
  {
		
	 servo_pin = 1;
   servo_delay(500);
   servo_pin = 0;
	 servo_delay(19500);
		
	 delay(200);
		
	 servo_pin = 1;
   servo_delay(1000);
   servo_pin = 0;
   servo_delay(19000);
		
	 delay(200);   
 }while(1);
}

void delay(unsigned int ms)
{
  int i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<127;j++);
}

void servo_delay(unsigned int ms)
{
  unsigned int i, j;	
	unsigned int us = ms/1000;

  for (i = 0; i < us; i++){
    for (j = 0; j < 127; j++);
	}
}

