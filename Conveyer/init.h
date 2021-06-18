#ifndef INIT_H
#define INIT_H

#include <math.h>
#include <stdint.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "driverlib/pin_map.h"
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"

#define GPIO_Direction  GPIO_PORTE_BASE
#define PINS GPIO_PIN_0 | GPIO_PIN_1 
#define AssDIR  GPIO_PIN_0  
#define DisDIR  GPIO_PIN_1
#define DIR   AssDIR | DisDIR
#define PWM_FREQUENCY 20

volatile uint32_t AssembConv;
volatile uint32_t DisAssemblyUnit;
volatile uint32_t DisAssembConv;

void StartAssemblyMotor() {
	ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_0); // ENABLES THE PWM_GEN_0 GENERATION

}
void StartDisassemblyMotor() {
	GPIOPinWrite(GPIO_Direction,DisDIR,~DisDIR);
	ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3); // ENABLES THE PWM_GEN_3 GENERATION   
}
void initClock(void){//Clock initialization
	SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5);
}
void initGPIO(void){//GPIO initialization and interrupt enabling
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_Direction, DIR);
}
void stopAssemblyStepper(void) {
	ROM_PWMGenDisable(PWM0_BASE, PWM_GEN_0);
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0,0);
}
void stopDisassemblyStepper(void) {
	ROM_PWMGenDisable(PWM1_BASE, PWM_GEN_3);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);
}
void InitAssemblyConveyer(){
	  volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;
    ui8Adjust = 83;
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_1); // SET PWM CLOCK BY: CLOCK_CPU
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // ENABLES PWM
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // ENABLES PB
    ROM_GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6); // DEFINE PWM IN PIN PB6
    ROM_GPIOPinConfigure(GPIO_PB6_M0PWM0); // CONFIGS PB6 AS M0PWM0
    ui32PWMClock = SysCtlClockGet() / 64; // PUT IN A VARIABLE THE PWM's CLOCK
		// TRANSFORMS THE CLOCK TO WORK IN A COUNTER THAT INITIALIZE AT 0 
		ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1; 
		// CONFIGS THE COUNTER AS DESCENT
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); 
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load); // SET THE COUNTER
    // SPLIT THE COUNTER BY 1000 AND MULTIPLIES BY THE ADJUST 
		ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000); 
		// CONFIGS THE PWM MODULE 0 AS OUT
    ROM_PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true); 
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_0); // ENABLES THE PWM GENERATION
		stopAssemblyStepper();
		GPIOPinWrite(GPIO_Direction,AssDIR,~AssDIR);
}
void InitDisassemblyConveyer(){
		volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;
    ui8Adjust = 83;
		GPIOPinWrite(GPIO_Direction,DisDIR,~DisDIR);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_1); // SET PWM CLOCK BY: CLOCK_CPU
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // ENABLES PWM1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // ENABLES PF
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3); // DEFINE PWM IN PIN PF3
    ROM_GPIOPinConfigure(GPIO_PF3_M1PWM7); // CONFIGS PF3 AS M1PWM7
    ui32PWMClock = SysCtlClockGet() / 64; // PUT IN A VARIABLE THE PWM's CLOCK
		// TRANSFORMS THE CLOCK TO WORK IN A COUNTER THAT INITIALIZE AT 0 
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
		// CONFIGS THE COUNTER AS DESCENT
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load); // SET THE COUNTER
		// SPLIT THE COUNTER BY 1000 AND MULTIPLIES BY THE ADJUST
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000); 
		// CONFIGS THE PWM MODULE 1 AS OUT
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3); // ENABLES THE PWM GENERATION
		stopDisassemblyStepper();		
}
void stepDisassemblyBackward(double mm) {
	int n;
	double rev;
	rev=mm/(360);
	rev=rev*1200;
	stopDisassemblyStepper();
	GPIOPinWrite(GPIO_Direction,DisDIR,DisDIR);
	for (n=0; n<rev; n++) {
		ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);
		SysCtlDelay(10000);
	}
	stopDisassemblyStepper();
}
void motorDisassemblyBackward(){
	stopDisassemblyStepper();
	GPIOPinWrite(GPIO_Direction,DisDIR,DisDIR);
	ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);
	SysCtlDelay(10000);
}
void motorAssemblyForward(){
	stopAssemblyStepper();
	GPIOPinWrite(GPIO_Direction,AssDIR,~AssDIR);
	ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_0);
	SysCtlDelay(10000);
}
void setConveyors(void){
		if(AssembConv==0)
				stopAssemblyStepper();
		else if(AssembConv==1)
				StartAssemblyMotor();
		if(DisAssembConv==1)
				StartDisassemblyMotor();
		else if(DisAssembConv==0)
				stopDisassemblyStepper();
}
#endif