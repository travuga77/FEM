//######################################################################
// MAIN FILE OF FE MADI PROJECT
//######################################################################

#include "DSP28x_Project.h"
#include "pid_controller.h"
#include <math.h>

int PedalOut=0;  //main output from pedal
int send_motors=0;
int alfa, alfa_n=ALFA; //
int mb_temp=0;
int SteerOut=0;  //main output from steering wheel
int i=0,t=0;
int count;
int temp1=0, temp2=0;
int send_motors_min_1=0;
float phase_n=PHASE;
float slip=0.0, slip_n=SLIP;
float voltage_left_motor=0.0, voltage_right_motor=0.0;
float current_left_motor=0.0, current_right_motor=0.0;
float voltage_bms=0.0;
float current_bms=0.0;
float current_acc_cont=0.0;
extern float speed, speedr, speedf, speedLF, speedRF, speedLR, speedRR;
extern int flag;
extern int minspeedr, minspeedf;
extern int error_code;
bool racelogic=false;
int racelogic_count=0;
extern int mode;


PIDControl mainPID;

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


    if (speedf<0.1) {
        racelogic=true;
        racelogic_count=0;
    } else {
        if (racelogic and speedf<50)
            racelogic_count+=5;
        else {
            racelogic=false;
        }
    }

	calc_PedalOut();

	send_CAN_sync_message();
	//if (PedalOut>1200) PedalOut=1200;
	bspd();
	//tang=tan(SteerOut*3.1415/180);
	SteerOut=calc_steer_wheel_spi();

	if (GpioDataRegs.GPADAT.bit.GPIO9==1) GpioDataRegs.GPASET.bit.GPIO5=1; else GpioDataRegs.GPACLEAR.bit.GPIO5=1;



    voltage_bms = (ECanbMboxes.MBOX28.MDL.all)*0.0015;
    current_bms = ECanbMboxes.MBOX29.MDL.word.LOW_WORD;
    voltage_left_motor = ECanbMboxes.MBOX15.MDL.word.LOW_WORD/10;
    voltage_right_motor = ECanbMboxes.MBOX16.MDL.word.LOW_WORD/10;
    current_left_motor = ECanbMboxes.MBOX15.MDL.word.HI_WORD/10;
    current_right_motor = ECanbMboxes.MBOX16.MDL.word.HI_WORD/10;
    current_acc_cont = 0.78125*ECanbMboxes.MBOX20.MDL.word.LOW_WORD-400;
    if (current_acc_cont<0) current_acc_cont=0;

    steering_buttons();

    slip = (speedr-speedf)/(speedf+0.01);
    //PIDInputSet(&mainPID,slip);
    //PIDCompute(&mainPID);
    send_motors=PedalOut;
    //send_motors*=(100-PIDOutputGet(&mainPID))/100;

    //if (slip>SLIP) alfa=0; else alfa=alfa_n;
    if (slip<slip_n*phase_n && slip>0) alfa=alfa_n*cos(slip*1.5708/slip_n);
    if (slip>=slip_n*phase_n) alfa=alfa_n*cos(1.5708*phase_n);
    if (slip<=0 or slip_n<=0.03)    alfa=alfa_n;


    if (send_motors>=send_motors_min_1+alfa) {
        send_motors=send_motors_min_1+alfa;
    }
    send_motors_min_1=send_motors;

    //(98-67)=31 - 100%
    //(voltage_bms-67) - %


    if (send_motors<0) send_motors=0;
    if (send_motors>4095) send_motors=4095;

    if (fabs(SteerOut)>5)
        send_CAN_motors(differential_r(send_motors,SteerOut)*flag, differential_l(send_motors,SteerOut)*flag);
    else
        send_CAN_motors(send_motors*flag, send_motors*flag);

    if (mode==0)    send_CAN_priborka(alfa,racelogic_count);
    if (mode==1)    send_CAN_priborka(slip_n*100,racelogic_count);
    if (mode==2)    send_CAN_priborka(phase_n*100,racelogic_count);
    //send_CAN_priborka(voltage_bms,speedf);
    send_CAN_steer(SteerOut);
    send_CAN_datalogger(slip*100,send_motors,speedf,speedr,0,voltage_bms,current_bms,current_acc_cont,0,0,voltage_left_motor,current_left_motor,0,0,voltage_right_motor,current_right_motor);

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

	// Enable Xint3 in the PIE: Group 12 interrupt 1
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  // Enable PIE Group 9 INTx7

    IER |= M_INT12; // Enable CPU int12
    IER |= M_INT13;
// Interrupts that are used in this example are re-mapped to ISR functions found within this file.
	EALLOW;
	//PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &main_timer_isr;
	PieVectTable.ECAN0INTB = &can_int_isr;
	EDIS;

	InitGpio();

	InitECap();
	InitECapGpio();

	InitSpi();
	InitSpiGpio();

	InitAdc();  // For this example, init the ADC

	InitCpuTimers();

	InitECan();
	InitECanGpio();

	InitXIntrupt();

	//ConfigCpuTimer(&CpuTimer0, 100, 500);
    ConfigCpuTimer(&CpuTimer1, 100, 50000);
    //CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
    CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

    EnableInterrupts();

	rtd();

	//PIDInit(&mainPID,10,0,0,0.05,0,100,AUTOMATIC,REVERSE);
    //PIDSetpointSet(&mainPID,0);

	while(1)
	{
	    stop_light();
	}
}
//========================================================================

