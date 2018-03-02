//###########################################################################
//
// FILE:	DSP2833x_PieCtrl.c
//
// TITLE:	DSP2833x Device PIE Control Register Initialization Functions.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V140 $
// $Release Date: March  4, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitPieCtrl: 
//---------------------------------------------------------------------------
// This function initializes the PIE control registers to a known state.
//
void InitPieCtrl(void)
{
    // Disable Interrupts at the CPU level:
    DINT;

    // Disable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;

	// Clear all PIEIER registers:
	PieCtrlRegs.PIEIER1.all = 0;
	PieCtrlRegs.PIEIER2.all = 0;
	PieCtrlRegs.PIEIER3.all = 0;	
	PieCtrlRegs.PIEIER4.all = 0;
	PieCtrlRegs.PIEIER5.all = 0;
	PieCtrlRegs.PIEIER6.all = 0;
	PieCtrlRegs.PIEIER7.all = 0;
	PieCtrlRegs.PIEIER8.all = 0;
	PieCtrlRegs.PIEIER9.all = 0;
	PieCtrlRegs.PIEIER10.all = 0;
	PieCtrlRegs.PIEIER11.all = 0;
	PieCtrlRegs.PIEIER12.all = 0;

	// Clear all PIEIFR registers:
	PieCtrlRegs.PIEIFR1.all = 0;
	PieCtrlRegs.PIEIFR2.all = 0;
	PieCtrlRegs.PIEIFR3.all = 0;	
	PieCtrlRegs.PIEIFR4.all = 0;
	PieCtrlRegs.PIEIFR5.all = 0;
	PieCtrlRegs.PIEIFR6.all = 0;
	PieCtrlRegs.PIEIFR7.all = 0;
	PieCtrlRegs.PIEIFR8.all = 0;
	PieCtrlRegs.PIEIFR9.all = 0;
	PieCtrlRegs.PIEIFR10.all = 0;
	PieCtrlRegs.PIEIFR11.all = 0;
	PieCtrlRegs.PIEIFR12.all = 0;


// Enable Xint3 in the PIE: Group 12 interrupt 1
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  // Enable PIE Group 9 INTx7

    IER |= M_INT12; // Enable CPU int12
    IER |= M_INT13;

}	

//---------------------------------------------------------------------------
// EnableInterrupts: 
//---------------------------------------------------------------------------
// This function enables the PIE module and CPU interrupts
//
void EnableInterrupts()
{

    // Enable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    		
	// Enables PIE to drive a pulse into the CPU 
	PieCtrlRegs.PIEACK.all = 0xFFFF;  

	// Enable Interrupts at the CPU level 
    EINT;

}


void InitXIntrupt(void) {

    EALLOW;
    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 0;        //xint3 sync to sysclockout
    GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 0;        //xint4 sync to sysclockout
    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 0;        //xint5 sync to sysclockout
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0;        //xint6 sync to sysclockout
    GpioCtrlRegs.GPBQSEL1.bit.GPIO39 = 0;        //xint7 sync to sysclockout
    GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;    //Each sampling window is 510*SYSCLKOUT
    GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 27;   //gpio59 as input for XINT1 LF
    GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 30;   //gpio62 as input for XINT2 RF
    GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 26;   //gpio58 as input for XINT3 LR
    GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 28;   //gpio60 as input for XINT4 RR
    GpioIntRegs.GPIOXINT7SEL.bit.GPIOSEL = 7;    //gpio39 as input for XINT5 button
    EDIS;
    XIntruptRegs.XINT3CR.bit.ENABLE = 1;          //turn on XINT3
    XIntruptRegs.XINT4CR.bit.ENABLE = 1;          //turn on XINT4
    XIntruptRegs.XINT5CR.bit.ENABLE = 1;          //turn on XINT5
    XIntruptRegs.XINT6CR.bit.ENABLE = 1;          //turn on XINT6
    XIntruptRegs.XINT7CR.bit.ENABLE = 1;          //turn on XINT7
    XIntruptRegs.XINT3CR.bit.POLARITY = 0x1;      //rising edge
    XIntruptRegs.XINT4CR.bit.POLARITY = 0x1;      //rising edge
    XIntruptRegs.XINT5CR.bit.POLARITY = 0x1;      //rising edge
    XIntruptRegs.XINT6CR.bit.POLARITY = 0x1;      //rising edge
    XIntruptRegs.XINT7CR.bit.POLARITY = 0x0;      //rising edge

}

//===========================================================================
// End of file.
//===========================================================================
