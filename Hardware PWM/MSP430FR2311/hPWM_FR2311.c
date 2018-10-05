/*Author: Nick Scamardi
 *Written: October 4, 2018
 *Last Updated: October 4, 2018
*/

#include <msp430.h>

void LEDSetup(void);
void ButtonSetup(void);
void TimerB0Setup(void);
void TimerB1Setup(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Disable the watchdog timer
    PM5CTL0 &= ~LOCKLPM5;   //Disable the default high-impedance mode

    LEDSetup();     // Initialize LEDs
    ButtonSetup();  // Initialize button
    TimerB0Setup(); // Initialize Timer B0
    TimerB1Setup(); // Initialize Timer B1

    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupt

    __enable_interrupt();   // Enable interrupt algorithm

    while(1){}; // Infinite while loop
}
// Setup for LEDs (P1.0 and P2.0)
void LEDSetup(void)
{
    P1DIR |= BIT0;  // Set P1.0 (Red LED) to output direction
    P2DIR |= BIT0;  // Set P2.0 (Green LED) to output direction
    P1OUT &= ~BIT0; // Clear P1.0 (Green LED OFF)
    P2SEL0 |= BIT0;  // Select bit 0 for peripheral module function (Connect PWM to LED)
    P2SEL1 &= ~BIT0;    // Select bit 1 for peripheral module function (Connect PWM to LED)
}
// Setup for Button (P1.1)
void ButtonSetup(void)
{
    P1DIR &= ~BIT1; // Set P1.1 (Button) to input direction
    P1REN |= BIT1;  // Enable the internal resistor inside P1.1
    P1OUT |= BIT1;  // Configure resistor to pull-up functionality
    P1IE |= BIT1;   // Interrupt enable for P1.1
    P1IES |= BIT1;  // Edge select to trigger interrupt (Falling Edge)
    P1IFG &= ~BIT1; // Clear interrupt flags for P1.1
}

// Setup for Timer B0 (Debouncing)
void TimerB0Setup(void)
{
    TB0CCTL0 = CCIE;    // Capture compare interrupt enable
    TB0CCR0 = 6000; // Set capture compare resister for timer B0
    TB0CTL = TBSSEL_1 + MC_1 + ID_1;    // Timer B0 Control: ACLK, Up Mode, Divide by 2
}

// Setup for Timer B1 (Duty Cycle)
void TimerB1Setup(void)
{
    TB1CTL = TBSSEL_2 + MC_1;   // Timer B1 Control: SMCLK, Up mode
    TB1CCR0 = 1000; // Set capture compare register 0 to 1000 (Period)
    TB1CCR1 = 500;  // Set capture compare register 1 to 500 (Duty Cycle 50% initially)
    TB1CCTL1 = OUTMOD_7;    // Reset/Set mode selected for Timer B
}

// Interrupt service routine for Port 1
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    TB0CTL = TBSSEL_1 + MC_1;   // Timer B0 Control: ACLK, Up mode

    if(P1IES & BIT1)    // If button is pressed (Falling edge detected on P1.1)
    {
        if (TB1CCR1<1000) // If duty cycle is less than 1000 (100%)
        {
            TB1CCR1 += 100;   // Increment duty cycle by 100 (10%) on each button press
        }
        else if (TB1CCR1>=1000)    // If duty cycle has reached 100% (Full brightness on LED)
        {
            TB1CCR1 = 0;    // Reset the the duty cycle (Turns LED OFF)
        }
    }
    P1OUT ^= BIT0;  // Toggle P1.0 to ON
    P1IES |= BIT1;  // Set Interrupt edge select on P1.1 to falling edge
    P1IFG &= ~BIT1; // Reset Interrupt Flags on P1.1
    P1IE &= ~BIT1;  // Disable interrupt for P1.1
}

// Interrupt service routine for TimerB0
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0(void)
{
    P1OUT &= ~BIT0; // Toggle P1.0 Off
    TB0CTL = MC_0 + TBCLR;  // Timer B0 Control: Stop timer, Clear
    P1IE |= BIT1;   // Enable interrupt on P1.1 (Button) to detect next interrupt; Occurs after ~10ms in order to prevent bouncing
    P1IFG &= ~BIT1; // Clear flags for the button interrupt
}
