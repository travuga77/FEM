/*
 * FEM_differential.c
 *
 *  Created on: 2 мар. 2018 г.
 *      Author: formu
 */

#include "DSP28x_Project.h"
#include <math.h>

extern int x;
extern float perc_per_g;

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
    if (sigma>0) Kr=K*(1-(R_TRACK*tan(sigma*3.1415/180)/(W_BASE))); else Kr=K;
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}

int differential_l(int K, int sigma)
{
    int Kl;
    if (sigma<0) Kl=K*(1+(R_TRACK*tan(sigma*3.1415/180)/(W_BASE))); else Kl=K;
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif

#ifdef grach_diff
int differential_r(int K, int sigma)
{
    int Kr;
    Kr=K*(1-(R_TRACK*tan(sigma*3.1415/180)/(2*W_BASE)));
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}

int differential_l(int K, int sigma)
{
    int Kl;
    Kl=K*(1+(R_TRACK*tan(sigma*3.1415/180)/(2*W_BASE)));
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif
#ifdef phillips_diff
int differential_r(int K, int sigma)
{
    int Kr; float x_a, Ka;
    Ka=perc_per_g*fabs(x)/1000;
    if (sigma>3)
    {
        x_a=W_BASE*tan((90-sigma)*3.1415/180);
        Kr=K*(x_a-R_TRACK/2)/x_a*(1-Ka);
    }
    else
    {
        if (sigma<-3)
        {
            x_a=W_BASE*tan((90+sigma)*3.1415/180);
            Kr=K*(x_a+R_TRACK/2)/x_a*(1+Ka);
        }
        else Kr=K;
    }
    if (Kr>0xFFF) Kr=0xFFF;
    return Kr;
}

int differential_l(int K, int sigma)
{
    int Kl; float x_a, Ka;
    Ka=perc_per_g*fabs(x)/1000;
    if (sigma>3)
    {

        x_a=W_BASE*tan((90-sigma)*3.1415/180);
        Kl=K*(x_a+R_TRACK/2)/x_a*(1+Ka);
    }
    else
    {
        if (sigma<-3)
        {
            x_a=W_BASE*tan((90+sigma)*3.1415/180);
            Kl=K*(x_a-R_TRACK/2)/x_a*(1-Ka);
        }
        else Kl=K;
    }
    if (Kl>0xFFF) Kl=0xFFF;
    return Kl;
}
#endif
