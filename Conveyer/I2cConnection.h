#ifndef I2CCONNECTION_H
#define I2CCONNECTION_H

#include "init.h"
#define SLAVE_ADDRESS          4

static uint32_t g_ui32DataRx=0XFF;

//Disassembly Subsystem Subroutine
void I2C2SlaveIntHandler(void){	
    // Clear the I2C2 interrupt flag.
    I2CSlaveIntClear(I2C2_BASE);
		if(I2CSlaveStatus(I2C2_BASE)==I2C_SLAVE_ACT_TREQ){
			//Sends to the Disassembly Subsystem to know which
			//product arrived the tall(4) or the short(8)
			I2CSlaveDataPut(I2C2_BASE,DisAssemblyUnit);			
		}
		else{
		g_ui32DataRx =I2CSlaveDataGet(I2C2_BASE);
		if(g_ui32DataRx==3)
			StartDisassemblyMotor();
		else if(g_ui32DataRx==2)
			stopDisassemblyStepper();
		else if(g_ui32DataRx==5){
			stepDisassemblyBackward(2000);
			g_ui32DataRx=10;
			}
		I2CSlaveStatus(I2C2_BASE);
	}
}
//Assembly Subsystem Subroutine
void I2C1SlaveIntHandler(void){	
    // Clear the I2C1 interrupt flag.
    I2CSlaveIntClear(I2C1_BASE);
		g_ui32DataRx =I2CSlaveDataGet(I2C1_BASE);
		if(g_ui32DataRx==0)
			stopAssemblyStepper();
		else if(g_ui32DataRx==1)
			StartAssemblyMotor();		
		I2CSlaveStatus(I2C1_BASE);
}
//Feeding Subsystem Subroutine
void I2C0SlaveIntHandler(void){	
    // Clear the I2C0 interrupt flag.
    I2CSlaveIntClear(I2C0_BASE);
		g_ui32DataRx =I2CSlaveDataGet(I2C0_BASE);
		if(g_ui32DataRx==0)
			stopAssemblyStepper();
		else if(g_ui32DataRx==1)
			StartAssemblyMotor();
		I2CSlaveStatus(I2C0_BASE);
}
void InitI2C0(void){
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_3|GPIO_PIN_2,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	ROM_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	ROM_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	ROM_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	ROM_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	ROM_I2CMasterInitExpClk(I2C0_BASE, MAP_SysCtlClockGet(), false);
	HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
	IntEnable(INT_I2C0);
  I2CSlaveIntEnableEx(I2C0_BASE, I2C_SLAVE_INT_DATA);
  I2CSlaveEnable(I2C0_BASE);
  I2CSlaveInit(I2C0_BASE, SLAVE_ADDRESS);
	I2CIntRegister(I2C0_BASE,I2C1SlaveIntHandler);
	IntPrioritySet(INT_I2C0,0);
}
void InitI2C1(void){
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
	//enable GPIO peripheral that contains I2C1
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Configure the pin muxing for I2C1 functions on port A6 and A7.
	ROM_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	ROM_GPIOPinConfigure(GPIO_PA7_I2C1SDA);
	// Select the I2C function for these pins.
	ROM_GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
	ROM_GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
	//clear I2C FIFOs
	HWREG(I2C1_BASE + I2C_O_FIFOCTL) = 80008000;
	IntEnable(INT_I2C1);
  I2CSlaveIntEnableEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
  I2CSlaveEnable(I2C1_BASE);
  I2CSlaveInit(I2C1_BASE, SLAVE_ADDRESS);
	I2CIntRegister(I2C1_BASE,I2C1SlaveIntHandler);
	IntPrioritySet(INT_I2C1,0);		
}
void InitI2C2(void){
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
	ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
	//enable GPIO peripheral that contains I2C 2
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	// Configure the pin muxing for I2C2 functions on port E4 and E5.
	ROM_GPIOPinConfigure(GPIO_PE4_I2C2SCL);
	ROM_GPIOPinConfigure(GPIO_PE5_I2C2SDA);
	// Select the I2C function for these pins.
	ROM_GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
	ROM_GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);
	//clear I2C FIFOs
	HWREG(I2C2_BASE + I2C_O_FIFOCTL) = 80008000;
	IntEnable(INT_I2C2);
  I2CSlaveIntEnableEx(I2C2_BASE, I2C_SLAVE_INT_DATA);
  I2CSlaveEnable(I2C2_BASE);
  I2CSlaveInit(I2C2_BASE, SLAVE_ADDRESS);
	I2CIntRegister(I2C2_BASE,I2C2SlaveIntHandler);
	IntPrioritySet(INT_I2C2,0);
}
#endif