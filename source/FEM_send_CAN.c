/*
 * FEM_send_CAN.c
 *
 *  Created on: 2 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"
#include <math.h>

void send_CAN_steer (int c) {

    volatile struct ECAN_REGS ECanbShadow;

    ECanbMboxes.MBOX24.MDL.word.LOW_WORD=c;   //TEST AREA

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS24 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA24 == 0);              // Wait for TA27 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA24 = 1;                     // Clear TA27
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
}

void send_CAN_priborka (int left, int right) {

    volatile struct ECAN_REGS ECanbShadow;

    ECanbMboxes.MBOX27.MDL.word.LOW_WORD=right;   //TEST AREA
    ECanbMboxes.MBOX27.MDL.word.HI_WORD=left;   //TEST AREA

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS27 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA27 == 0);              // Wait for TA27 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA27 = 1;                     // Clear TA27
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
}

void send_CAN_datalogger (int tr0, int tr1, int tr2, int tr3) {

    volatile struct ECAN_REGS ECanbShadow;

    ECanbMboxes.MBOX10.MDL.word.LOW_WORD=tr0;   //TEST AREA
    ECanbMboxes.MBOX10.MDL.word.HI_WORD=tr1;   //TEST AREA
    ECanbMboxes.MBOX10.MDH.word.LOW_WORD=tr2;   //TEST AREA
    ECanbMboxes.MBOX10.MDH.word.HI_WORD=tr3;   //TEST AREA

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS10 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA10 == 0);              // Wait for TA10 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA10 = 1;                     // Clear TA10
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
}

void send_CAN_motors (int a, int b) {

    volatile struct ECAN_REGS ECanbShadow;

    ECanbMboxes.MBOX25.MDL.word.LOW_WORD=a;   //TEST AREA
    ECanbMboxes.MBOX26.MDL.word.LOW_WORD=b;   //TEST AREA

    ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
/*        do
        {
            ECanbShadow. CANRMP.all = ECanbRegs. CANRMP.all;
        } while(ECanbShadow. CANRMP.bit.RMP28 != 1 );                       // Wait for RMP1 bit to be set..
        ECanbShadow.CANRMP.bit.RMP28 = 1;
        ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;
*/
/*  ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
        do
        {
            ECanbShadow. CANRMP.all = ECanbRegs. CANRMP.all;
        } while(ECanbShadow. CANRMP.bit.RMP29 != 1 );                       // Wait for RMP1 bit to be set..
        ECanbShadow.CANRMP.bit.RMP29 = 1;
        ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;
*/

    GpioDataRegs.GPATOGGLE.bit.GPIO3=1;

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS25 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA25 == 0);              // Wait for TA25 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA25 = 1;                     // Clear TA25
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS26 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA26 == 0);              // Wait for TA26 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA26 = 1;                     // Clear TA26
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
}


