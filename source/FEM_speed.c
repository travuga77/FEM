/*
 * FEM_speed.c
 *
 *  Created on: 2 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"

float speedf=0.0, speedr=0.0, speedRR=0.0, speedLR=0.0, speedRF=0.0, speedLF=0.0, speed=0.0;

void calc_speed(void) {
    if (ECap1Regs.CAP1>30000000 || ECap1Regs.CAP1==0)
        speedLR=0;
    else {
        speedLR=6937500*3.6/ECap1Regs.CAP1; //1665mm/24=69.4mm/hole=0.069375m
        ECap1Regs.CAP1=0;
    }
    if (ECap2Regs.CAP1>30000000 || ECap2Regs.CAP1==0)
        speedRR=0;
    else {
        speedRR=6937500*3.6/ECap2Regs.CAP1;
        ECap2Regs.CAP1=0;
    }
    if (ECap3Regs.CAP1>30000000 || ECap3Regs.CAP1==0)
        speedRF=0;
    else {
        speedRF=6937500*3.6/ECap3Regs.CAP1;
        ECap3Regs.CAP1=0;
    }
    if (ECap4Regs.CAP1>30000000 || ECap4Regs.CAP1==0)
        speedLF=0;
    else {
        speedLF=6937500*3.6/ECap4Regs.CAP1;
        ECap4Regs.CAP1=0;
    }

    speedf=(speedRF+speedLF)/2;
    speedr=(speedRR+speedLR)/2;
    speed=(speedf+speedr)/2;
}

