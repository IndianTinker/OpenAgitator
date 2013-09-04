/*
Open Agitator v2
author: Rohit Gupta
email:  rohit7gupta at gmail dot com

homepage: www.indiantinker.wordpress.com

compiler: Code Composer Studio


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include<msp430g2231.h>
#define A1 BIT0
#define A2 BIT1
#define SWC BIT6
#define EN BIT6
#define LED3 BIT7
#define LED2 BIT4
#define LED1 BIT5
#define ES BIT2
void setmode(int val)
{
	if ( (val>=3000) && (val<5000) )
	{   P1OUT&=~(LED1+LED2+LED3);
		P1OUT|=LED1;
	}
	else if ((val>=5000) && (val<=7500))
	{
		P1OUT&=~(LED1+LED2+LED3);
		P1OUT|=(LED1+LED2);
	}
	else if(val>7500)
	{
		P1OUT|=(LED1+LED2+LED3);

	}
}
void move()
{

		P1OUT&=~(A1+A2); //STOP
		__delay_cycles(1000);

		P1OUT|=A1;   //down
		P1OUT&=~A2;
		__delay_cycles(340000); // This controls the horizontal travel of the CDROM Cradle
		P1OUT&=~(A1+A2);
		__delay_cycles(1000);

		P1OUT&=~(A1+A2); //STOP
		__delay_cycles(1000);

		P1OUT|=A2;   //up
		P1OUT&=~A1;


}
void main()
{
	WDTCTL = WDTPW + WDTHOLD;      //Stop our Personal Watchdog

	BCSCTL1 = CALBC1_1MHZ;         //Run at 1Mhz
	DCOCTL = CALDCO_1MHZ;

	P1OUT&=~(LED1+LED2+LED3);               //GPIO Config
	P1DIR|=(A1+A2+EN+LED1+LED2+LED3);
	P1SEL|=EN;  //PWM output
    P2SEL&=~SWC;                           // P2.6 is special u see


    P2DIR&=~SWC; //switch as input
	P2IE|=SWC;  //interrupt enable on switch
    P2IFG&=~SWC; //clear interrupt flag for interrupt to proceed
	P2REN|=SWC;
	P2OUT|=SWC;

	P1DIR&=~ES;
	P1IE|=ES;  //interrupt enable on switch
	P1IFG&=~ES; //clear interrupt flag for interrupt to proceed
	P1REN|=ES;
	P1OUT|=ES;
	
	CCR0 = 10000;             // PWM Period
	CCTL1 = OUTMOD_7;          // CCR1 reset/set
    CCR1 = 1000;                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode

    _bis_SR_register(GIE); //Global Interrupts enabled

    P1OUT|=A2;   //up until u generate an interrupt
    P1OUT&=~A1;

    while(1)
	{
                //Do nothing
	}
}

#pragma vector=PORT2_VECTOR  //ISR Sub-routine
__interrupt void Port_2(void)
{
	if(P2IFG&SWC)              // When that button is pressed
	{
	P1OUT&=~(A1+A2);           //Stop cradle

	CCR1+=500;                 //Increase Duty
	if(CCR1==10000)            //Check: It doesnt overflow
		{
		CCR1=9500;
		}
	__delay_cycles(100000);    //Wait a bit. Mechanical time constants are larger
	setmode(CCR1);             // Light up the LED
	P1OUT|=A2;   //up           // Go up again and generate an interrupt
	P1OUT&=~A1;

	}
	P2IFG&=~SWC;	 //clear flag

}
#pragma vector=PORT1_VECTOR  //ISR Sub-routine
__interrupt void Port_1(void)
{
	if(P1IFG&ES)
	{
	//When end stop comes then move forward for 1.5sec
	//and then move backward until the next end stop.

		move();

	}
	P1IFG&=~ES;

}

