# Software Debouncing
Due to the mechanical nature of a button or switch, the functionality does not always behave as desired. In order to combat this, something referred to as "debouncing" can be implemented. Button bounce results in mutliple depressions being detected and a program not running exactly the way it should. The objective of this lab is correct this behavior using interrupts and timers.

# Dependencies
The only dependency for the ```debounce_G2553.c``` is the header file for the MSP430 boards ```<msp430.h>```.

# MSP430G2553: Code Functionality
First and foremost, the built-in watchdog timer is disabled using the line ```WDTCTL = WDTPW | WDTHOLD```. In order to verify the functionality of our debounce code, the red LED is triggered on each interrupt. To do this, P1.6 is configured as an output by setting the direction using PDIR. The button itself is then configured by setting P1.3 to an output. In order to have the button function properly, the internal resistor in P1.3 is enabled and set to pull-up. This gives the desired functionality so that when the button is pressed, the LED will trigger.

Once the LED and button are configured, the interrupts have to be set up. First, we enable an interrupt to trigger on the button press using ``` P1IE = BIT3```. This basically sets the interrupt to go off whenever the state of P1.3, which is the button, changes. The interrupt flags are then cleared, and the capture compare interrupt is enabled for Timer A0. To enable the interrupt algorithm to actually take effect, ```__enable_interrupt()``` is used. Finally, the main function is concluded by inserting an infinite while loop ```while(1)``` in order for the code to continue running.

Moving on from the main function, there are two interrupt service routines that are configured. First, the interrupt routine for Port 1 is configured.
```c
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1OUT ^= BIT6;  // Toggle P1.6 (Red LED)
    P1IE &= ~BIT3;  // Turn off interrupt for button to prevent bouncing due to unintentional interrupts

    TACCR0 = 10000; // Set capture compare register value for timer count (~10ms)
    TA0CTL = TASSEL_2 + MC_1 + TACLR;   // Timer Control: SMCLK (~1 MHz), Up mode, Clear
}
```
Once an interrupt is detected on P1.3, the red LED is toggled using an exclusive-OR. Also, the interrupt on P1.3 is disabled in order to prevent another interrupt from being triggered due to bouncing. After disabling the interrupt, the timer is configured using SMCLK, which runs at approximately 1 MHz It is also set to run in Up mode, which makes the timer count to the value stored in the capture compare register, and then it is cleared. The capture compare register is set to contain the value 10000, which gives us a timer count of approximately 10ms.

The interrupt routine for the timer was then configured.
```c
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    TA0CTL = MC_0 + TACLR;  // Timer Control: Stop timer, Clear
    P1IE |= BIT3;   // Enable interrupt on P1.3 (Button) to detect next interrupt; Occurs after ~10ms in order to prevent bouncing
    P1IFG &= ~BIT3; // Clear flags for the button interrupt
}
```
Once the code enters this routine, the timer is set to stop counting and is then cleared. We also have to re-enable the interrupt on P1.3 so that another button push will toggle the LED off. Lastly, the flags are cleared and the program is set to run again.

# MSP432P401R: Code Functionality
The code for the MSP432P401R functions in the same fashion as the G2553. There are a few key differences in terms of syntax. For one, the LED pin is P2.2, which controls the blue LED on the board. Also, the button is controlled by P1.1. There interrupt enable is given by the line ```__enable_irq()```. Also, the board's sleep on exit feature is disabled using ```SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;```. Finally, the service routine functions are initiated slightly different with the Port 1 routine being ```void PORT1_IRQHandler(void)``` and the timer one being ```void TA0_0_IRQHandler(void)```. Other than that, the code runs the same.






