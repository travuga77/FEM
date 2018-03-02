/*
 * FEM_differential.c
 *
 *  Created on: 2 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"
#include <math.h>

#ifdef no_diff
int differential_r(int K, int sigma)
{
    int Kr;
    Kr=K;
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}
int differential_l(int K, int sigma)
{
    int Kl;
    Kl=K;
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif

#ifdef grach_diff_rules
int differential_r(int K, int sigma)
{
    int Kr;
    if (sigma>0) Kr=K*(1-(dk*tan(sigma*3.1415/180)/(Lk))); else Kr=K;
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}

int differential_l(int K, int sigma)
{
    int Kl;
    if (sigma<0) Kl=K*(1+(dk*tan(sigma*3.1415/180)/(Lk))); else Kl=K;
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif

#ifdef grach_diff
int differential_r(int K, int sigma)
{
    int Kr;
    Kr=K*(1-(dk*tan(sigma*3.1415/180)/(2*Lk)));
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}

int differential_l(int K, int sigma)
{
    int Kl;
    Kl=K*(1+(dk*tan(sigma*3.1415/180)/(2*Lk)));
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif
#ifdef phillips_diff
int differential_r(int K, int sigma)
{
    int Kr; float x;
    if (sigma>3)
    {
        x=Lk*tan((90-sigma)*3.1415/180);
        Kr=K*(x-dk/2)/x;
    }
    else
    {
        if (sigma<-3)
        {
            x=Lk*tan((90+sigma)*3.1415/180);
            Kr=K*(x+dk/2)/x;
        }
        else Kr=K;
    }
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}

int differential_l(int K, int sigma)
{
    int Kl; float x;
    if (sigma>3)
    {
        x=Lk*tan((90-sigma)*3.1415/180);
        Kl=K*(x+dk/2)/x;
    }
    else
    {
        if (sigma<-3)
        {
            x=Lk*tan((90+sigma)*3.1415/180);
            Kl=K*(x-dk/2)/x;
        }
        else Kl=K;
    }
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif
