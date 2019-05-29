//######################################################################
// MAIN FILE OF FE MADI PROJECT
//######################################################################

#include "DSP28x_Project.h"
#include "pid_controller.h"
#include <math.h>

int PedalOut=0;  //main output from pedal
int send_motors=0, send_left_motor=0, send_right_motor=0;
int send_motors_max=4095;
int count_pri=0;
int mb_temp=0;
int SteerOut=0;  //main output from steering wheel
int i=0,t=0;
int count;
int count_RL;
int countLF=0;
int countRF=0;
int odometer=0;
int x=0, y=0, z=0;
int temp1=0, temp2=0;
int pol;
int SoC;
double C1, C2, x_curr;
double max_alfa_mod=0.0;
double Ul, el;
double voltage_left_motor=0.0, voltage_right_motor=0.0;
double current_left_motor=0.0, current_right_motor=0.0;
double voltage_bms=0.0;
double current_bms=0.0;
double current_acc_cont=0.0;
double current_left_motor_calc;
int temp_left_motor=0, temp_right_motor=0;
extern double speed, speedr, speedf, speedLF, speedRF, speedLR, speedRR;
extern double omegaLR, omegaRR;
extern int flag;
extern int minspeedr, minspeedf;
extern int error_code;

bool racelogic=false;
int racelogic_time=0;
extern int mode;

#ifdef ALFA_ROUTINE
int alfa, max_alfa=MAX_ALFA;
int send_motors_min_1=0;
double phase=PHASE;
double max_curr=MAX_CURR;
double slip=0.0, max_slip=MAX_SLIP;
double curr_phase=CURR_PHASE;
double curr_perc=CURR_PERC;
double perc_per_g=PERC_PER_G;
#endif
#ifdef PID_ROUTINE
double curr_pid_coff=0.0;
PIDControl mainPID;
#endif
/*
interrupt void cpu_timer0_isr(void)
{
    //summ_average();
    // Acknowledge this interrupt to receive more interrupts from group 1
    error_code=ECanbMboxes.MBOX31.MDL.word.LOW_WORD;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
*/
interrupt void ecap3_int_isr(void) {
    countRF++;
    ECap3Regs.ECCLR.bit.INT = 1;
    ECap3Regs.ECCLR.bit.CEVT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

interrupt void ecap4_int_isr(void) {
    countLF++;
    ECap4Regs.ECCLR.bit.INT = 1;
    ECap4Regs.ECCLR.bit.CEVT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

interrupt void main_timer_isr(void) {
    send_CAN_sync_message();

    calc_speed();
    odometer=(countLF+countRF)*0.069375/4;

    if (speedf<0.1) {
        racelogic=true;
        racelogic_time=0;
        countLF=countRF=0;
    } else {
        //if (racelogic and odometer<75)
        if (racelogic and speedf<RACELOGIC_SPEED)
            racelogic_time+=PERIOD/10;
        else {
            racelogic=false;
        }
    }
    count_RL=0;
	calc_PedalOut();

	//if (PedalOut>1200) PedalOut=1200;
	bspd();
	//tang=tan(SteerOut*3.1415/180);
	SteerOut=calc_steer_wheel_spi();

	if (GpioDataRegs.GPADAT.bit.GPIO9==1) GpioDataRegs.GPASET.bit.GPIO5=1; else GpioDataRegs.GPACLEAR.bit.GPIO5=1;

	SoC = ECanbMboxes.MBOX28.MDH.byte.BYTE5;
    voltage_bms = ECanbMboxes.MBOX28.MDH.word.HI_WORD/10;
    current_bms = -(ECanbMboxes.MBOX28.MDL.byte.BYTE1*256+ECanbMboxes.MBOX28.MDL.byte.BYTE2)/10;
    voltage_left_motor = ECanbMboxes.MBOX15.MDL.word.LOW_WORD/10;
    voltage_right_motor = ECanbMboxes.MBOX16.MDL.word.LOW_WORD/10;
    current_left_motor = ECanbMboxes.MBOX15.MDL.word.HI_WORD/10;
    current_right_motor = ECanbMboxes.MBOX16.MDL.word.HI_WORD/10;
    current_acc_cont = 1.5625*ECanbMboxes.MBOX20.MDL.word.LOW_WORD-800;
    temp_left_motor = 150.6*ECanbMboxes.MBOX13.MDL.word.HI_WORD*5/1024-146.1;
    temp_right_motor = 150.6*ECanbMboxes.MBOX13.MDL.word.LOW_WORD*5/1024-146.1;
    x=ECanbMboxes.MBOX20.MDL.word.HI_WORD;
    y=ECanbMboxes.MBOX20.MDH.word.LOW_WORD;
    z=ECanbMboxes.MBOX20.MDH.word.HI_WORD;
    if (current_acc_cont<0) current_acc_cont=0;



    steering_buttons();

    slip = (speedr-speedf)/(speedf+0.001);
    if (slip>10) slip=10;

#ifdef PID_ROUTINE
    // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM

    C1 = MAX_CURR*(1-curr_perc);
    C2 = MAX_CURR*(1+curr_perc);
    x_curr = 3.1415*(current_bms - C1)/(C2-C1);
    if (current_bms<=C1) curr_pid_coff=1;
    if (current_bms>C1 && current_bms<C2) curr_pid_coff=cos(x_curr);
    if (current_bms>=C2) curr_pid_coff=-1;

    PIDInputSet(&mainPID,PedalOut-send_motors);
    PIDCompute(&mainPID);
    if (PedalOut-send_motors>0) {
        PIDTuningKpSet(&mainPID,4*curr_pid_coff*0.2);
        PIDTuningKiSet(&mainPID,4*curr_pid_coff*1);
    }
    else {
        PIDTuningKpSet(&mainPID,100*abs(curr_pid_coff)*0.0);
        PIDTuningKiSet(&mainPID,100*abs(curr_pid_coff)*1);
    }

    send_motors=PIDOutputGet(&mainPID);

    // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM  // PID ALGORITHM
#endif
#ifdef ALFA_ROUTINE
    // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM
    send_motors=PedalOut;

    //send_motors_max=MAX_CURR*0.98*4095/(current_left_motor+current_right_motor+0.1);
    //if (send_motors>send_motors_max) send_motors=send_motors_max;

    max_alfa_mod=max_alfa*PERIOD/20;
    if (slip<max_slip*phase && slip>0) alfa=max_alfa_mod*cos(slip*1.5708/max_slip);
    if (slip>=max_slip*phase) alfa=max_alfa_mod*cos(1.5708*phase);
    if (slip<=0 or max_slip<=0.03) alfa=max_alfa_mod;


    double C1 = max_curr*(1-curr_perc);
    double C2 = max_curr*(1+curr_perc);
    double x_curr = 3.1415*(current_acc_cont - C1)/(C2-C1);
    if (current_acc_cont<=C1) alfa*=1;
    if (current_acc_cont>C1 && current_acc_cont<=max_curr) alfa*=cos(x_curr);
    if (current_acc_cont>max_curr && current_acc_cont<C2) alfa*=2*cos(x_curr);
    if (current_acc_cont>=C2) alfa*=-2;

    //pol=4095*1.2*speedf/98+300;
    //if (pol>4095) pol=4095;

    //if (send_motors_min_1<pol) send_motors_min_1=pol;

    if (send_motors>=send_motors_min_1+alfa) {
        send_motors=send_motors_min_1+alfa;
    }
    send_motors_min_1=send_motors;

    // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM  // ALFA ALGORITHM
#endif

    if (send_motors<0) send_motors=0;
    if (send_motors>4095) send_motors=4095;

    send_left_motor =differential_l(send_motors,SteerOut)*flag;
    send_right_motor=differential_r(send_motors,SteerOut)*flag;
/*
    Ul=(send_left_motor*voltage_left_motor/4095);
    el=0.025*omegaLR*60*4.1;
    current_left_motor_calc=(Ul-el)*1000/16.95/0.75;
*/
    if (fabs(SteerOut)>5)
        send_CAN_motors(send_left_motor,send_right_motor);
    else
        send_CAN_motors(send_motors*flag, send_motors*flag);
    if (count_pri>=100) {
        if (mode==0)    send_CAN_priborka(max_alfa,speedf);
        if (mode==1)    send_CAN_priborka(max_slip*100,slip*100);
        if (mode==2)    send_CAN_priborka(phase*100,PedalOut);
        if (mode==3)    send_CAN_priborka(curr_phase*100,send_motors);
        if (mode==4)    send_CAN_priborka(max_curr,current_acc_cont);
        if (mode==5)    send_CAN_priborka(fabs(perc_per_g)*100,fabs(x));
        count_pri=0;
    }
    else count_pri+=PERIOD;
    //send_CAN_priborka(voltage_bms,speedf);
    send_CAN_steer(SteerOut);
    send_CAN_datalogger(SteerOut,PedalOut,alfa,send_motors,speedf,speedr,slip*100,voltage_bms,current_acc_cont,current_left_motor,current_right_motor,x,y,z,temp_left_motor,temp_right_motor);

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
    PieCtrlRegs.PIEIER4.bit.INTx3 = 1;  // Enable PIE Group 4 INTx3
    PieCtrlRegs.PIEIER4.bit.INTx4 = 1;  // Enable PIE Group 4 INTx4
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  // Enable PIE Group 9 INTx7

    IER |= M_INT4;
    //IER |= M_INT9;
    IER |= M_INT12; // Enable CPU int12
    IER |= M_INT13;
// Interrupts that are used in this example are re-mapped to ISR functions found within this file.
	EALLOW;
	//PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &main_timer_isr;
	PieVectTable.ECAN0INTB = &can_int_isr;
	PieVectTable.ECAP3_INT = &ecap3_int_isr;
	PieVectTable.ECAP4_INT = &ecap4_int_isr;
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
    ConfigCpuTimer(&CpuTimer1, 100, PERIOD*1000);
    //CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
    CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

    EnableInterrupts();

	rtd();

#ifdef PID_ROUTINE
    PIDInit(&mainPID,4*0.2,4*1,0,0.02,0,4095,AUTOMATIC,REVERSE);
    PIDSetpointSet(&mainPID,0);
#endif
	while(1)
	{
	    stop_light();
	    ECap4Regs.ECCLR.bit.INT = 1;
	}
}
//========================================================================

