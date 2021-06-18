#include "I2cConnection.h"
#include "CanConnection.h"

//Main routine
int main(void) {
	initClock();
	initGPIO();
	InitI2C1();
	InitI2C2();
	IntMasterEnable();
	InitAssemblyConveyer();
	InitDisassemblyConveyer();
	StartDisassemblyMotor();
	StartAssemblyMotor();
	while(1){
		// rx interrupt has occured
		if(rxFlag) { 
				 // set pointer to rx buffer
				msg.pui8MsgData = msgData;
				// read CAN message object 1 from CAN peripheral
				CANMessageGet(CAN0_BASE, 1, &msg, 0); 
				// clear rx flag
				rxFlag = 0; 
				AssembConv = msgData[0];
				DisAssemblyUnit= msgData[1];
				DisAssembConv= msgData[2] ; 
		}
		setConveyors();

	}
}