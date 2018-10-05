# Hardware PWM
Hardware PWM accomplishes the same behavior as Software PWM, with the main difference being the PWM is connected directly to the LED and no software configuration is required other than the initialization of the pins and timer module.

# Dependencies
The only dependency for the ```hPWM_G2553.c``` and the ```hPWM_FR2311``` is the header file for the MSP430 boards ```<msp430.h>```.

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

After all the initializations, the service routines are configured. There are two total routines, one for Port 1 and one for Timer A0. The port 1 routine first sets Timer A1 to use ACLK and run in Up mode. It then uses if statements to control the state of the duty cycle. If the button is pressed, ```if(P1IES & BIT3)``` and the duty cycle is less than 1000 ```if (TA0CCR1<1000)```, register one increments by 100 (10%). Once it reaches 1000, it is then reset to 0. This routine also toggles P1.0 with each button press, resets the interrupt flags on P1.3 and disables the button interrupt to prevent bouncing.

Moving onto the timer routine, P1.0 is toggled off, the timer is halted and cleared, and the interrupt is re-enabled so that future interrupts can be detected. The flags are also cleared on P1.3 once again.

# MSP430FR2311: Code Functionality
This code functions the same as above, with only a few key differences. For one, we have ot use the following line in order to disable the default high-impedance mode of the board.
```c
PM5CTL0 &= ~LOCKLPM5;
```
The other major difference is that Timer B is used to control the duty cycle and debouncing. For all intents and purposes of this lab, Timer B's functionality is the same as Timer B. Finally, the pins are different, with P1.0 being the red LED, P2.0 being the green LED, and P1.1 being the button. Other than that, the code runs the same and the overall structure is identical.


