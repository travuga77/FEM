/*
 * FEM_safety.c
 *
 *  Created on: 2 ���. 2018 �.
 *      Author: formu
 */

#include "DSP28x_Project.h"

int err=0;
int flag=1;
int error_code=800;
extern int PedalOut;

void bspd(void)
{
    if (PedalOut>600 && !GpioDataRegs.GPBDAT.bit.BRAKE_PEDAL_PIN==1)
        flag=0;
    if (PedalOut<600 && !GpioDataRegs.GPBDAT.bit.BRAKE_PEDAL_PIN==0)
        flag=1;
}

void rtds(void)
{
    GpioDataRegs.GPBSET.bit.RTDS_PIN=1;
    DELAY_MS(1500);
    GpioDataRegs.GPBCLEAR.bit.RTDS_PIN=1;
    GpioDataRegs.GPACLEAR.bit.GPIO3=1;
    GpioDataRegs.GPACLEAR.bit.GPIO4=1;
}


void rtd (void)
{
    DINT;
    int r=0;
    while (r!=1000)
    {
        if (GpioDataRegs.GPBDAT.bit.START_BUTTON_PIN==1) GpioDataRegs.GPASET.bit.GPIO4=1; else GpioDataRegs.GPACLEAR.bit.GPIO4=1;
        if (!GpioDataRegs.GPBDAT.bit.BRAKE_PEDAL_PIN==1)
        {
            GpioDataRegs.GPASET.bit.GPIO3=1;
            GpioDataRegs.GPBSET.bit.BRAKE_LIGHT_PIN=1;
        }
        else
        {
            GpioDataRegs.GPACLEAR.bit.GPIO3=1;
            GpioDataRegs.GPBCLEAR.bit.BRAKE_LIGHT_PIN=1;
        }

    #ifdef FLASH
        if (GpioDataRegs.GPBDAT.bit.START_BUTTON_PIN*!GpioDataRegs.GPBDAT.bit.BRAKE_PEDAL_PIN*GpioDataRegs.GPCDAT.bit.SHUTDOWN_CIRCUIT_PIN==1)
    #endif
    #ifdef RAM
        if (GpioDataRegs.GPBDAT.bit.START_BUTTON_PIN)
    #endif
        {
            r++;
        }
    }
    rtds ();
    EINT;
}


void turnoffmf (void)
{
    PedalOut=0;
    send_CAN_motors(0,0);
    rtd();
}

void shutdown_detect(void) {

    if (GpioDataRegs.GPCDAT.bit.SHUTDOWN_CIRCUIT_PIN == 0) {
        err++;
        if (err>=50) {
            err=0;
            error_code=801;
            turnoffmf();
        }
    }
        else {
            err=0;
        }
}

void stop_light(void) {
    if (!GpioDataRegs.GPBDAT.bit.BRAKE_PEDAL_PIN==1) {
        GpioDataRegs.GPBSET.bit.BRAKE_LIGHT_PIN=1;
    }
    else {
        GpioDataRegs.GPBCLEAR.bit.BRAKE_LIGHT_PIN=1;
    }
}
