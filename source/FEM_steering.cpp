/*
 * FEM_steering.c
 *
 *  Created on: 1 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"
int sw_buttons=0;
bool button_pressed_once=true;
int mode=0;
extern int alfa_n;
extern float phase_n, slip_n;

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
    sw_buttons =  ECanbMboxes.MBOX12.MDL.byte.BYTE3;
    if (sw_buttons*button_pressed_once==1) {
        if (mode==0)    alfa_n+=5;
        if (mode==1)    slip_n+=0.01;
        if (mode==2)    phase_n+=0.01;
        button_pressed_once=false;
    }

    if (sw_buttons*button_pressed_once==8) {
        mode+=1;
        if (mode>2) mode=0;
        button_pressed_once=false;
    }

    if (sw_buttons*button_pressed_once==4) {
        if (mode==0)    alfa_n-=5;
        if (mode==1)    slip_n-=0.01;
        if (mode==2)    phase_n-=0.01;
        button_pressed_once=false;
    }

    if (sw_buttons==0) {
        button_pressed_once=true;
    }
}
