/*Author: Nick Scamardi
 *Written: October 3, 2018
 *Last Updated: October 3, 2018
*/

#include <msp432.h>

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Disable the watchdog timer

    P2DIR |= BIT2;  // Set P2.2 (Blue LED) to output direction

    P1DIR &= ~BIT1;  // Set P1.1 (Button) to input direction
    P1REN |= BIT1;   // Enable internal resistor
    P1OUT |= BIT1;   // Set resistor to pull-up

    P1IES |= BIT1;   // Set interrupt edge select for button to falling edge
    P1IE |= BIT1;    // Enable interrupt for the button
    P1IFG &= ~BIT1;  // Clear the interrupt flags on the button

    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);   // Interrupt enable for P1
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // Interrupt enable for TimerA0

    TA0CCTL0 = CCIE;    // Capture Compare Interrupt Enable for Timer A

    __enable_irq(); // Enable global interrupt

    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Disable the sleep on exit feature
    __sleep();  // Sleep Mode

    while(1){};   // Infinite while loop
}

// Interrupt service routine for P1
void PORT1_IRQHandler(void)
{
    P2OUT ^= BIT2;  // Toggle Blue LED (P2.2) ON
    P1IE &= ~BIT1;  // Disable interrupt on button
    P1IFG &= ~BIT1; // Clear interrupt flags for button
    TA0CTL = TASSEL_2 + MC_1 + TACLR;   // Timer Control: SMCLK (~1 MHz), Up mode, Clear
    TA0CCR0 = 10000;    // Set capture compare register value for timer count (~10ms)
}

// Interrupt service routine for Timer A
void TA0_0_IRQHandler(void)
{
    TA0CTL = MC_0 + TACLR; // Timer Control: Stop Mode (Halt Timer), Clear settings
    P1IE |= BIT1;   // Re-enable interrupt for button to detect next push
    P1IFG &= ~BIT1; // Clear interrupt flags on button
}

