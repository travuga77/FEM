/*
 * FEM_pedal.c
 *
 *  Created on: 1 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"

int PedalOutmin1=0;
int n=0;
int minspeedr=32000.0, minspeedf=32000.0;
int can_pedal_rx_flag=0;
extern int PedalOut;
extern float speedf, speedr;
extern int alfa;

int pol=0;

void correct_alfa(void) {
    if (speedr<minspeedr) minspeedr=speedr;
    if (speedf<minspeedf) minspeedf=speedf;
    if (n>=3) {
        if (minspeedr-minspeedf>2)
            alfa=-alfa_n;
        else
            alfa=alfa_n;
        n=0;
        minspeedr=minspeedf=32000;
    }
    else n++;
}

interrupt void can_int_isr(void)
{
    can_pedal_rx_flag=1;
// Acknowledge this interrupt to get more from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

void calc_PedalOut (void){

    volatile struct ECAN_REGS ECanbShadow;

    can_pedal_rx_flag=ECanbRegs.CANRMP.bit.RMP30;
    PedalOut=ECanbMboxes.MBOX30.MDL.word.LOW_WORD*can_pedal_rx_flag+300;
    ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
    ECanbShadow.CANRMP.bit.RMP30=1;
    ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;
    ECanbMboxes.MBOX30.MDL.word.LOW_WORD=0;
    if (PedalOut<0) PedalOut=0;
    if (PedalOut>4095) PedalOut=4095;

    //correct_alfa();
    pol=4095*1.3*speedf/98+300;
    if (pol>4095) pol=4095;
    if (PedalOut>=PedalOutmin1+alfa) {
        PedalOut=PedalOutmin1+alfa;
    }
    PedalOutmin1=PedalOut;
    if (PedalOutmin1<pol) PedalOutmin1=pol;
}
