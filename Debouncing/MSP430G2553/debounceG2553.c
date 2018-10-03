/*Author: Nick Scamardi
 *Written: October 3, 2018
 *Last Updated: October 3, 2018
*/

#include <msp430.h>

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Disable the watchdog timer

	P1DIR |= BIT6;  // Set P1.6 to output direction (Red LED)

	P1DIR &= ~BIT3; // Set P1.3 to input direction (Button)
	P1REN |= BIT3;  // Enable internal resistor
	P1OUT |= BIT3;  // Sets resistor to pull-up

	P1IE = BIT3;    // Interrupt enabled for Button (P1.3)
	P1IFG &= ~BIT3; // Clear interrupt flag for button
	TA0CCTL0 = CCIE; // Capture Compare Interrupt Enabled

	__enable_interrupt();   // Enable interrupt algorithm

	while(1){}; // Infinite while loop
}

// Interrupt service routine for Port 1
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1OUT ^= BIT6;  // Toggle P1.6 (Red LED)
    P1IE &= ~BIT3;  // Turn off interrupt for button to prevent bouncing due to unintentional interrupts

    TACCR0 = 10000; // Set capture compare register value for timer count (~10ms)
    TA0CTL = TASSEL_2 + MC_1 + TACLR;   // Timer Control: SMCLK (~1 MHz), Up mode, Clear
}

// Interrupt service routine for TimerA0
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    TA0CTL = MC_0 + TACLR;  // Timer Control: Stop timer, Clear
    P1IE |= BIT3;   // Enable interrupt on P1.3 (Button) to detect next interrupt; Occurs after ~10ms in order to prevent bouncing
    P1IFG &= ~BIT3; // Clear flags for the button interrupt
}
