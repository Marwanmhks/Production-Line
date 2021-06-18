#ifndef CANCONNECTION_H
#define CANCONNECTION_H

#include "init.h"

// msg recieved flag
volatile bool rxFlag = 0;
// error flag
volatile bool errFlag = 0; 
// the CAN msg object
tCANMsgObject msg;
// 8 byte buffer for rx message data
unsigned char msgData[8];

// CAN interrupt handler
void CANIntHandler(void) {
	// read interrupt status
	unsigned long status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); 
	// controller status interrupt
	if(status == CAN_INT_INTID_STATUS) { 
		status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
		errFlag = 1;
	} 
	// msg object 1
	else if(status == 1) { 
		// clear interrupt
		CANIntClear(CAN0_BASE, 1);
		// set rx flag
		rxFlag = 1; 
		// clear any error flags	
		errFlag = 0; 
	}
}
void InitCAN0(void){
	// Set up CAN0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB4_CAN0RX);
	GPIOPinConfigure(GPIO_PB5_CAN0TX);
	GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	CANInit(CAN0_BASE);
	CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);
	CANIntRegister(CAN0_BASE, CANIntHandler); 
	CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
	IntEnable(INT_CAN0);
	CANEnable(CAN0_BASE);
	msg.ui32MsgID = 0;
	msg.ui32MsgIDMask = 0;
	msg.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	msg.ui32MsgLen = 8; // allow up to 8 bytes
	// Load msg into CAN peripheral message object 1 
	//so it can trigger interrupts on any matched rx messages
	CANMessageSet(CAN0_BASE, 1, &msg, MSG_OBJ_TYPE_RX);
}
#endif 