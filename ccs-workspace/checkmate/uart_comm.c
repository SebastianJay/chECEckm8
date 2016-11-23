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

/* EUSCI A0 UART ISR  */
void EUSCIA0_IRQHandler(void)
{
	uartReceiveISR();
}

/* EUSCI A2 UART ISR  */
void EUSCIA2_IRQHandler(void)
{
	uartReceiveISR();
}

void uartReceiveISR()
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(UART_EUSCI_BASE);
    MAP_UART_clearInterruptFlag(UART_EUSCI_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
		gReceiveBuffer[gReceiveBufferIndex++] = MAP_UART_receiveData(UART_EUSCI_BASE);
		if (gReceiveBufferIndex >= UART_RECEIVE_BUFFER_LENGTH)
		{
			gReceiveBufferIndex = 0;
		}
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

	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(UART_RX_PORT,
			UART_RX_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(UART_TX_PORT,
			UART_TX_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

	MAP_UART_initModule(UART_EUSCI_BASE, &uartConfig);
	MAP_UART_enableModule(UART_EUSCI_BASE);

	MAP_UART_enableInterrupt(UART_EUSCI_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
	MAP_Interrupt_enableInterrupt(UART_EUSCI_INT);
}

void send(piece_movement* move)
{
	char first = move->rStart * 8 + move->cStart;
	char second = move->rEnd * 8 + move->cEnd;
	MAP_UART_transmitData(UART_EUSCI_BASE, first);
	MAP_UART_transmitData(UART_EUSCI_BASE, second);
}

signed char receive(piece_movement* move, piece_movement* other_move)
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

	if (first == 0xFF && second == 0xFF)
	{
		// response was error - previous human move was invalid
		return ERROR;
	}
	else if ((first & 0x80) != 0)
	{
		// if MSB set, another move is incoming
		first = first & ~0x80;
		while (1)
		{
			if (gReceiveBufferIndex >= 4)
			{
				break;
			}
		}
		char third = gReceiveBuffer[2];
		char fourth = gReceiveBuffer[3];
		other_move->rStart = third / 8;
		other_move->cStart = third % 8;
		other_move->rEnd = fourth / 8;
		other_move->cEnd = fourth % 8;
	}
	else
	{
		// invalidate other_move so caller knows not to use it
		other_move->rStart = 0xFF;
		other_move->cStart = 0xFF;
		other_move->rEnd = 0xFF;
		other_move->cEnd = 0xFF;
	}

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
	piece_movement other;
	while (1)
	{
		// place a breakpoint before sending and modify move through debugger
		;
		send(&move);
		receive(&move, &other);
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
		MAP_UART_transmitData(UART_EUSCI_BASE, testMessage[i]);
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
