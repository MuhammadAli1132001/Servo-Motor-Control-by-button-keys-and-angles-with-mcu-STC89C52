#include <reg51.h>               // Include the 8051 microcontroller specific header file
#include "lcd.h"                 // Include the LCD library
#include <stdio.h>    
#include <intrins.h>
#include <stdlib.h>

#define  buzzer P1        // Define a bit for the buzzer

sbit Servo_motor_Pin = P2^0; // Define a bit for the servo motor control pin
sbit column1 = P1^0; // Define columns of the 4x4 keypad
sbit column2 = P1^1;
sbit column3 = P1^2;
sbit column4 = P1^3;

sbit row1 = P1^4; // Define rows of the 4x4 keypad
sbit row2 = P1^5;
sbit row3 = P1^6;
sbit row4 = P1^7;


char keypad[4][4] = {
    {'D', '#', '0', '*'},
    {'C', '9', '8', '7'},
    {'B', '6', '5', '4'},
    {'A','3','2', '1'}
};

unsigned int  ON_pulse, OFF_pulse; // Variables to store ON and OFF pulse durations for PWM
float DutyCycle;                 // Variable to store the duty cycle for PWM

unsigned char degree_symb[8] = {0x06, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00}; // Custom character for degree symbol

volatile unsigned int target_position = 0; // Variable to store the target servo position


void Timer0_Init() {
	
    TMOD = 0x01; // Timer0 in 16-bit mode
    TH0 = 0xB7;  
    TL0 = 0xFD;  
    TR0 = 1;     // Start Timer0
		IE= 0x82;     // Enable Timer0 interrupt
}

// Timer0 Interrupt Service Routine (ISR) for servo control
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

void Set_Duty_Cycle (float duty_cycle)  // Function to calculate ON and OFF pulse durations based on duty cycle
{  
    unsigned int Period;
    Period = 65535 - 0xB7FD;     										 // Total time period for PWM
    
    ON_pulse = ((Period / 100.0) * duty_cycle);   // Calculate ON pulse duration
    OFF_pulse = Period - ON_pulse;               // Calculate OFF pulse duration
    
    ON_pulse = 65535 - ON_pulse;   								// Invert ON pulse duration
    OFF_pulse = 65535 - OFF_pulse;							 // Invert OFF pulse duration
}



// Function to make a short beep sound on the buzzer
void BeepShort() {
    buzzer = 1; // Turn on the buzzer
    delay_ms(10); // Beep duration
    buzzer = 0; // Turn off the buzzer
}

// Function to make a long beep sound on the buzzer
void BeepLong() {
    buzzer = 1; // Turn on the buzzer
    delay_ms(100); // Beep duration
    buzzer = 0; // Turn off the buzzer
}


void lcd_disp_numb(unsigned int number,unsigned int width) {
	
    char buffer[8];                                                             // Create a character buffer to hold the formatted number
    sprintf(buffer, "%*d", width, number);
		lcd_clear();
    lcd_set_cursor(1, 0);                                              // Set cursor to the second row and first column
    lcd_write_string("Angle:");                               // Display input prompt
    lcd_set_cursor(1,6);  	                                          // Format the number with specified width
  	lcd_write_string(buffer);                                             // Display the formatted number on the LCD
}

void lcd_Custom_Char(unsigned char loc, unsigned char *msg) {
    unsigned char i;
	
    if (loc < 8) {
        lcd_write_command(0x40 + (loc * 8));  // Set the CGRAM address for the custom character
        for (i = 0; i < 8; i++) {
            lcd_write_data(msg[i]);  // Write the custom character pattern to CGRAM
        }
        lcd_write_command(0x80);  // Return to the LCD data address
    }
}



char GetKeyPressed() {
    char key = 0;
    row1 = 0; // Enable the first row
    row2 = 1;
    row3 = 1;
    row4 = 1;
    if (column1 == 0) key = keypad[0][0];
    if (column2 == 0) key = keypad[0][1];
    if (column3 == 0) key = keypad[0][2];
    if (column4 == 0) key = keypad[0][3];
    row1 = 1; // Disable the first row
    
    row1 = 1;
    row2 = 0; // Enable the second row
    row3 = 1;
    row4 = 1;
    if (column1 == 0) key = keypad[1][0];
    if (column2 == 0) key = keypad[1][1];
    if (column3 == 0) key = keypad[1][2];
    if (column4 == 0) key = keypad[1][3];
    row2 = 1; // Disable the second row
    
    row1 = 1;
    row2 = 1;
    row3 = 0; // Enable the third row
    row4 = 1;
    if (column1 == 0) key = keypad[2][0];
    if (column2 == 0) key = keypad[2][1];
    if (column3 == 0) key = keypad[2][2];
    if (column4 == 0) key = keypad[2][3];
    row3 = 1; // Disable the third row
    
    row1 = 1;
    row2 = 1;
    row3 = 1;
    row4 = 0; // Enable the fourth row
    if (column1 == 0) key = keypad[3][0];
    if (column2 == 0) key = keypad[3][1];
    if (column3 == 0) key = keypad[3][2];
    if (column4 == 0) key = keypad[3][3];
    row4 = 1; // Disable the fourth row
		

    return key;
}


unsigned int ReadAngleFromKeypad() {
    char angle_str[4];
    char key;
    unsigned int angle = 0;
    unsigned int length = 0;
    
    while (1) {
        key = GetKeyPressed();
        
        if (key != 0 && key != '#' && length < 3) {
            angle_str[length++] = key;
					            
				   	lcd_blink_cursor();
					  lcd_set_cursor(0, 12 + length);        					 // Set cursor to the first row and first column
            lcd_write_data(key);                   // Display the pressed key on an LCD (if available)
            delay_ms(100);                        // Debounce delay	
        }
				else if (key == '#') {
            angle_str[length] = '\0';         // Null-terminate the string
            angle = atoi(angle_str);         // Convert the string to an integer
            break;
        }
    }
    lcd_clear();
    return angle;
}



void main (void)
{    
	unsigned int entered_angle;
	
	lcd_init();
	Timer0_Init(); // Initialize Timer0 for servo control

		
   


    while (1) {
			
    lcd_clear();
    lcd_set_cursor(0, 0);        					 // Set cursor to the first row and first column
	  lcd_write_string("enter Angle:");		 // Display a message on the first row

    entered_angle = ReadAngleFromKeypad();

        if (entered_angle >= 0 && entered_angle <= 180) {
					
				                                       
            lcd_disp_numb(entered_angle, 3);                                 // Display the entered angle
            lcd_set_cursor(1,6);  	                                          // Format the number with specified width
					  lcd_Custom_Char(1,degree_symb);
					  lcd_set_cursor(2,9);
						lcd_write_data(1);
     //       BeepShort();                               // Short beep as feedback
        } 
				
				else {
					
            
						lcd_clear();                             // Entered angle is out of range, display an error message
            lcd_set_cursor(1, 1);                       // Set cursor to the second row and first column
            lcd_write_string("Error:Out-Range");
         //   BeepLong();                                // Long beep to indicate an error
        }
				
				DutyCycle = ((entered_angle) / 180.0) * 10; // Calculate the duty cycle for PWM
        Set_Duty_Cycle(DutyCycle);												 // Set the PWM duty cycle
        delay_ms(500); 										// Delay for stability
    }
}
