# Software PWM
The objective of Software PWM was to generate a 1 kHz signal in order to increment the duty cylce of an LED from 0% to 100%. The initial duty cycle was set to 50% upon running the code. Each button press would increment the LED by 10% until it reached 100%. Once this occured, the cycle would revert to 0% and repeat.

# Dependencies
The only dependency for the ```debounce_G2553.c``` is the header file for the MSP430 boards ```<msp430.h>```.

# MSP430G2553: Code Functionality
This code uses four different functions to configure the LED, button, TimerA0, TimerA1. The respective functions are LEDSteup(), ButtonSetup(), TimerA0Setup(), and TimerA1Setup().

```c
void LEDSetup(void)
{
    P1DIR |= (BIT0 + BIT6); // Set P1.0 (Green LED) & P1.6 (Red LED) to output direction
    P1OUT &= ~BIT0; // Clear P1.0 (Green LED OFF)
    P1SEL |= BIT6;  // Select bit for peripheral module function (Connect PWM to LED)
    P1SEL2 &= ~BIT6;    // Select bit 2 for peripheral module function (Connect PWM to LED)
}
```
This function configures the LED pins. The direction of P1.0 and P1.6 are set to be outputs. The green LED is also set to OFF initially, as it's the indicator LED that only comes on when the button is pressed. The select bits are then set to 10, which is what configures the pins to peripheral mode so that the PWM can be output directly to the red LED (P1.6).

```c
void ButtonSetup(void)
{
    P1DIR &= ~BIT3; // Set P1.3 (Button) to input direction
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
    TA0CTL = TASSEL_2 + MC_1;   // Timer A0 Control: SMCLK, Up mode
    TA0CCR0 = 1000; // Set capture compare register 0 to 1000 (Period)
    TA0CCR1 = 500;  // Set capture compare register 1 to 500 (Duty Cycle 50% initially)
    TA0CCTL1 = OUTMOD_7;    // Reset/Set mode selected for Timer A
}
```
The timer A0 setup is responsible for the duty cycle of the LED. Timer control sets the timer to use SMCLK and puts it in Up mode. Capture compare register 0 is set to 1000, which is the total duty cycle (100%). Capture compare register 1 is set to 500 which represents 50% duty cycle, which is the initial duty cycle of the LED.

```c
// Setup for Timer A1 (Debouncing)
void TimerA1Setup(void)
{
    TA1CCTL0 = CCIE;    // Capture compare interrupt enable
    TA1CCR0 = 6000; // Set capture compare resister for timer A1
    TA1CTL = TASSEL_1 + MC_1 + ID_1;    // Timer A1 Control: ACLK, Up Mode, Divide by 2
}
```
Timer A1 is then configured for the debouncing of the button. Capture compare mode interrupt is enabled, the register is set to 6000 for the delay, and the timer is configured to use ACLK, Up mode, and is divided by 2.
