#include "DSP28x_Project.h"
#include <math.h>

int PedalOut=0;  //main output from pedal
int alfa=alfa_n; //
int mb_temp=0;
int SteerOut=0;  //main output from steering wheel
int i=0,t=0;
int voltage_bms=0;
int count;
int temp1=0, temp2=0;
int m=0;
float current_bms=0;
extern long speed, speedr, speedf;
extern int flag;
extern int minspeedr, minspeedf;
extern int error_code;

/*
interrupt void cpu_timer0_isr(void)
{
    //summ_average();
    // Acknowledge this interrupt to receive more interrupts from group 1
    error_code=ECanbMboxes.MBOX31.MDL.word.LOW_WORD;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
*/

interrupt void main_timer_isr(void) {
	calc_speed();

	calc_PedalOut();

	//if (PedalOut>1200) PedalOut=1200;
	bspd();
	//tang=tan(SteerOut*3.1415/180);
	SteerOut=calc_steer_wheel_spi();
	if (fabs(SteerOut)>5)
	    send_CAN_motors(differential_r(PedalOut,SteerOut)*flag, differential_l(PedalOut,SteerOut)*flag);
	else
	    send_CAN_motors(PedalOut*flag, PedalOut*flag);

    voltage_bms=(ECanbMboxes.MBOX28.MDL.all)*0.0015;
    current_bms=ECanbMboxes.MBOX29.MDL.word.HI_WORD/10;

    send_CAN_priborka(minspeedr*100,speedr*100);
    send_CAN_steer(SteerOut);
    send_CAN_datalogger(minspeedr*100,speedr*100,minspeedf*100,speedf*100);

#ifdef FLASH
	shutdown_detect();
#endif
}

void main(void){
// Initialize System Control: PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();

	// Copy time critical code and Flash setup code to RAM.
    // The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
    // symbols are created by the linker. Refer to the linker files.
#ifdef FLASH
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
#endif
    // Call Flash Initialization to setup flash waitstates.
    // This function must reside in RAM.
    InitFlash(); // Call the flash wrapper init function.

// Clear all interrupts and initialize PIE vector table, disable CPU interrupts
	DINT;
// Initialize PIE control registers to their default state
// The default state is all PIE interrupts disabled and flags are cleared
	InitPieCtrl();
// Disable CPU interrupts and clear all CPU interrupt flags
	IER = 0x0000;
	IFR = 0x0000;
// Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR)
// This will populate the entire table, even if the interrupt is not used in this example
	InitPieVectTable();
// Interrupts that are used in this example are re-mapped to ISR functions found within this file.
	EALLOW;
	//PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &main_timer_isr;
	PieVectTable.ECAN0INTB = &can_int_isr;
	EDIS;

	InitGpio();

	InitECap();

	InitSpi();
	InitSpiGpio();

	InitAdc();  // For this example, init the ADC

	InitCpuTimers();

	InitECan();
	InitECanGpio();

	InitXIntrupt();

	rtd();

	while(1)
	{

	}
}
//========================================================================

