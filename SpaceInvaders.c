// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURlocE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
char WinCheck(void);
unsigned long TimerCount;
unsigned long Semaphore;
char Location[4][4];
char Win=0;
char loc,stepX,stepY,player,game_Type,game_element,done;


// *************************** Images ***************************




const unsigned char logo [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xF0, 0xF8, 0xF8, 0xFC,
0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE,
0xFE, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0x7F, 0x7F, 0xFF, 0x7F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x33, 0xFF, 0xFF,
0x87, 0x01, 0x00, 0x38, 0x38, 0x01, 0x01, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC,
0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xEE, 0xC0, 0xC0, 0xF0, 0xF0, 0xE2, 0xE7, 0x7F, 0x7F, 0x7F, 0x3F, 0x00, 0x00,
0x00, 0x1F, 0x1F, 0x8F, 0x8F, 0x8E, 0xCE, 0xC6, 0xC6, 0xC7, 0xE7, 0xE7, 0xE3, 0xF3, 0xF7, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE3, 0xF3, 0xF1, 0x79, 0x18, 0x18, 0x9C, 0x9C, 0x1C, 0x3E,
0xFE, 0xFE, 0xF0, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xF7, 0xC7, 0x07, 0x0F, 0x03, 0x23, 0x7F,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x1F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xE0, 0xE3, 0xC7,
0xC7, 0xE0, 0xF0, 0xFC, 0xFF, 0xFF, 0xFE, 0xE0, 0x80, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC,
0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x1F, 0x1F,
0x3F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F,
0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 
};

const unsigned char SmallEnemy30PointA[] = {
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x0F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
 0xFF, 0x0F, 0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};



// *************************** Capture image dimensions out of BMP**********

	void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
void PORTE_INIT(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock PortE
  GPIO_PORTE_CR_R = 0x03;           // allow changes to PE2       
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R = 0x02;          // 5) PE0 input, PE1 output   
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTE_PDR_R= 0x01;          // enable pulldown resistors on PE0       
  GPIO_PORTE_DEN_R = 0x01;          // 7) enable digital pins PE0-PE1
}

void game_Init(char g_T){
	
	 char i,j;
	 loc=0; 
	game_Type=g_T;  // game type = 3*3
	game_element=(game_Type*game_Type-1);
	done=1;
	stepX=4,    // the distance between the element in the row
	stepY=2;    // the distance between the rows
	 
	 player=0; // 0--> playerX 1 --> playerY 
	 for(i=0;i<g_T;i++){
		 for(j=0;j<g_T;j++)
		 Location[i][j]=0;
	 }
}
void Draw(){
	char i,j,x,y;
	x=84/game_Type;
	y=48/game_Type;
	
	for(i=0;i<game_Type;i++){             // to make sure that all pixels are clear to start play
		for(j=0;j<48;j++)Nokia5110_SetPixel(x*i,j);
	  for(j=0;j<84;j++)Nokia5110_SetPixel(j,y*i);
	}
	
	 Nokia5110_SetCursor(0,5);
	 Nokia5110_OutString("Player X");
	
}
char start(){
	char Right, Left ,Play,choose=3;
	Nokia5110_SetCursor(0,0);
	Nokia5110_OutString("Press Play Button to Start");
	Nokia5110_SetCursor(1,2);
	Nokia5110_OutChar('_');
	
	while(1){
		 Right = GPIO_PORTF_DATA_R&0x10;    // read PF4 into SW1
     Left = GPIO_PORTF_DATA_R&0x01;     // read PF0 into SW2
	   Play = GPIO_PORTE_DATA_R&0x01;     // read PE0 into Play
	
	   if(Play) return choose;
	}
}

 void set_Cursor() {      // cursor that point to your current location       
	 char locX,locY;
	 if( Location[loc/game_Type][loc%game_Type]==0){
		        locX=((loc%game_Type)*stepX);
				    locY=((loc/game_Type)*stepY);
						Location[loc/game_Type][loc%game_Type]='_';
						Nokia5110_SetCursor(locX,locY);
			  	  Nokia5110_OutChar('_');
		        Nokia5110_SetCursor(locX,locY);
						}
	 }
 
	 void remove_Cursor(){    // when change location of cursor remove the previose location's cursor
		 char locX,locY;
		 if(Location[loc/game_Type][loc%game_Type]=='_'){
			    locX=((loc%game_Type)*stepX);
				  locY=((loc/game_Type)*stepY);
					Location[loc/game_Type][loc%game_Type]=0;
					Nokia5110_SetCursor(locX,locY);
			  	Nokia5110_OutChar(' ');
				}
	 }
	 
char WinCheck(void){        
	   char i,j,countrX=0,countrY=0,countcX=0,countcY=0,countdX=0,countdY=0,countodX=0,countodY=0,Full=1;
		 for(i=0;i<game_Type;i++){
			 countrX=0,countrY=0,countcX=0,countcY=0;
		    for(j=0;j<game_Type;j++){
					if(Location[i][j]=='X')countrX++;  // counter check x in rows
					if(Location[i][j]=='O')countrY++;	// counter check o in rows
          if(Location[j][i]=='X')countcX++;  // counter check x in colums
          if(Location[j][i]=='O')countcY++;	// counter check o in colums
          if(Location[i][j]==0) Full=0;			
		 }
				if(Location[i][i]=='X')countdX++;  // counter check x in diagonal
				if(Location[i][i]=='O')countdY++;  // counter check o in diagonal
		    if(Location[i][game_Type-1-i]=='X')countodX++; // counter check x in other diagonal
				if(Location[i][game_Type-1-i]=='O')countodY++; // counter check o in other diagonal
				if(countrX==game_Type||countcX==game_Type||countdX==game_Type||countodX==game_Type)return 'x';
		    if(countrY==game_Type||countcY==game_Type||countdY==game_Type||countodY==game_Type)return 'o';
	 }
		 if(Full) return 'q';
	   return 0;
}

int main(void){
	
	 unsigned long Right,Left,Play;
	 TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
	 Random_Init(1);
 	 Nokia5110_Init();
	 Nokia5110_ClearBuffer();
	 Nokia5110_DisplayBuffer();      // draw buffer
	
	 PortF_Init();
	 PORTE_INIT();
	    
	Nokia5110_PrintBMP(0,0,SmallEnemy30PointA,0);
	Nokia5110_DisplayBuffer();
	
	 //Delay100ms(2);
	 Nokia5110_Clear();
	
	 game_Init(start());  // the start of the game 
	 Nokia5110_Clear();
	 Delay100ms(1);
	 Draw();
	 set_Cursor();
	 Nokia5110_SetCursor(0,0);
	
	
 
while(1){
		 Right = GPIO_PORTF_DATA_R&0x10;    //Right ---> read PF4 into SW1
     Left = GPIO_PORTF_DATA_R&0x01;     //Left ---> read PF0 into SW2
		 Play = GPIO_PORTE_DATA_R&0x01;     //Play ---> read PE0
		
	
			
			if(!(Right)){
        remove_Cursor();				
				loc++;
				if(loc>game_element)loc=game_element;
    	   while(!(GPIO_PORTF_DATA_R&0x10));
				  set_Cursor();				  						  	
			}
				
			if(!(Left))
			{
				remove_Cursor();
				loc--;
				if(loc<0)loc=0;
				while(!(GPIO_PORTF_DATA_R&0x01));
				  set_Cursor();
			}
			
			if((Play)){
				while(GPIO_PORTE_DATA_R&0x01);
				if(!(player)){
					if(Location[loc/game_Type][loc%game_Type]=='_')
					{
				  	Nokia5110_OutChar('X');
				  	Location[loc/game_Type][loc%game_Type]='X';
						loc++;
					  if(loc>game_element)loc=game_element;
						Nokia5110_SetCursor(0,5);
						Nokia5110_OutString("Player O"); 
						set_Cursor();
						player^=1;
					}
				}
					
				else{
					if(Location[loc/game_Type][loc%game_Type]=='_')
						{
							Nokia5110_OutChar('O');
							Location[loc/game_Type][loc%game_Type]='O';
							loc++;
							if(loc>game_element)loc=game_element;
							Nokia5110_SetCursor(0,5);						
							Nokia5110_OutString("Player X");
							set_Cursor();
							player^=1;
					}
				}
				Win=WinCheck();        // calculate the Winner every time the play button pressed
			}
			
			if(Win){
				
				if(Win=='x') {
					if(done){
					Nokia5110_Clear();
					Nokia5110_SetCursor(0,2);
					Nokia5110_OutString("X Win");
						done=0;
					}
					
				}
				if(Win=='o') {
					if(done){
					Nokia5110_Clear();
					Nokia5110_SetCursor(0,2);
					Nokia5110_OutString("Y Win");
						done=0;
					}
				}
				
				if(Win=='q') {
					if(done){
					Nokia5110_Clear();
					Nokia5110_SetCursor(0,2);
					Nokia5110_OutString("Try Again");
						done=0;
					}
				}
			}
  }

}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
