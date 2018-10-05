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

    LEDSetup();     // Initialize LEDs
    ButtonSetup();  // Initialize button
    TimerA0Setup(); // Initialize Timer A0
    TimerA1Setup(); // Initialize Timer A1

    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupt

    __enable_interrupt();   // Enable interrupt algorithm

    while(1){}; // Infinite while loop
}
// Setup for LEDs (P1.0 and P1.6)
void LEDSetup(void)
{
    P1DIR |= (BIT0 + BIT6); // Set P1.0 (Green LED) & P1.6 (Red LED) to output direction
    P1OUT &= ~BIT0; // Clear P1.0 (Green LED OFF)
    P1SEL |= BIT6;  // Select bit for peripheral module function (Connect PWM to LED)
    P1SEL2 &= ~BIT6;    // Select bit 2 for peripheral module function (Connect PWM to LED)
}
// Setup for Button (P1.3)
void ButtonSetup(void)
{
    P1DIR &= ~BIT3; // Set P1.3 (Button) to input direction
    P1REN |= BIT3;  // Enable the internal resistor inside P1.3
    P1OUT |= BIT3;  // Configure resistor to pull-up functionality
    P1IE |= BIT3;   // Interrupt enable for P1.3
    P1IES |= BIT3;  // Edge select to trigger interrupt (Falling Edge)
    P1IFG &= ~BIT3; // Clear interrupt flags for P1.3
}
// Setup for Timer A0 (Duty Cycle)
void TimerA0Setup(void)
{
    TA0CTL = TASSEL_2 + MC_1;   // Timer A0 Control: SMCLK, Up mode
    TA0CCR0 = 1000; // Set capture compare register 0 to 1000 (Period)
    TA0CCR1 = 500;  // Set capture compare register 1 to 500 (Duty Cycle 50% initially)
    TA0CCTL1 = OUTMOD_7;    // Reset/Set mode selected for Timer A
}

// Setup for Timer A1 (Debouncing)
void TimerA1Setup(void)
{
    TA1CCTL0 = CCIE;    // Capture compare interrupt enable
    TA1CCR0 = 6000; // Set capture compare resister for timer A1
    TA1CTL = TASSEL_1 + MC_1 + ID_1;    // Timer A1 Control: ACLK, Up Mode, Divide by 2
}

// Interrupt service routine for Port 1
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    TA1CTL = TASSEL_1 + MC_1;   // Timer A1 Control: ACLK, Up mode

    if(P1IES & BIT3)    // If button is pressed (Falling edge detected on P1.3)
    {
        if (TA0CCR1<1000) // If duty cycle is less than 1000 (100%)
        {
            TA0CCR1 += 100;   // Increment duty cycle by 100 (10%) on each button press
        }
        else if (TA0CCR1>=1000)    // If duty cycle has reached 100% (Full brightness on LED)
        {
            TA0CCR1 = 0;    // Reset the the duty cycle (Turns LED OFF)
        }
    }
    P1OUT |= BIT0;  // Toggle P1.0 to ON
    P1IFG &= ~BIT3; // Reset Interrupt Flags on P1.3
    P1IE &= ~BIT3;  // Disable interrupt for P1.3
}

// Interrupt service routine for TimerA0
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
    P1OUT &= ~BIT0; // Toggle P1.0 Off
    TA1CTL = MC_0 + TACLR;  // Timer Control: Stop timer, Clear
    P1IE |= BIT3;   // Enable interrupt on P1.3 (Button) to detect next interrupt; Occurs after ~10ms in order to prevent bouncing
    P1IFG &= ~BIT3; // Clear flags for the button interrupt
}
