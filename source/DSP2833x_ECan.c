//###########################################################################
//
// FILE:	DSP2833x_ECan.c
//
// TITLE:	DSP2833x Enhanced CAN Initialization & Support Functions.
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
// InitECan:
//---------------------------------------------------------------------------
// This function initializes the eCAN module to a known state.
//
void InitECan(void)
{
#if DSP28_ECANA
   InitECana();
#endif // if DSP28_ECANA
#if DSP28_ECANB
   InitECanb();
#endif // if DSP28_ECANB
}

void InitECana(void)		// Initialize eCAN-A module
{
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. This is especially true while writing to/reading from a bit
 (or group of bits) among bits 16 - 31 */

struct ECAN_REGS ECanaShadow;

	EALLOW;		// EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/

    ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
    ECanaShadow.CANTIOC.bit.TXFUNC = 1;
    ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

    ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
    ECanaShadow.CANRIOC.bit.RXFUNC = 1;
    ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
									// HECC mode also enables time-stamping feature

	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.SCB = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero

    ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.

	ECanaRegs.CANTA.all	= 0xFFFFFFFF;	/* Clear all TAn bits */

	ECanaRegs.CANRMP.all = 0xFFFFFFFF;	/* Clear all RMPn bits */

	ECanaRegs.CANGIF0.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits */
	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;


/* Configure bit timing parameters for eCANA*/
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
	{
	    ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 1 );  		// Wait for CCE bit to be set..

    ECanaShadow.CANBTC.all = 0;

    #if (CPU_FRQ_150MHZ)                       // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
		/* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default. Bit rate = 1 Mbps
		   See Note at End of File */
			ECanaShadow.CANBTC.bit.BRPREG = 4;
			ECanaShadow.CANBTC.bit.TSEG2REG = 2;
			ECanaShadow.CANBTC.bit.TSEG1REG = 10;
    #endif
	#if (CPU_FRQ_100MHZ)                       // CPU_FRQ_100MHz is defined in DSP2833x_Examples.h
	/* The following block is only for 100 MHz SYSCLKOUT (50 MHz CAN clock). Bit rate = 1 Mbps
	   See Note at End of File */
	    ECanaShadow.CANBTC.bit.BRPREG = 9;
		ECanaShadow.CANBTC.bit.TSEG2REG = 1;
		ECanaShadow.CANBTC.bit.TSEG1REG = 6;
	#endif


    ECanaShadow.CANBTC.bit.SAM = 1;
    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
    {
       ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..

/* Disable all Mailboxes  */
 	ECanaRegs.CANME.all = 0;		// Required before writing the MSGIDs

    EDIS;
}


#if (DSP28_ECANB)
void InitECanb(void)		// Initialize eCAN-B module
{
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. This is especially true while writing to/reading from a bit
 (or group of bits) among bits 16 - 31 */

    struct ECAN_REGS ECanbShadow;

   EALLOW;		// EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/

    ECanbShadow.CANTIOC.all = ECanbRegs.CANTIOC.all;
    ECanbShadow.CANTIOC.bit.TXFUNC = 1;
    ECanbRegs.CANTIOC.all = ECanbShadow.CANTIOC.all;

    ECanbShadow.CANRIOC.all = ECanbRegs.CANRIOC.all;
    ECanbShadow.CANRIOC.bit.RXFUNC = 1;
    ECanbRegs.CANRIOC.all = ECanbShadow.CANRIOC.all;

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */

	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.SCB = 1;
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero

    ECanbMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX1.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.

	ECanbRegs.CANTA.all	= 0xFFFFFFFF;	/* Clear all TAn bits */

	ECanbRegs.CANRMP.all = 0xFFFFFFFF;	/* Clear all RMPn bits */

	ECanbRegs.CANGIF0.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits */
	ECanbRegs.CANGIF1.all = 0xFFFFFFFF;


/* Configure bit timing parameters for eCANB*/

	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.DBO = 1;
	ECanbShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
	{
	    ECanbShadow.CANES.all = ECanbRegs.CANES.all;
	} while(ECanbShadow.CANES.bit.CCE != 1 ); 		// Wait for CCE bit to be  cleared..


    ECanbShadow.CANBTC.all = 0;

    #if (CPU_FRQ_150MHZ)                       // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
	/* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default. Bit rate = 1 Mbps
	   See Note at end of file */
		ECanbShadow.CANBTC.bit.BRPREG = 4;
		ECanbShadow.CANBTC.bit.TSEG2REG = 2;
		ECanbShadow.CANBTC.bit.TSEG1REG = 10;
	#endif
	#if (CPU_FRQ_100MHZ)                       // CPU_FRQ_100MHz is defined in DSP2833x_Examples.h
	/* The following block is only for 100 MHz SYSCLKOUT (50 MHz CAN clock). Bit rate = 1 Mbps
	   See Note at end of file */
        ECanbShadow.CANBTC.bit.BRPREG = 9;
        ECanbShadow.CANBTC.bit.TSEG2REG = 1;
        ECanbShadow.CANBTC.bit.TSEG1REG = 6;
	#endif

    ECanbShadow.CANBTC.bit.SAM = 1;
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
    {
        ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..


/* Disable all Mailboxes  */
 	ECanbRegs.CANME.all = 0;		// Required before writing the MSGIDs

    EDIS;

/* Write to the MSGID field  */

    ECanbMboxes.MBOX1.MSGID.all = 0;
    ECanbMboxes.MBOX8.MSGID.all = 0;
    ECanbMboxes.MBOX9.MSGID.all = 0;
    ECanbMboxes.MBOX10.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX11.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX12.MSGID.all = 0;
    ECanbMboxes.MBOX13.MSGID.all = 0;
    ECanbMboxes.MBOX15.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX16.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX20.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX24.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX25.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX26.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX27.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX28.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX29.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX30.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX31.MSGID.all = 0; // Standart Identifier

    ECanbMboxes.MBOX8.MSGID.bit.STDMSGID = 0x134; // Standart Identifier datalogger
    ECanbMboxes.MBOX9.MSGID.bit.STDMSGID = 0x135; // Standart Identifier datalogger
    ECanbMboxes.MBOX10.MSGID.bit.STDMSGID = 0x136; // Standart Identifier datalogger
    ECanbMboxes.MBOX11.MSGID.bit.STDMSGID = 0x137; // Standart Identifier datalogger
    ECanbMboxes.MBOX24.MSGID.bit.STDMSGID = 0x400; // Standart Identifier steering
    ECanbMboxes.MBOX25.MSGID.bit.STDMSGID = 0x222; // Standart Identifier motor left
    ECanbMboxes.MBOX26.MSGID.bit.STDMSGID = 0x221; // Standart Identifier motor right
    ECanbMboxes.MBOX27.MSGID.bit.STDMSGID = 0x120; // Standart Identifier dashboard
    ECanbMboxes.MBOX1.MSGID.bit.STDMSGID = 0x80;  // sync mes


    ECanbMboxes.MBOX28.MSGID.bit.AME=1;
    ECanbLAMRegs.LAM12.all=0x80000000;
    ECanbLAMRegs.LAM13.all=0x80000000;
    ECanbLAMRegs.LAM15.all=0x80000000;
    ECanbLAMRegs.LAM16.all=0x80000000;
    ECanbLAMRegs.LAM20.all=0x80000000;
    ECanbLAMRegs.LAM28.all=0x80000000;
    ECanbLAMRegs.LAM29.all=0x80000000;
    ECanbLAMRegs.LAM30.all=0x80000000;
    ECanbLAMRegs.LAM31.all=0x80000000;

    ECanbMboxes.MBOX12.MSGID.bit.STDMSGID = 0x330; //from priborka
    ECanbMboxes.MBOX13.MSGID.bit.STDMSGID = 0x240; //motor temperatures
    ECanbMboxes.MBOX15.MSGID.bit.STDMSGID = 0x1A2; // left motor info
    ECanbMboxes.MBOX16.MSGID.bit.STDMSGID = 0x1A1; // right motor info
    ECanbMboxes.MBOX20.MSGID.bit.STDMSGID = 0x230; // acc container current
    ECanbMboxes.MBOX28.MSGID.bit.STDMSGID = 0x1A0; // bms voltage and temp
    ECanbMboxes.MBOX29.MSGID.bit.STDMSGID = 0x4A0; // Standart Identifier
    ECanbMboxes.MBOX30.MSGID.bit.STDMSGID = 0x150; // Standart Identifier
    ECanbMboxes.MBOX31.MSGID.bit.STDMSGID = 0x151; // Standart Identifier

/* Configure Mailbox under test as a Transmit mailbox */

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;
    ECanbShadow.CANMD.bit.MD1 = 0;
    ECanbShadow.CANMD.bit.MD8 = 0;
    ECanbShadow.CANMD.bit.MD9 = 0;
    ECanbShadow.CANMD.bit.MD10 = 0;
    ECanbShadow.CANMD.bit.MD11 = 0;
    ECanbShadow.CANMD.bit.MD24 = 0;
    ECanbShadow.CANMD.bit.MD25 = 0;
    ECanbShadow.CANMD.bit.MD26 = 0;
    ECanbShadow.CANMD.bit.MD27 = 0;
    ECanbShadow.CANMD.bit.MD12 = 1;
    ECanbShadow.CANMD.bit.MD13 = 1;
    ECanbShadow.CANMD.bit.MD15 = 1;
    ECanbShadow.CANMD.bit.MD16 = 1;
    ECanbShadow.CANMD.bit.MD20 = 1;
    ECanbShadow.CANMD.bit.MD28 = 1;
    ECanbShadow.CANMD.bit.MD29 = 1;
    ECanbShadow.CANMD.bit.MD30 = 1;
    ECanbShadow.CANMD.bit.MD31 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

/* Enable Mailbox under test */

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;
    ECanbShadow.CANME.bit.ME1 = 1;
    ECanbShadow.CANME.bit.ME8 = 1;
    ECanbShadow.CANME.bit.ME9 = 1;
    ECanbShadow.CANME.bit.ME10 = 1;
    ECanbShadow.CANME.bit.ME11 = 1;
    ECanbShadow.CANME.bit.ME24 = 1;
    ECanbShadow.CANME.bit.ME25 = 1;
    ECanbShadow.CANME.bit.ME26 = 1;
    ECanbShadow.CANME.bit.ME27 = 1;
    ECanbShadow.CANME.bit.ME12 = 1;
    ECanbShadow.CANME.bit.ME13 = 1;
    ECanbShadow.CANME.bit.ME15 = 1;
    ECanbShadow.CANME.bit.ME16 = 1;
    ECanbShadow.CANME.bit.ME20 = 1;
    ECanbShadow.CANME.bit.ME28 = 1;
    ECanbShadow.CANME.bit.ME29 = 1;
    ECanbShadow.CANME.bit.ME30 = 1;
    ECanbShadow.CANME.bit.ME31 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    EALLOW;
    ECanbShadow.CANMIM.all = ECanbRegs.CANMIM.all;
    ECanbShadow.CANMIM.bit.MIM30=1;
    ECanbRegs.CANMIM.all = ECanbShadow.CANMIM.all;

    ECanbShadow.CANGIM.all = ECanbRegs.CANGIM.all;
    ECanbShadow.CANGIM.bit.I0EN=1;
    ECanbRegs.CANGIM.all = ECanbShadow.CANGIM.all;
    EDIS;

/* Write to DLC field in Master Control reg */

    ECanbMboxes.MBOX1.MSGCTRL.bit.DLC = 1;
    ECanbMboxes.MBOX8.MSGCTRL.bit.DLC = 8;
    ECanbMboxes.MBOX9.MSGCTRL.bit.DLC = 8;
    ECanbMboxes.MBOX10.MSGCTRL.bit.DLC = 8; //transmit only
    ECanbMboxes.MBOX11.MSGCTRL.bit.DLC = 8;
    ECanbMboxes.MBOX24.MSGCTRL.bit.DLC = 1;
    ECanbMboxes.MBOX25.MSGCTRL.bit.DLC = 2;
    ECanbMboxes.MBOX26.MSGCTRL.bit.DLC = 2;
    ECanbMboxes.MBOX27.MSGCTRL.bit.DLC = 4;

/* Write to the mailbox RAM field */

    ECanbMboxes.MBOX1.MDL.all = 0x0;
    ECanbMboxes.MBOX8.MDL.all = 0x0;
    ECanbMboxes.MBOX9.MDL.all = 0x0;
    ECanbMboxes.MBOX10.MDL.all = 0x0;
    ECanbMboxes.MBOX11.MDL.all = 0x0;
    ECanbMboxes.MBOX24.MDL.all = 0x0;
    ECanbMboxes.MBOX25.MDL.all = 0x0;
    ECanbMboxes.MBOX26.MDL.all = 0x0;
    ECanbMboxes.MBOX27.MDL.all = 0x0;

}

#endif // if DSP28_ECANB


//---------------------------------------------------------------------------
// Example: InitECanGpio:
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as eCAN pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.
//
// Caution:
// Only one GPIO pin should be enabled for CANTXA/B operation.
// Only one GPIO pin shoudl be enabled for CANRXA/B operation.
// Comment out other unwanted lines.


void InitECanGpio(void)
{
#if (DSP28_ECANA)
   InitECanaGpio();
#endif // if DSP28_ECANA
#if (DSP28_ECANB)
   InitECanbGpio();
#endif // if DSP28_ECANB
}

void InitECanaGpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected CAN pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;	    // Enable pull-up for GPIO30 (CANRXA)
//	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	    // Enable pull-up for GPIO18 (CANRXA)

	GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;	    // Enable pull-up for GPIO31 (CANTXA)
//	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	    // Enable pull-up for GPIO19 (CANTXA)

/* Set qualification for selected CAN pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.

    GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 3;   // Asynch qual for GPIO30 (CANRXA)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;   // Asynch qual for GPIO18 (CANRXA)


/* Configure eCAN-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be eCAN functional pins.

	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 1;	// Configure GPIO30 for CANRXA operation
//  GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;	// Configure GPIO18 for CANRXA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 1;	// Configure GPIO31 for CANTXA operation
//  GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;	// Configure GPIO19 for CANTXA operation

    EDIS;
}

#if (DSP28_ECANB)
void InitECanbGpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected CAN pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

//	GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;	  // Enable pull-up for GPIO8  (CANTXB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;   // Enable pull-up for GPIO12 (CANTXB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up for GPIO16 (CANTXB)
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;   // Enable pull-up for GPIO20 (CANTXB)

//	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0;	  // Enable pull-up for GPIO10 (CANRXB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;   // Enable pull-up for GPIO13 (CANRXB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up for GPIO17 (CANRXB)
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;   // Enable pull-up for GPIO21 (CANRXB)

/* Set qualification for selected CAN pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

//  GpioCtrlRegs.GPAQSEL1.bit.GPIO10 = 3; // Asynch qual for GPIO10 (CANRXB)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3; // Asynch qual for GPIO13 (CANRXB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch qual for GPIO17 (CANRXB)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 3; // Asynch qual for GPIO21 (CANRXB)

/* Configure eCAN-B pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be eCAN functional pins.

//	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 2;   // Configure GPIO8 for CANTXB operation
//  GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;  // Configure GPIO12 for CANTXB operation
//  GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 2;  // Configure GPIO16 for CANTXB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 3;  // Configure GPIO20 for CANTXB operation

//	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 2;  // Configure GPIO10 for CANRXB operation
//  GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 2;  // Configure GPIO13 for CANRXB operation
//  GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 2;  // Configure GPIO17 for CANRXB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 3;  // Configure GPIO21 for CANRXB operation

    EDIS;
}

#endif // if DSP28_ECANB



/*
Note: Bit timing parameters must be chosen based on the network parameters such
as the sampling point desired and the propagation delay of the network.
The propagation delay is a function of length of the cable, delay introduced by
the transceivers and opto/galvanic-isolators (if any).

The parameters used in this file must be changed taking into account the above
mentioned factors in order to arrive at the bit-timing parameters suitable
for a network.

*/
