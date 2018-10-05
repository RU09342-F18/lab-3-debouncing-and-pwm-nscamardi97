/*Author: Nick Scamardi
 *Written: October 4, 2018
 *Last Updated: October 4, 2018
*/

#include <msp430.h>

void LEDSetup(void);
void ButtonSetup(void);
void TimerA0Setup(void);
void TimerA1Setup(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Disable the watchdog timer
    LEDSetup(); // Initialize the LEDs
    ButtonSetup();  // Initialize Button (P1.3)
    TimerA0Setup(); // Initialize TimerA0
    TimerA1Setup(); // Initialize TimerA1

    __bis_SR_register(LPM0_bits + GIE); // Enter Low Power Mode once interrupt is detected

    __enable_interrupt();   // Enable interrupt algorithm

    while(1){}; // Infinite while loop
}

void ButtonSetup(void)
{
    P1REN |= BIT3;  // Enable internal resistor on P1.3
    P1OUT |= BIT3;  // Set internal resistor to pull-up
    P1IE = BIT3;    // Interrupt enable for P1.3 (Interrupt on button state change)
    P1IES = BIT3;   // Edge select for interrupt (Falling Edge); Detects interrupt when the button is pushed down
    P1IFG &= ~BIT3; // Clear Interrupt Flags for P1.3
}

// Configure LEDs
void LEDSetup(void)
{
    P1DIR |= (BIT0 + BIT6); // Set P1.0 and P1.6 to output direction
    P1OUT &= ~BIT0; // Set P1.0 OFF (Indicator LED)
    P1OUT |= BIT6;  // Set P1.6 ON
}

// Configure Timer A0 (Duty Cycle)
void TimerA0Setup(void)
{
    TA0CTL = TASSEL_2 + MC_1;   // Timer Control: SMCLK (~ 32 kHz), Up mode, No division
    TA0CCTL0 = CCIE;    // Capture Compare Interrupt Enabled for Reg 0
    TA0CCTL1 = CCIE;    // Capture Compare Interrupt Enabled for Reg 1
    TA0CCR0 = 1000;     // Set Capture Compare Register 0 value to 1000 (Period)
    TA0CCR1 = 500;      // Set Capture Compare Register 1 value to 500 (Duty Cycle 50% initially)
}

// Configure Timer A1 (Debounce)
void TimerA1Setup(void)
{
    TA1CTL = TASSEL_2 + MC_0;   // Timer Control: SMCLK, Stop mode, No division
    TA1CCTL0 = CCIE;    // Capture Compare Interrupt Enabled for Reg 0
    TA1CCR0 = 5000; // Set compare register to 5000 for debouncing delay
}

// Interrupt service routine for Port 1 (Button)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (TA0CCR1<1000) // If duty cycle is less than 1000 (100%)
    {
        TA0CCR1 += 100;   // Increment duty cycle by 100 (10%) on each button press
    }
    else if (TA0CCR1>=1000)    // If duty cycle has reached 100% (Full brightness on LED)
    {
        TA0CCR1 = 0;    // Reset the the duty cycle (Turns LED OFF)
    }

    TA1CTL = TASSEL_1 + MC_1;   // Timer Control: ACLK, Up mode
    P1IE &= ~BIT3;  //Disable interrupt on P1.3
    P1IFG &= ~BIT3; // Clear interrupt flags on P1.3
    P1OUT ^= BIT0;  // Toggle Green LED
}

// Interrupt routine for Timer A0
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
    P1IE |= BIT3;  // Enable interrupt on P1.3
    P1OUT &= ~BIT0; // Turn OFF Green LED
    TA1CTL = MC_0 + TACLR;    // Timer Control: Stop count, Clear timer
    P1IFG &= ~BIT3; //Disables interrupt flags on P1.3

}

// Interrupt routine for Timer A1
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1 (void)
{
    P1OUT &= ~BIT6; // Turn off Red LED
    TA0CCTL1 &= ~BIT0; // Clear Flag on P1.0
}

// Interrupt routine for Timer A0
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
    if(TA0CCR1 < 1000)  // If duty cycle is less than 100%
        {
        P1OUT ^= BIT6;  // Red LED is ON
        }
    else
    {
        P1OUT &= ~BIT6; // Red LED is OFF
    }

    TA0CCTL0 &= ~BIT0; // Reset flag on P1.0
}
