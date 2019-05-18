/*
 * FEM_steering.c
 *
 *  Created on: 1 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"

bool button_pressed_once=true;
int mode=0;
extern int max_alfa;
extern float phase, max_slip, curr_phase, curr_perc, perc_per_g;

void spi_xmit(Uint16 a)
{
    SpiaRegs.SPITXBUF=a;
}

int calc_steer_wheel_spi(void) {
    spi_xmit(0xff);
    while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { }
    int St = SpiaRegs.SPIRXBUF;
    St+=16384;
    St/=520;
    return St;
}

void steering_buttons(void) {
    int sw_buttons =  ECanbMboxes.MBOX12.MDL.byte.BYTE3;
    if (sw_buttons*button_pressed_once==1) {
        if (mode==0)    max_alfa+=5;
        if (mode==1)    max_slip+=0.01;
        if (mode==2)    phase+=0.01;
        if (mode==3)    curr_phase+=0.02;
        if (mode==4)    curr_perc+=0.05;
        if (mode==5)    perc_per_g+=0.01;
        button_pressed_once=false;
    }

    if (sw_buttons*button_pressed_once==8) {
        mode+=1;
        if (mode>5) mode=0;
        button_pressed_once=false;
    }

    if (sw_buttons*button_pressed_once==4) {
        if (mode==0)    max_alfa-=5;
        if (mode==1)    max_slip-=0.01;
        if (mode==2)    phase-=0.01;
        if (mode==3)    curr_phase-=0.02;
        if (mode==4)    curr_perc-=0.05;
        if (mode==5)    perc_per_g-=0.01;
        button_pressed_once=false;
    }

    if (sw_buttons==0) {
        button_pressed_once=true;
    }
}
