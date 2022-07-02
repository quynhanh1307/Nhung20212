#include<at89x52.h>	
#include<intrins.h>
#define BUZZER P1_5
bit buzz = 0;

unsigned char code notes[] = {
	0xF9, 0x1F,
	0xF9, 0xDF,
	0xFA, 0x8A,
	0xFA, 0xD8,
	0xFB, 0x68,
	0xFB, 0xE9,
	0xFC, 0x23,
	0xFC, 0x5B,
	0xFC, 0x8F};

unsigned char TH0_val = 0, TL0_val = 0, duration = 20, elapsed = 0;
unsigned char curNote = 0;
unsigned char uart_data;

void InitTimers()
{
	//Timer 0 for note frequency
	//Timer 1 for note duration (interval of 50 ms)
	TMOD = 0x11; 
	TH0 = TH0_val;	 
	TL0 = TL0_val; 
	TH1 = 0x4C;	 //50 ms
	TL1 = 0x00; 
	EA = 1;			 
	ET0 = 1;	
	ET1 = 1;
	TR0 = 1;
	TR1 = 1;
	//Timer 2 for baudrate generator
	RCLK = 1;
	TCLK = 1;
	SCON = 0x50; 		// uart in mode 1 (8 bit), REN=1 */
	T2CON = 0x34; 		// Use Timer 2 for uart */
	RCAP2H = 0xFF; 		// 9600 Bds at 11.0592MHz */
	RCAP2L = 0xDC; 		// 9600 Bds at 11.0592MHz */
	ES = 1; 			// Enable serial interrupt*/
	TR2 = 1;
}

void main(void)
{
	P0 = 0;					//turn off the 7-seg LEDs
	TH0_val = notes[2*curNote];
	TL0_val = notes[2*curNote+1];
	InitTimers();
	while(1)
	{

	}				
}

void Timer0() interrupt 1
{
	TH0 = TH0_val;	 //reload for playing music
	TL0 = TL0_val;
	buzz = ~buzz;
	BUZZER = buzz;
}

void Timer1() interrupt 3
{
	TH1 = 0x4C;	 //reload for ~50 ms interval
	TL1 = 0x00;
	elapsed++;
	if (elapsed == duration)
	{
		elapsed = 0;
		
		curNote++;
		if (curNote > 2) curNote = 0;
		
		TH0_val = notes[2*curNote];
		TL0_val = notes[2*curNote+1];
	}
}

void serial_IT(void) interrupt 4
{
	if (RI == 1)
	{ 
		RI = 0; 			/* prepare for next reception */
		uart_data = SBUF; 	/* Read receive data */
		SBUF = uart_data; 	/* Send back same data on uart*/
	}
	else 
		TI = 0; 			/* if emission occur */
}