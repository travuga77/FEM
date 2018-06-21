/*
 * FEM_send_CAN.c
 *
 *  Created on: 2 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"

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

void send_CAN_datalogger (int tr15, int tr14, int tr13, int tr12, int tr11, int tr10, int tr9, int tr8, int tr7, int tr6, int tr5, int tr4, int tr3, int tr2, int tr1, int tr0) {

    volatile struct ECAN_REGS ECanbShadow;

    ECanbMboxes.MBOX8.MDL.word.LOW_WORD=tr0;   //TEST AREA
    ECanbMboxes.MBOX8.MDL.word.HI_WORD=tr1;   //TEST AREA
    ECanbMboxes.MBOX8.MDH.word.LOW_WORD=tr2;   //TEST AREA
    ECanbMboxes.MBOX8.MDH.word.HI_WORD=tr3;   //TEST AREA
    ECanbMboxes.MBOX9.MDL.word.LOW_WORD=tr4;   //TEST AREA
    ECanbMboxes.MBOX9.MDL.word.HI_WORD=tr5;   //TEST AREA
    ECanbMboxes.MBOX9.MDH.word.LOW_WORD=tr6;   //TEST AREA
    ECanbMboxes.MBOX9.MDH.word.HI_WORD=tr7;   //TEST AREA
    ECanbMboxes.MBOX10.MDL.word.LOW_WORD=tr8;   //TEST AREA
    ECanbMboxes.MBOX10.MDL.word.HI_WORD=tr9;   //TEST AREA
    ECanbMboxes.MBOX10.MDH.word.LOW_WORD=tr10;   //TEST AREA
    ECanbMboxes.MBOX10.MDH.word.HI_WORD=tr11;   //TEST AREA
    ECanbMboxes.MBOX11.MDL.word.LOW_WORD=tr12;   //TEST AREA
    ECanbMboxes.MBOX11.MDL.word.HI_WORD=tr13;   //TEST AREA
    ECanbMboxes.MBOX11.MDH.word.LOW_WORD=tr14;   //TEST AREA
    ECanbMboxes.MBOX11.MDH.word.HI_WORD=tr15;   //TEST AREA

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS8 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA8 == 0);              // Wait for TA8 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA8 = 1;                     // Clear TA8
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS9 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA9 == 0);              // Wait for TA9 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA9 = 1;                     // Clear TA9
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

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

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS11 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA11 == 0);              // Wait for TA11 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA11 = 1;                     // Clear TA11
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

void send_CAN_sync_message(void){

    volatile struct ECAN_REGS ECanbShadow;

    ECanbShadow.CANTRS.all = 0;
        ECanbShadow.CANTRS.bit.TRS1 = 1;
        ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    do
    {
        ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
    } while (ECanbShadow.CANTA.bit.TA1 == 0);              // Wait for TA11 bit to be set...
    ECanbShadow.CANTA.all = 0;
        ECanbShadow.CANTA.bit.TA1 = 1;                     // Clear TA11
        ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

}
