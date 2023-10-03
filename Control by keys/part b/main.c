#include <reg51.h>               // Include the 8051 microcontroller specific header file
#include "lcd.h"                 // Include the LCD library
#include <stdio.h>             // Include the intrinsic functions for controlling interrupts

sbit Servo_motor_Pin = P2^0;     // Define a bit for the servo motor control pin
sbit key_1 = P3^1;               // Define a bit for the first external interrupt (Increment)
sbit key_2 = P3^0;               // Define a bit for the second external interrupt (Decrement)

unsigned int servo_position = 0; // Variable to store the current servo motor position
unsigned int  ON_pulse, OFF_pulse; // Variables to store ON and OFF pulse durations for PWM
float DutyCycle;                 // Variable to store the duty cycle for PWM

unsigned char degree_symb[8] = {0x06, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00}; // Custom character for degree symbol

void Timer0_Intilization ()     // Function to initialize Timer0 for PWM generation
{
    TMOD = 0x01;                // Timer0 in 16-bit mode 1 
    TH0 = 0xB7;                 // Initial value for Timer0 High byte
    TL0 = 0xFD;                 // Initial value for Timer0 Low byte
    TR0 = 1;                    // Start Timer0
    IE = 0x82;                  // Enable Timer0 interrupt
}

void ISR_servo_moter() interrupt 1  // Timer0 Interrupt Service Routine (ISR) for PWM generation
{ 
    Servo_motor_Pin = ~Servo_motor_Pin ; // Toggle the servo motor control pin
    
    if (Servo_motor_Pin)
    {
        TH0 = ON_pulse >> 8;    // Set high byte for ON pulse duration
        TL0 = ON_pulse;         // Set low byte for ON pulse duration
    }
    else
    {
        TH0 = OFF_pulse >> 8;   // Set high byte for OFF pulse duration
        TL0 = OFF_pulse;        // Set low byte for OFF pulse duration
    }
}


void lcd_disp_numb(unsigned int number,unsigned int width) {
	
    char buffer[8];                                                             // Create a character buffer to hold the formatted number
    sprintf(buffer, "%*d", width, number);                                     // Format the number with specified width
  	lcd_write_string(buffer);                                                 // Display the formatted number on the LCD
}

void lcd_Custom_Char(unsigned char loc, unsigned char *msg) {
    unsigned char i;
	
    if (loc < 8) {
        lcd_write_command(0x40 + (loc * 8));                          // Set the CGRAM address for the custom character
        for (i = 0; i < 8; i++) {                                
            lcd_write_data(msg[i]);                                     // Write the custom character pattern to CGRAM
        }                   
        lcd_write_command(0x80);                                      // Return to the LCD data address
    }
}

void Set_Duty_Cycle (float duty_cycle)             // Function to calculate ON and OFF pulse durations based on duty cycle
{  
    unsigned int Period;
    Period = 65535 - 0xB7FD;     									            	 // Total time period for PWM
                         
    ON_pulse = ((Period / 100.0) * duty_cycle);                  // Calculate ON pulse duration
    OFF_pulse = Period - ON_pulse;                             // Calculate OFF pulse duration
    
    ON_pulse = 65535 - ON_pulse;   							              	// Invert ON pulse duration
    OFF_pulse = 65535 - OFF_pulse;							               // Invert OFF pulse duration
}

void main (void)
{
    lcd_init();               	// Initialize the LCD
    Timer0_Intilization();    				// Initialize Timer0 for PWM generation
			
    lcd_set_cursor(0, 1);        					 // Set cursor to the first row and first column
    lcd_write_string("Moter Angle");		 // Display a message on the first row
    Servo_motor_Pin = 0;
    while (1)
    {
        if (key_1 == 0)     			  
        { 			
            delay_ms(200);  						 // Delay to debounce the button
            
            if (servo_position < 180)
            {	
                servo_position += 45; 							// Increment the servo position
                lcd_set_cursor(2, 6);    							// Set cursor to the second row and 6th column
                lcd_disp_numb(servo_position, 3); // Display the servo position
                lcd_Custom_Char(1, degree_symb);   // Display the custom degree symbol
                lcd_set_cursor(2, 9);    							// Set cursor to the second row and 9th column
                lcd_write_data(1);   								// Write the custom degree symbol on the LCD
                delay_ms(500);       						// Delay for stability
            }     
            
            while (key_1 == 0);   					// Wait until the button is released
        }
        
        if (key_2 == 0)        					 
        {     
            delay_ms(200);     					// Delay to debounce the button
            if (servo_position > 0)
            {
                servo_position -= 45;			 // Decrement the servo position
                lcd_set_cursor(2, 6);   							 // Set cursor to the second row and 6th column
                lcd_disp_numb(servo_position, 3); 	// Display the servo position
                lcd_Custom_Char(1, degree_symb);  	 // Display the custom degree symbol
                lcd_set_cursor(2, 9);    									// Set cursor to the second row and 9th column
                lcd_write_data(1);  					 // Write the custom degree symbol on the LCD
                delay_ms(500);      						 // Delay for stability
            }		
            while (key_2 == 0);   							// Wait until the button is released
        }
        
        DutyCycle = ((servo_position + 45) / 180.0) * 10;                // Calculate the duty cycle for PWM
        Set_Duty_Cycle(DutyCycle);												               // Set the PWM duty cycle
        delay_ms(500); 									                           	// Delay for stability
    }
}

