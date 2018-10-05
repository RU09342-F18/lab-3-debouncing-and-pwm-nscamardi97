# Software PWM
The objective of Software PWM was to generate a 1 kHz signal in order to increment the duty cylce of an LED from 0% to 100%. The initial duty cycle was set to 50% upon running the code. Each button press would increment the LED by 10% until it reached 100%. Once this occured, the cycle would revert to 0% and repeat.

# Dependencies
The only dependency for the ```sPWM_G2553.c``` and ```sPWM_F5529``` is the header file for the MSP430 boards ```<msp430.h>```.

# MSP430G2553: Code Functionality
This code uses four different functions to configure the LED, button, TimerA0, TimerA1. The respective functions are LEDSteup(), ButtonSetup(), TimerA0Setup(), and TimerA1Setup().

```c
void LEDSetup(void)
{
    P1DIR |= (BIT0 + BIT6); // Set P1.0 (Green LED) & P1.6 (Red LED) to output direction
    P1OUT &= ~BIT0; // Clear P1.0 (Green LED OFF)
}
```
This function configures the LED pins. The direction of P1.0 and P1.6 are set to be outputs. The green LED is also set to OFF initially, as it's the indicator LED that only comes on when the button is pressed.

```c
void ButtonSetup(void)
{
    P1REN |= BIT3;  // Enable the internal resistor inside P1.3
    P1OUT |= BIT3;  // Configure resistor to pull-up functionality
    P1IE |= BIT3;   // Interrupt enable for P1.3
    P1IES |= BIT3;  // Edge select to trigger interrupt (Falling Edge)
    P1IFG &= ~BIT3; // Clear interrupt flags for P1.3
}
```
This function is responsible for initiating the button. The direction is set to output, the internal resistor is enabled and set to pull-up, and the interrupt is enabled for P1.3. The edge select is also set to interrupt on the falling edge and the flags are cleared. 

```c
void TimerA0Setup(void)
{
    TA0CTL = TASSEL_2 + MC_1;   // Timer Control: SMCLK (~ 32 kHz), Up mode, No division
    TA0CCTL0 = CCIE;    // Capture Compare Interrupt Enabled for Reg 0
    TA0CCTL1 = CCIE;    // Capture Compare Interrupt Enabled for Reg 1
    TA0CCR0 = 1000;     // Set Capture Compare Register 0 value to 1000 (Period)
    TA0CCR1 = 500;      // Set Capture Compare Register 1 value to 500 (Duty Cycle 50% initially)
}
```
The timer A0 setup is responsible for the duty cycle of the LED. Timer control sets the timer to use SMCLK and puts it in Up mode. Capture compare register 0 is set to 1000, which is the total duty cycle (100%). Capture compare register 1 is set to 500 which represents 50% duty cycle, which is the initial duty cycle of the LED.

```c
// Setup for Timer A1 (Debouncing)
void TimerA1Setup(void)
{
    TA1CTL = TASSEL_2 + MC_0;   // Timer Control: SMCLK, Stop mode, No division
    TA1CCTL0 = CCIE;    // Capture Compare Interrupt Enabled for Reg 0
    TA1CCR0 = 5000; // Set compare register to 5000 for debouncing delay
}
```
Timer A1 is then configured for the debouncing of the button. Capture compare mode interrupt is enabled, the register is set to 5000 for the delay, and the timer is configured to use ACLK and Up mode.

After initialization, the port and timer service routines are configured. There are a total of 4 service routines configured. First, the routine is configured for Port 1. This is done using and if and else if statement. If the value in CCR1 is less than 1000, the register is incremented to increase the duty cycle. Otherwise, if it is equal to or greater than 1000, the register is reset to 0. We also set the timer to use ACLK and Up mode, disable the button interrupt, clear the flags, and toggle the green indicator LED.

Next, the routine is configured for Timer1 A0, in which we enable the interrupt on P1.3, the turn off the green LED, stop the timer count and clear the interrupt flags on the button. This is done for debouncing off the button.

After that, the Timer0 A1 routine is configured, which is responsible for turning off the red LED and clearing the flags on the LED.

Finally, there is a routine for Timer0 A0. This controls the behavior of the red LED. If the cylce is less than 100% the LED is ON. Otherwise, the LED is off. Finally, the capture compare is cleared.

# MSP430F5529: Code Functionality
The code for the F5529 is pretty much identical, with the only differences being the pins for the buttons and LEDs. The pins for the F5529 are as follows: Button = P1.1, Red LED = P1.0, Green LED = P4.7. Other than that, the code structure is identical.


