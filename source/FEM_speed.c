/*
 * FEM_speed.c
 *
 *  Created on: 2 мар. 2018 г.
 *      Author: formu
 */


#include "DSP28x_Project.h"

unsigned long count_wheel=0;
double speedf=0.0, speedr=0.0, speedRR=0.0, speedRL=0.0, speedFR=0.0, speedFL=0.0, speed=0.0;
double omegaRL=0.0, omegaRR=0.0;

void calc_speed(void) {
    // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG // FLAG BUG

    if (ECap1Regs.TSCTR>40000000 || ECap1Regs.ECFLG.bit.CTROVF==1)
        speedRL=0.0;
    else {
        speedRL=100000000*0.069375*3.6/ECap1Regs.CAP1; //1665mm/24=69.4mm/hole=0.069375m
        omegaRL=100000000.0/(24*ECap1Regs.CAP1);
    }
    count_wheel=ECap2Regs.TSCTR;
    if (ECap2Regs.TSCTR>40000000 || ECap2Regs.ECFLG.bit.CTROVF==1)
        speedRR=0.0;
    else {
        speedRR=6937500*3.6/ECap2Regs.CAP1;
    }
    if (ECap3Regs.TSCTR>40000000 || ECap3Regs.ECFLG.bit.CTROVF==1)
        speedFR=0.0;
    else {
        speedFR=6937500*3.6/ECap3Regs.CAP1;
    }
    if (ECap4Regs.TSCTR>40000000 || ECap4Regs.ECFLG.bit.CTROVF==1)
        speedFL=0.0;
    else {
        speedFL=6937500*3.6/ECap4Regs.CAP1;
    }

    speedf=(speedFL+speedFR)/2;
    speedr=(speedRL+speedRR)/2;
    speed=(speedf+speedr)/2;
}

