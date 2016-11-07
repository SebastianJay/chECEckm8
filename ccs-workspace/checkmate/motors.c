/*
 * motors.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Jay
 */

#include "driverlib.h"
#include "motors.h"

/*
 * Timer_A PWM Configuration Parameter
 * PWM is used to control the servo
 */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
		SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_1,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
		SERVO_PULSE_WIDTH_1
};

void initMotors()
{
	// clock source
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // choose GPIO pin
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /*
    // TODO understand how to remap pins, so we can possibly change PWM pin from 2.4
    // Port mapper configuration register
	const uint8_t port_mapping[] =
	{
			//Port P2:
			PM_TA0CCR0A, PM_TA0CCR0A, PM_TA0CCR0A, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE
	};
	MAP_PMAP_configurePorts((const uint8_t *) port_mapping, P2MAP, 1, PMAP_DISABLE_RECONFIGURATION);
	*/

    // Configure Timer A PWM
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}

void debugServoLoop()
{
	while(1)
	{
		int i;
		engageMagnet();
		for (i = 0; i < 1000000; i++);
		disengageMagnet();
		for (i = 0; i < 1000000; i++);
	}
}

void engageMagnet()
{
	pwmConfig.timerPeriod = SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_1;
	pwmConfig.dutyCycle = SERVO_PULSE_WIDTH_1;
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
	// could use busy loop so action completes before code proceeds
}

void disengageMagnet()
{
	pwmConfig.timerPeriod = SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_2;
	pwmConfig.dutyCycle = SERVO_PULSE_WIDTH_2;
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
	// could use busy loop so action completes before code proceeds
}
