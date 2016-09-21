#include <msp430.h> 

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    InitializeHardware();
    InitializeVariables();

	_BIS_SR(GIE);
	for (;;) {
	}
	return 0;
}
