//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V141 $
// $Release Date: November  6, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define ADC_usDELAY  5000L

//---------------------------------------------------------------------------
// InitAdc:
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);

    // *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.

	    EALLOW;
		SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
		ADC_cal();
		EDIS;

    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
	// after power up to give all analog circuits time to power up and settle

    // Please note that for the delay function below to operate correctly the
	// CPU_RATE define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.

    AdcRegs.ADCTRL3.all = 0x00E0;  // Power up bandgap/reference/ADC circuits
    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels

    AdcRegs.ADCMAXCONV.all = 0x0004;       // Setup 5 conv's on SEQ1
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x5; // Setup ADCINA5 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x7; // Setup ADCINA7 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x3; // Setup ADCINA3 as 3nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x8; // Setup ADCINB0 as 4th SEQ1 conv.
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0xA; // Setup ADCINB2 as 5th SEQ1 conv.
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0;       // Continious run
    AdcRegs.ADCTRL3.bit.ADCCLKPS = 0xf;      // Long conversion windows
    AdcRegs.ADCTRL1.bit.ACQ_PS = 0xF;
    AdcRegs.ADCTRL1.bit.CPS = 1;
}

void read_ADC(void) {
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
    while (AdcRegs.ADCST.bit.INT_SEQ1!=1) {}
    AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
}

//===========================================================================
// End of file.
//===========================================================================
