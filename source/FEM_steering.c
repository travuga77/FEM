/*
 * FEM_steering.c
 *
 *  Created on: 1 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"
#include <math.h>

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
