/*
 * uart_comm.c
 *
 *  Created on: Oct 23, 2016
 *      Author: Jay
 */

#include "driverlib.h"

#include "uart_comm.h"
#include "board_state.h"

/*
 * UART configuration for Oversampling at 12 MHz clock, Baud Rate 9600
 *
 * Values calculated at:
 * http://processors.wiki.ti.com/index.php/USCI_UART_Baud_Rate_Gen_Mode_Selection
 */
const eUSCI_UART_Config uartConfig =
{
		EUSCI_A_UART_CLOCKSOURCE_SMCLK,		// source clock
		78,									// BRDIV
		2,									// UCxBRF
		0,									// UCxBRS
		EUSCI_A_UART_NO_PARITY,				// No parity
		EUSCI_A_UART_LSB_FIRST,				// LSB first
		EUSCI_A_UART_ONE_STOP_BIT,			// one stop bit
		EUSCI_A_UART_MODE,					// UART mode
		EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION	// oversampling mode
};

/* EUSCI A0 UART ISR - Echoes data back to PC host */
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
		gReceiveBuffer[gReceiveBufferIndex++] = MAP_UART_receiveData(EUSCI_A0_BASE);
		if (gReceiveBufferIndex >= UART_RECEIVE_BUFFER_LENGTH)
		{
			gReceiveBufferIndex = 0;
		}
    }

}

void helloWorldSend()
{
	const char* testMessage = "Hello world!";
	const int len = 12;
	int i;
	for (i = 0; i < len; i++)
	{
		// transmit will block until transmit flag is reset
		MAP_UART_transmitData(EUSCI_A0_BASE, testMessage[i]);
	}
}

void helloWorldReceive()
{
	// light red LED
	MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

	// spin while ISR fills up receive buffer
	while (1)
	{
		if (gReceiveBufferIndex >= 12)
		{
			break;
		}
	}

	// check if message matches
	const char* compareMessage = "Hello world!";
	const int len = 12;
	int i;
	int comp = 1;
	for (i = 0; i < len; i++)
	{
		if (compareMessage[i] != gReceiveBuffer[i])
		{
			comp = 0;
			break;
		}
	}

	// if it does, unlight red LED
	if (comp) {
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
	}
}

void send(piece_movement* move)
{
	char first = move->rStart * 8 + move->cStart;
	char second = move->rEnd * 8 + move->cEnd;
	MAP_UART_transmitData(EUSCI_A0_BASE, first);
	MAP_UART_transmitData(EUSCI_A0_BASE, second);
}

signed char receive(piece_movement* move)
{
	// spin while ISR fills up receive buffer
	while (1)
	{
		if (gReceiveBufferIndex >= 2)
		{
			break;
		}
	}
	// unpack args
	char first = gReceiveBuffer[0];
	char second = gReceiveBuffer[1];

	// TODO check for error conditions
	// if bad code -> return ERROR;

	move->rStart = first / 8;
	move->cStart = first % 8;
	move->rEnd = second / 8;
	move->cEnd = second % 8;

	// reset index, discard any remaining message
	gReceiveBufferIndex = 0;
	return TRUE;
}

void debugGameLoop()
{
	piece_movement move;
	while (1)
	{
		// place a breakpoint before sending and modify move through debugger
		;
		send(&move);
		receive(&move);
	}
}

void initUART()
{
	// init globals
	gReceiveBufferIndex = 0;

	// init USCI module
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
			GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
			GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
	MAP_UART_enableModule(EUSCI_A0_BASE);

	MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
	MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
}
