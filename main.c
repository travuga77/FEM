#include "DSP28x_Project.h"
#include <math.h>
#define alfa_n 55
#define DELAY_MS(X) (DELAY_US (X*1000))
#define Lk 160
#define dk 114
#define no_diff 1
//#define grach_diff_rules 1
//#define grach_diff 1
//#define phillips_diff 1
volatile struct ECAN_REGS ECanbShadow;
interrupt void cpu_timer0_isr(void);
void send_CAN_motors (int a, int b);

int16 Conter;
int16 AlfaBin1=0, AlfaBin2=0, AlfaBin10=0, AlfaBin20=0, AlfaBin11=0, AlfaBin21=0, AlfaBin3=0;
int16 PedalOut0;
int16 PedalOutmin1=0;
int PedalOut=0, pol=0, mb_temp=0;
int SteerOut=0;
float xkp=1.0, xkminus1=1.0, Kk=1.0, Pkminus1=1.0, Pk=1.0, Pkp=1.0, xk=1.0; //kalman
int F=1, H=1.2, R=512; float Q=0.005;                                           //kalman
int i=0,t=0;
int switch_diff=0;
int voltage_bms=0;
float current_bms=0;
float speedf=0.0, speedr=0.0, minspeedr=32000.0, minspeedf=32000.0, speedRR=0.0, speedLR=0.0, speedRF=0.0, speedLF=0.0, speed=0.0, speedRRmin1=0.0, speedLRmin1=0.0, speedRFmin1=0.0, speedLFmin1=0.0;
int err=0;
int alfa=alfa_n;
int gamma=0;
long CAP1min1;
int count;
int temp1=0, temp2=0;
int n=0, m=0;
int error_code=800;
int flag=1, flag2=1, can_pedal_rx_flag=0, can_mc_rx_flag=0;
unsigned int max1=0, max2=0;
long summalfa1=0, summalfa2=0;
float tang;
float hg=0.0;


struct spitxdata {
	Uint16 progr:8;      // 31:24
	Uint16 sot:7;        // 23:17
	Uint16 des:7;        // 16:10
	Uint16 edin:7;        // 9:3
	Uint16 batt:3;        // 2:0
} ;

union spitxd {
	Uint32 all;
	struct spitxdata edit;
} txdata;

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


void bspd(void)
{
    if (PedalOut>500 && !GpioDataRegs.GPBDAT.bit.GPIO50==1)
        flag=0;
    if (PedalOut<500 && !GpioDataRegs.GPBDAT.bit.GPIO50==0)
        flag=1;
}

void calibrate(void)
{
	//int a1, a2;
	AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
	while (AdcRegs.ADCST.bit.INT_SEQ1!=1) {}
	AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
	if (AdcMirror.ADCRESULT0>max1) max1=AdcMirror.ADCRESULT0;
	if (AdcMirror.ADCRESULT1>max2) max2=AdcMirror.ADCRESULT1;
}



void rtds(void)
{
    unsigned long r=0;
    GpioDataRegs.GPBSET.bit.GPIO40=1;
    while (r<1000000)
    {
    	r++;
    	calibrate();
    }
    GpioDataRegs.GPBCLEAR.bit.GPIO40=1;
    GpioDataRegs.GPACLEAR.bit.GPIO3=1;
    GpioDataRegs.GPACLEAR.bit.GPIO4=1;
}




void speedometer (int s)
{
    int a;
    if (t==10)
    {
        if (i==0)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO12=1;
            DELAY_US(1);
            GpioDataRegs.GPASET.bit.GPIO0=1;
            DELAY_US(1);
            GpioDataRegs.GPACLEAR.bit.GPIO0=1;


            a=fmod(s,10);
            while (a!=0)
            {
                GpioDataRegs.GPASET.bit.GPIO3=1;
                DELAY_US(1);
                GpioDataRegs.GPACLEAR.bit.GPIO3=1;
                DELAY_US(1);
                a--;
            }
            DELAY_US(1);
            GpioDataRegs.GPASET.bit.GPIO6=1;

        }

        if (i==1)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO6=1;
            DELAY_US(1);
            GpioDataRegs.GPASET.bit.GPIO0=1;
            DELAY_US(1);
            GpioDataRegs.GPACLEAR.bit.GPIO0=1;
            DELAY_US(1);


            a=fmod(s/10,10);
            while (a!=0) //
            {
                GpioDataRegs.GPASET.bit.GPIO3=1;
                DELAY_US(1);
                GpioDataRegs.GPACLEAR.bit.GPIO3=1;
                DELAY_US(1);
                a--;
            }
            GpioDataRegs.GPASET.bit.GPIO9=1;
            DELAY_US(1);
        }

        if (i==2)
       {
           GpioDataRegs.GPACLEAR.bit.GPIO9=1;
           DELAY_US(1);
           GpioDataRegs.GPASET.bit.GPIO0=1;
           DELAY_US(1);
           GpioDataRegs.GPACLEAR.bit.GPIO0=1;
           DELAY_US(1);


           a=(s/100);
           while (a!=0) //
           {
               GpioDataRegs.GPASET.bit.GPIO3=1;
               DELAY_US(1);
               GpioDataRegs.GPACLEAR.bit.GPIO3=1;
               DELAY_US(1);
               a--;
           }
           GpioDataRegs.GPASET.bit.GPIO12=1;
           DELAY_US(1);
       }

        /*
        GpioDataRegs.GPASET.bit.GPIO0=1;
        DELAY_US(1);
        GpioDataRegs.GPACLEAR.bit.GPIO0=1;
        DELAY_US(1);
        */
        if (i==2) i=0; else i++;

        t=0;
    }

    else t++;
}


void rtd (void)
{
    DINT;
    int r=0;
    while (r!=1000)
    {
    	speedometer(error_code);
        if (GpioDataRegs.GPBDAT.bit.GPIO34==1) GpioDataRegs.GPASET.bit.GPIO4=1; else GpioDataRegs.GPACLEAR.bit.GPIO4=1;
        if (!GpioDataRegs.GPBDAT.bit.GPIO50==1)
        {
        	GpioDataRegs.GPASET.bit.GPIO3=1;
        	//GpioDataRegs.GPBSET.bit.GPIO47=1;
        }
        else
        {
        	GpioDataRegs.GPACLEAR.bit.GPIO3=1;
        	//GpioDataRegs.GPBCLEAR.bit.GPIO47=1;
        }

#ifdef FLASH
        if (GpioDataRegs.GPBDAT.bit.GPIO34*!GpioDataRegs.GPBDAT.bit.GPIO50*GpioDataRegs.GPCDAT.bit.GPIO73==1)
#endif
#ifdef RAM
        if (GpioDataRegs.GPBDAT.bit.GPIO34*!GpioDataRegs.GPBDAT.bit.GPIO50==1)
#endif
        {
            r++;
        }
    }
    rtds ();
    EINT;
}

void spi_xmit(Uint16 a)
{
    SpiaRegs.SPITXBUF=a;
}

///////////////////////////////////////////////////////////////////////////////
int16 getwheelangle(int16 rawsteer)
{
	rawsteer-=0x570;
	rawsteer/=103;
	return rawsteer;
}
/*
//////////////////////////////////////////////////////////////////////////////////////
int16 getpedalangle1(int16 rawpedal) //blue
{
    rawpedal-=1670;
	rawpedal*=3.5;
	if (rawpedal<0) rawpedal=0;
	if (rawpedal>4095) rawpedal=4095;
#ifdef grach_diff
	if (rawpedal>0xDFF) rawpedal=0xDFF;
#endif
	return rawpedal;
}
////////////////////////////////////////////////////////////////////////////////

int16 getpedalangle2(int16 rawpedal) //green
{
    rawpedal-=1720+50;
    rawpedal*=4.6;
    if (rawpedal<0) rawpedal=0;
    if (rawpedal>4095) rawpedal=4095;
#ifdef grach_diff
	if (rawpedal>0xDFF) rawpedal=0xDFF;
#endif
    return rawpedal;
}
//////////////////////////////////////////////////////////////////////////////////////
*/
void kalman (float q)
{
    xkp=F*xkminus1;
    Pkp=F*Pkminus1*F+Q;

    Kk=(Pkp*H)/(H*Pkp*H+R);
    xk=xkp+Kk*(q-H*xkp);
    Pk=(1-Kk*H)*Pkp;

    xkminus1=xk;
    Pkminus1=Pk;

    //return xk;
}

interrupt void can_int_isr(void)
{
    can_pedal_rx_flag=1;
// Acknowledge this interrupt to get more from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

interrupt void xint7_isr(void)
{
    count++;
    //PieCtrlRegs.PIEIER12.bit.INTx3 = 0;  // Enable PIE Group 12 INTx3

    //PieCtrlRegs.PIEIER12.bit.INTx3 = 1;  // Enable PIE Group 12 INTx3
// Acknowledge this interrupt to get more from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

void turnoffmf (void)
{
	AlfaBin1=0;
	AlfaBin2=0,
	AlfaBin10=0;
	AlfaBin20=0;
	AlfaBin11=0;
	AlfaBin21=0;
	AlfaBin3=0;
	PedalOut0=0;
	PedalOut=0;
	send_CAN_motors(0,0);
	summalfa1=0;
	summalfa2=0;
	max1=0; max2=0;
	rtd();
}
/*
void ten_percent(int a, int b)
{
    if (fabs(a-b)>410) //!!!!!!!
    {
       flag2=0;
    }
    else
    {
       flag2=1;
    }
}
*/
void read_ADC(void) {
	AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
	while (AdcRegs.ADCST.bit.INT_SEQ1!=1) {}
	AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
}
/*
void summ_average(void) {
	AlfaBin10 = AdcMirror.ADCRESULT0;
	AlfaBin20 = AdcMirror.ADCRESULT1;
	AlfaBin1 = getpedalangle1(AlfaBin10);
	AlfaBin2 = getpedalangle2(AlfaBin20);
	summalfa1+=AlfaBin1;
	summalfa2+=AlfaBin2;
	m++;
}
*/
void calc_temp(void) {
	temp1=AdcMirror.ADCRESULT3;
	temp2=AdcMirror.ADCRESULT4;
	temp1=(temp1-2930)/5.125+20;
	temp2=(temp2-2930)/5.125+20;
}

void send_CAN_steer (int c) {
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
	ECanbMboxes.MBOX27.MDL.word.LOW_WORD=right;   //TEST AREA
	ECanbMboxes.MBOX27.MDL.word.HI_WORD=left;   //TEST AREA

	ECanbShadow.CANTRS.all = 0;
		ECanbShadow.CANTRS.bit.TRS27 = 1;
		ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
	do
	{
		ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
	} while (ECanbShadow.CANTA.bit.TA27 == 0);   			// Wait for TA27 bit to be set...
	ECanbShadow.CANTA.all = 0;
		ECanbShadow.CANTA.bit.TA27 = 1;     	       		// Clear TA27
		ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
}

void send_CAN_datalogger (int tr0, int tr1, int tr2, int tr3) {
    ECanbMboxes.MBOX10.MDL.word.LOW_WORD=tr0;   //TEST AREA
    ECanbMboxes.MBOX10.MDL.word.HI_WORD=tr1;   //TEST AREA
    ECanbMboxes.MBOX10.MDH.word.LOW_WORD=tr2;   //TEST AREA
    ECanbMboxes.MBOX10.MDH.word.HI_WORD=tr3;   //TEST AREA

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
}

void send_CAN_motors (int a, int b) {
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
/*	ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
	    do
	    {
	        ECanbShadow. CANRMP.all = ECanbRegs. CANRMP.all;
	    } while(ECanbShadow. CANRMP.bit.RMP29 != 1 );                       // Wait for RMP1 bit to be set..
	    ECanbShadow.CANRMP.bit.RMP29 = 1;
	    ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;
*/

	voltage_bms=(ECanbMboxes.MBOX28.MDL.all)*0.0015;
	current_bms=ECanbMboxes.MBOX29.MDL.word.HI_WORD/10;


	GpioDataRegs.GPATOGGLE.bit.GPIO3=1;

	ECanbShadow.CANTRS.all = 0;
		ECanbShadow.CANTRS.bit.TRS25 = 1;
		ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
	do
	{
		ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
	} while (ECanbShadow.CANTA.bit.TA25 == 0);   			// Wait for TA25 bit to be set...
	ECanbShadow.CANTA.all = 0;
		ECanbShadow.CANTA.bit.TA25 = 1;     	       		// Clear TA25
		ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

	ECanbShadow.CANTRS.all = 0;
		ECanbShadow.CANTRS.bit.TRS26 = 1;
		ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
	do
	{
		ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;
	} while (ECanbShadow.CANTA.bit.TA26 == 0);   			// Wait for TA26 bit to be set...
	ECanbShadow.CANTA.all = 0;
		ECanbShadow.CANTA.bit.TA26 = 1;     	       		// Clear TA26
		ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
}

/*
void calc_alfa() {
	if (PedalOutmin1<2000)
	alfa=alfa_n;
	else
	hg=current_bms/225;
	alfa=alfa_n*(1-hg);
}
*/

void calc_speed(void) {
    if (ECap1Regs.CAP1>30000000 || ECap1Regs.CAP1==0)
        speedLR=0;
    else {
        speedLR=10406250*3.6/ECap1Regs.CAP1; //1665mm/24=69.4mm/hole=0.0694m
        ECap1Regs.CAP1=0;
    }
    if (ECap2Regs.CAP1>30000000 || ECap2Regs.CAP1==0)
        speedRR=0;
    else {
        speedRR=10406250*3.6/ECap2Regs.CAP1;
        ECap2Regs.CAP1=0;
    }
    if (ECap3Regs.CAP1>30000000 || ECap3Regs.CAP1==0)
        speedLF=0;
    else {
        speedLF=10406250*3.6/ECap3Regs.CAP1;
        ECap3Regs.CAP1=0;
    }
    if (ECap4Regs.CAP1>30000000 || ECap4Regs.CAP1==0)
        speedRF=0;
    else {
        speedRF=10406250*3.6/ECap4Regs.CAP1;
        ECap4Regs.CAP1=0;
    }

    speedf=(speedRF+speedLF)/2;
    speedr=(speedRR+speedLR)/4;
    speed=(speedf+speedr)/2;
}

void killswitch_detect(void) {
	if (GpioDataRegs.GPCDAT.bit.GPIO73 == 0) {
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

void calc_steer_wheel_adc(void) {
    AlfaBin3 = AdcMirror.ADCRESULT2;
    SteerOut = getwheelangle(AlfaBin3);
}

void calc_steer_wheel_spi(void) {
    spi_xmit(0xff);
    while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { }
    SteerOut = SpiaRegs.SPIRXBUF;
    SteerOut+=16384;
    SteerOut/=520;
}
/*
void calc_PedalOut(void) {
    AlfaBin11=summalfa1/m;
    AlfaBin21=summalfa2/m;
    summalfa1=summalfa2=0;
    m=0;
    ten_percent(AlfaBin11,AlfaBin21);
    if (AlfaBin11<AlfaBin21) PedalOut0=AlfaBin11; else PedalOut0=AlfaBin21;
    PedalOut=PedalOut0;
}

void correct_PedalOut(void) {
	calc_alfa();
	if (PedalOutmin1+alfa<PedalOut) PedalOut=PedalOutmin1+alfa;
	PedalOutmin1=PedalOut;
	gamma=409+Rav*120*4.1*3686/(22*40*voltage_bms);
	if (PedalOutmin1<gamma) PedalOutmin1=gamma;
}
*/

interrupt void cpu_timer0_isr(void)
{
    read_ADC();

    //summ_average();
    // Acknowledge this interrupt to receive more interrupts from group 1
    error_code=ECanbMboxes.MBOX31.MDL.word.LOW_WORD;
    if (error_code == 0)
        //speedometer(voltage_bms);
        speedometer(current_bms);
        //speedometer(speedf);
        //speedometer(alfa);
    else speedometer(error_code);

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

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

interrupt void cpu_timer1_isr(void) {
//Conter=XIntruptRegs.XINT1CTR;
	//GpioDataRegs.GPATOGGLE.bit.GPIO5 = 1;
	calc_speed();
	//calc_steer_wheel_adc();
	calc_steer_wheel_spi();
	//calc_PedalOut();


	can_pedal_rx_flag=ECanbRegs.CANRMP.bit.RMP30;
	PedalOut=ECanbMboxes.MBOX30.MDL.word.LOW_WORD*can_pedal_rx_flag+300;
    ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
    ECanbShadow.CANRMP.bit.RMP30=1;
    ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;
	ECanbMboxes.MBOX30.MDL.word.LOW_WORD=0;
	if (PedalOut<0) PedalOut=0;
	if (PedalOut>4095) PedalOut=4095;

	correct_alfa();
	pol=4095*1.3*speedf/98+300;
	if (pol>4095) pol=4095;
	if (PedalOut>=PedalOutmin1+alfa) {
        PedalOut=PedalOutmin1+alfa;
	}
	PedalOutmin1=PedalOut;
	if (PedalOutmin1<pol) PedalOutmin1=pol;

	calc_temp();

	//correct_PedalOut();

	//if (PedalOut>1200) PedalOut=1200;
	bspd();
	tang=tan(SteerOut*3.1415/180);
	if (fabs(SteerOut)>5)
	    send_CAN_motors(differential_r(PedalOut,SteerOut)*flag, differential_l(PedalOut,SteerOut)*flag);
	else
	    send_CAN_motors(PedalOut*flag, PedalOut*flag);
    send_CAN_priborka(minspeedr*100,speedr*100);
    send_CAN_steer(SteerOut);
    send_CAN_datalogger(minspeedr*100,speedr*100,minspeedf*100,speedf*100);

#ifdef FLASH
	killswitch_detect();
#endif
	//txdata.edit.batt=0;
	//txdata.edit.edin=0;
	//txdata.edit.des=0;
	//txdata.edit.sot=0;
	//txdata.edit.progr=0;
	//spi_xmit(txdata.send.rh);
	//spi_xmit(txdata.send.lh);
}

void spi_init()
{
	SpiaRegs.SPICCR.all =0x004F;	             // Reset on, rising edge, 16-bit char bits
	SpiaRegs.SPICTL.all =0x000F;    		     // Enable master mode, normal phase,
                                                 // enable talk, and SPI int disabled.
	SpiaRegs.SPIBRR =0x007F;
    SpiaRegs.SPICCR.all =0x0CF;		         // Relinquish SPI from Reset
    SpiaRegs.SPIPRI.bit.FREE = 1;                // Set so breakpoints disturb xmission
}

void spi_fifo_init()
{
// Initialize SPI FIFO registers
    SpiaRegs.SPIFFTX.all=0xE040;
    SpiaRegs.SPIFFRX.all=0x204f;
    SpiaRegs.SPIFFCT.all=0x0;
}

void stop_light(void) {
   	if (!GpioDataRegs.GPBDAT.bit.GPIO50==1) {
		GpioDataRegs.GPBSET.bit.GPIO47=1;
	}
	else {
		GpioDataRegs.GPBCLEAR.bit.GPIO47=1;
	}
}



void main(void){
// Initialize System Control: PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();

	// Copy time critical code and Flash setup code to RAM.
    // The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
    // symbols are created by the linker. Refer to the linker files.
#ifdef FLASH
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
#endif
    // Call Flash Initialization to setup flash waitstates.
    // This function must reside in RAM.
    InitFlash(); // Call the flash wrapper init function.


// Clear all interrupts and initialize PIE vector table, disable CPU interrupts
	DINT;
// Initialize PIE control registers to their default state
// The default state is all PIE interrupts disabled and flags are cleared
	InitPieCtrl();
// Disable CPU interrupts and clear all CPU interrupt flags
	IER = 0x0000;
	IFR = 0x0000;
// Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR)
// This will populate the entire table, even if the interrupt is not used in this example
	InitPieVectTable();
// Interrupts that are used in this example are re-mapped to ISR functions found within this file.
	EALLOW;
//    PieVectTable.XINT1 = &xint1_isr;
//    PieVectTable.XINT2 = &xint2_isr;
//    PieVectTable.XINT3 = &xint3_isr;
//    PieVectTable.XINT4 = &xint4_isr;
//    PieVectTable.XINT5 = &xint5_isr;
//    PieVectTable.XINT6 = &xint6_isr;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &cpu_timer1_isr;
	PieVectTable.ECAN0INTB = &can_int_isr;
	EDIS;



	InitECap1Gpio();

    ECap1Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped

    // Configure peripheral registers
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;      // One-shot
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 1 event
    ECap1Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap1Regs.ECCTL2.bit.REARM = 1;            // arm one-shot
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap1Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt

    InitECap2Gpio();

    ECap2Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap2Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap2Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped

    // Configure peripheral registers
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;      // One-shot
    ECap2Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 1 event
    ECap2Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
    ECap2Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation

    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap2Regs.ECCTL2.bit.REARM = 1;            // arm one-shot
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap2Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt

    InitECap3Gpio();

    ECap3Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap3Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap3Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped

    // Configure peripheral registers
    ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;      // One-shot
    ECap3Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 1 event
    ECap3Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
    ECap3Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation

    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap3Regs.ECCTL2.bit.REARM = 1;            // arm one-shot
    ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap3Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt


    InitECap4Gpio();

    ECap4Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap4Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap4Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
    ECap4Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped

    // Configure peripheral registers
    ECap4Regs.ECCTL2.bit.CONT_ONESHT = 0;      // One-shot
    ECap4Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 1 event
    ECap4Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
    ECap4Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation

    ECap4Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap4Regs.ECCTL2.bit.REARM = 1;            // arm one-shot
    ECap4Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap4Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt


	InitSpiGpio();
	spi_init();
	spi_fifo_init();

	InitAdc();  // For this example, init the ADC

	InitCpuTimers();


// ECanb init
	InitECan();
	InitECanGpio();

/* Write to the MSGID field  */

	ECanbMboxes.MBOX10.MSGID.all = 0; // Standart Identifier
	ECanbMboxes.MBOX15.MSGID.all = 0; // Standart Identifier
	ECanbMboxes.MBOX16.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX24.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX25.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX26.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX27.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX28.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX29.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX30.MSGID.all = 0; // Standart Identifier
    ECanbMboxes.MBOX31.MSGID.all = 0; // Standart Identifier

    ECanbMboxes.MBOX10.MSGID.bit.STDMSGID = 0x136; // Standart Identifier
    ECanbMboxes.MBOX24.MSGID.bit.STDMSGID = 0x400; // Standart Identifier
    ECanbMboxes.MBOX25.MSGID.bit.STDMSGID = 0x220; // Standart Identifier
    ECanbMboxes.MBOX26.MSGID.bit.STDMSGID = 0x221; // Standart Identifier
    ECanbMboxes.MBOX27.MSGID.bit.STDMSGID = 0x120; // Standart Identifier

    ECanbMboxes.MBOX28.MSGID.bit.AME=1;
    ECanbLAMRegs.LAM28.all=0x80000000;
    ECanbLAMRegs.LAM29.all=0x80000000;
    ECanbLAMRegs.LAM30.all=0x80000000;
    ECanbLAMRegs.LAM31.all=0x80000000;
    ECanbMboxes.MBOX15.MSGID.bit.STDMSGID = 0x1A0; // Standart Identifier
    ECanbMboxes.MBOX16.MSGID.bit.STDMSGID = 0x1A1; // Standart Identifier
    ECanbMboxes.MBOX28.MSGID.bit.STDMSGID = 0x3A0; // Standart Identifier
    ECanbMboxes.MBOX29.MSGID.bit.STDMSGID = 0x1A0; // Standart Identifier
    ECanbMboxes.MBOX30.MSGID.bit.STDMSGID = 0x150; // Standart Identifier
    ECanbMboxes.MBOX31.MSGID.bit.STDMSGID = 0x151; // Standart Identifier

/* Configure Mailbox under test as a Transmit mailbox */

	ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;
	ECanbShadow.CANMD.bit.MD10 = 0;
    ECanbShadow.CANMD.bit.MD24 = 0;
	ECanbShadow.CANMD.bit.MD25 = 0;
	ECanbShadow.CANMD.bit.MD26 = 0;
	ECanbShadow.CANMD.bit.MD27 = 0;
	ECanbShadow.CANMD.bit.MD15 = 1;
	ECanbShadow.CANMD.bit.MD16 = 1;
    ECanbShadow.CANMD.bit.MD28 = 1;
    ECanbShadow.CANMD.bit.MD29 = 1;
    ECanbShadow.CANMD.bit.MD30 = 1;
    ECanbShadow.CANMD.bit.MD31 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

/* Enable Mailbox under test */

	ECanbShadow.CANME.all = ECanbRegs.CANME.all;
	ECanbShadow.CANME.bit.ME10 = 1;
    ECanbShadow.CANME.bit.ME24 = 1;
	ECanbShadow.CANME.bit.ME25 = 1;
	ECanbShadow.CANME.bit.ME26 = 1;
	ECanbShadow.CANME.bit.ME27 = 1;
	ECanbShadow.CANME.bit.ME15 = 1;
	ECanbShadow.CANME.bit.ME16 = 1;
	ECanbShadow.CANME.bit.ME28 = 1;
	ECanbShadow.CANME.bit.ME29 = 1;
    ECanbShadow.CANME.bit.ME30 = 1;
    ECanbShadow.CANME.bit.ME31 = 1;
	ECanbRegs.CANME.all = ECanbShadow.CANME.all;

	EALLOW;
	ECanbShadow.CANMIM.all = ECanbRegs.CANMIM.all;
	ECanbShadow.CANMIM.bit.MIM30=1;
	ECanbRegs.CANMIM.all = ECanbShadow.CANMIM.all;

	ECanbShadow.CANGIM.all = ECanbRegs.CANGIM.all;
	ECanbShadow.CANGIM.bit.I0EN=1;
	ECanbRegs.CANGIM.all = ECanbShadow.CANGIM.all;
	EDIS;


/* Write to DLC field in Master Control reg */

	ECanbMboxes.MBOX10.MSGCTRL.bit.DLC = 8;
    ECanbMboxes.MBOX24.MSGCTRL.bit.DLC = 1;
	ECanbMboxes.MBOX25.MSGCTRL.bit.DLC = 2;
	ECanbMboxes.MBOX26.MSGCTRL.bit.DLC = 2;
	ECanbMboxes.MBOX27.MSGCTRL.bit.DLC = 4;
	ECanbMboxes.MBOX15.MSGCTRL.bit.DLC = 6;
	ECanbMboxes.MBOX16.MSGCTRL.bit.DLC = 6;
	ECanbMboxes.MBOX28.MSGCTRL.bit.DLC = 6;
	ECanbMboxes.MBOX29.MSGCTRL.bit.DLC = 6;
    ECanbMboxes.MBOX30.MSGCTRL.bit.DLC = 2;
    ECanbMboxes.MBOX31.MSGCTRL.bit.DLC = 2;

/* Write to the mailbox RAM field */

    ECanbMboxes.MBOX10.MDL.all = 0x0;
    ECanbMboxes.MBOX24.MDL.all = 0x0;
	ECanbMboxes.MBOX25.MDL.all = 0x0;
	ECanbMboxes.MBOX26.MDL.all = 0x0;
	ECanbMboxes.MBOX27.MDL.all = 0x0;




// Enable Xint3 in the PIE: Group 12 interrupt 1
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
//	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;  // Enable PIE Group 12 INTx1
//    PieCtrlRegs.PIEIER12.bit.INTx2 = 1;  // Enable PIE Group 12 INTx2
//    PieCtrlRegs.PIEIER12.bit.INTx3 = 1;  // Enable PIE Group 12 INTx3
//	PieCtrlRegs.PIEIER12.bit.INTx4 = 1;  // Enable PIE Group 12 INTx4
	PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  // Enable PIE Group 9 INTx7


    //PieCtrlRegs.PIEIER12.bit.INTx1 = 1; // Enable PIE Group 12 INTx2
    //PieCtrlRegs.PIEIER12.bit.INTx2 = 1;  // Enable PIE Group 12 INTx2

	//IER |= M_INT9;
	IER |= M_INT12; // Enable CPU int12
	IER |= M_INT13;
	//IER |= M_INT1;
	//EINT; // Enable Global Interrupts


    EALLOW;
    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 0;        //xint3 sync to sysclockout
    GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 0;        //xint4 sync to sysclockout
    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 0;        //xint5 sync to sysclockout
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0;        //xint6 sync to sysclockout
    GpioCtrlRegs.GPBQSEL1.bit.GPIO39 = 0;        //xint7 sync to sysclockout
    GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
    GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL=27;     //gpio59 as input for XINT1 LF
    GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL=30;     //gpio62 as input for XINT2 RF
    GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL=26;     //gpio58 as input for XINT3 LR
    GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL=28;     //gpio60 as input for XINT4 RR
    GpioIntRegs.GPIOXINT7SEL.bit.GPIOSEL=7;     //gpio39 as input for XINT5 button
    EDIS;
    XIntruptRegs.XINT3CR.bit.ENABLE=1;          //turn on XINT3
    XIntruptRegs.XINT4CR.bit.ENABLE=1;          //turn on XINT4
    XIntruptRegs.XINT5CR.bit.ENABLE=1;          //turn on XINT5
    XIntruptRegs.XINT6CR.bit.ENABLE=1;          //turn on XINT6
    XIntruptRegs.XINT7CR.bit.ENABLE=1;          //turn on XINT7
    XIntruptRegs.XINT3CR.bit.POLARITY=0x1;      //rising edge
    XIntruptRegs.XINT4CR.bit.POLARITY=0x1;      //rising edge
    XIntruptRegs.XINT5CR.bit.POLARITY=0x1;      //rising edge
    XIntruptRegs.XINT6CR.bit.POLARITY=0x1;      //rising edge
    XIntruptRegs.XINT7CR.bit.POLARITY=0x0;      //rising edge



// GPIO5 is Ouput
	EALLOW;
        GpioCtrlRegs.GPADIR.bit.GPIO0 = 1; // Output ex speedometer
        GpioCtrlRegs.GPADIR.bit.GPIO3 = 1; // Output ex speedometer
        GpioCtrlRegs.GPADIR.bit.GPIO4 = 1; // Output led
        GpioCtrlRegs.GPADIR.bit.GPIO6 = 1; // Output ex speedometer
        GpioCtrlRegs.GPADIR.bit.GPIO9 = 1; // Output ex speedometer
        GpioCtrlRegs.GPADIR.bit.GPIO12 = 1; // Output ex speedometer
        GpioCtrlRegs.GPADIR.bit.GPIO17 = 0; // input spi
        GpioCtrlRegs.GPADIR.bit.GPIO5 = 1; // Output led
        GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0; // Input start button
        GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0; // Input brake pedal
        GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1; //output ex stop light
        GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;
        GpioCtrlRegs.GPCDIR.bit.GPIO73 = 0; //input killswitch input
        GpioDataRegs.GPCSET.bit.GPIO73=1;
        //GpioCtrlRegs.GPAPUD.bit.GPIO24 = 1;
        //GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
        //GpioDataRegs.GPBSET.bit.GPIO50 = 1;
        GpioCtrlRegs.GPBDIR.bit.GPIO40 = 1; // Output

	EDIS;

	// Configure ADC
	   AdcRegs.ADCMAXCONV.all = 0x0004;       // Setup 5 conv's on SEQ1
	   AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x5; // Setup ADCINA5 as 1st SEQ1 conv.
	   AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x7; // Setup ADCINA7 as 2nd SEQ1 conv.
	   AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x3; // Setup ADCINA3 as 3nd SEQ1 conv.
	   AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x8; // Setup ADCINB0 as 4th SEQ1 conv.
	   AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0xA; // Setup ADCINB2 as 5th SEQ1 conv.
	   AdcRegs.ADCTRL1.bit.CONT_RUN =0;
	   AdcRegs.ADCTRL3.bit.ADCCLKPS=0xf;
	   AdcRegs.ADCTRL1.bit.ACQ_PS=0xF;
	   AdcRegs.ADCTRL1.bit.CPS=1;


	   //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts //interrupts

	ConfigCpuTimer(&CpuTimer0, 100, 500);
	ConfigCpuTimer(&CpuTimer1, 100, 50000);
	CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

	Conter=0;

	rtd();

	while(1)
	{
		//stop_light();
	}
}
//========================================================================

