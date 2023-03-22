/**@file main.c        @PROJECT_NAME "ServoTester2"

@brief  Exercise servo movement while monitoring current demand from servo.
   Servo movement sweeps either side of mid position,
    sweep controlled by a potentiometer, from zero sweep through to max sweep.
   1000ms after a sweep, the direction changes. 
   A toggle switch inhibits the direction change.
   Pot configuration allows normal sweep (1ms/2ms) or wide sweep (0.5ms/2.5ms).
   If servo current < 20mA then Yellow LED light.
   If servo current >= 20mA then Yellow+green LEDs light.
   If servo current > 500mA then Red LED light.
   If Vdd drops below 4.5v then yellow LED blinks off, at 500ms.

@author (c)2022 Dave Harris, Andover, UK.  (MERG member 2740 'WortingUK')
@copyright Creative Commons BY-NC-SA (Attribution-NonCommercial-ShareAlike)
 
@version 1.0

@history 23-Nov-2022 v0.0 DH started
@history 17-Jan-2023 v0.1 DH stable 
@history 22-Mar-2023 v1.0 DH production

@brief Environment
   Language  C  Version C99.
   IDE       Microchip MPLAB X v5.50, using MCC (MPLAB Code Configurator) v4.
   Compiler  XC8 v2.32 (free). Device Family Pack: PIC16F1xxxx_DFP (v1.13.178)
   MCU       Microchip PIC16F18313   SOIC-8 package
   PCB       KiCad project.
   Panel     OpenSCAD project 

@brief Peripheral configuration (Setup in MCC)
   FOSC - HFINTOSC 32MHz
   TMR0 - 8 bit, FOSC/4, PreScaler 1:1, period 10.0us, interrupt (generate PWM)
   FVR  - ADFVR gain x1 = 1.024V -  used by ADC VPREF
   ADC  - 10 bit, FOSC/32 (TAD 1.0us), Conversion 11.5us, right align.
          ADC VPREF is VDD (read pot or read FVR) or FVR (Amp sense)

@brief MCU pin/port usage
  Pins names configured in MCC and defined in pin_manager.h
      names PIN_LED2, PIN_YELLOW, PIN_SERVO, PIN_AN_POT, PIN_AN_SENSE, PIN_SW1
    PIC 16F1xxxx top view        +---_---+ 
                      5.0V - VDD | 1   8 | VSS - 0V
      LEDs red/green - 3k3 - RA5 | 2   7 | RA0/ICSPDAT - AN0 - Pot sweep setting
   Current 0R5 sense - AN4 - RA4 | 3   6 | RA1/ICSPCLK - RA1 - 3k3 - LED yellow
    HoldSW - ^RA3 - RA3/MCLR/VPP | 4   5 | RA2 - Servo signal
                                 +-------+                           ^ has WPU
    Current sense input has an 70ms RC low pass filter, 10k + 10uF.
    The ICSP header is also the operational potentiometer/switch connections.

@brief Memory usage summary
    Program space        used   288h (   648) of   800h words   ( 31.6%)
    Data space           used    24h (    36) of   100h bytes   ( 14.1%)
*/


/****************************************************************************
@brief includes headers/libraries
*/

#include "main.h" /* main header - data type, constant, global variable */


/****************************************************************************
@brief function testForVDDfault() - Show fault if VDD is low.
   Measure FVR with plus ref as the variable VDD.
   Called every 20ms.

@param  none
@return none
*/

void testForVDDfault(void)
{
    setAdcPosRefVolt(ADPREF_VDD);
    adc_result_t vddValue = ADC_GetConversion(channel_FVR);
    if(vddValue > VDDTHRES_4_5)                         /* value is inverse */
    {
        TMR0_StopTimer();
        PIN_SERVO_SetDigitalInput();   /* disable servo signal */

        for(uint8_t count20ms = 0; count20ms < 150; count20ms++)  /* 3 sec */
        {
            yellowBlink(BLINK_OFF);
            __delay_ms(20);          /* blocks code  */
        }
        PIN_SERVO_SetDigitalOutput();   /* enable servo signal */
        TMR0_StartTimer(); /* allow a retry */
    }
}


/****************************************************************************
@brief function readPotDCcount() - get and condition the pot DC value

@remark ADC max 1024, divided by 10, there are 100 steps = 0.9°.

@param  none
@return count_dc_t - duty cycle sweep count 0° to 90° (0-100)
*/

count_dc_t readPotDCcount(void)
{
    setAdcPosRefVolt(ADPREF_VDD);
    count_dc_t DCvalue = (count_dc_t)(ADC_GetConversion(PIN_AN_POT) / 10);
    
    if(DCvalue > 100) /* DCvalue could be 101 or 102 */
    {
        DCvalue = 100;
    }
    return DCvalue;
}


/****************************************************************************
@brief function readSenseAmps() - get and condition the sense Amps value

@param  none
@return adc_result_t - current 
*/

adc_result_t readSenseAmps(void)
{
    setAdcPosRefVolt(ADPREF_FVR);
    return ADC_GetConversion(PIN_AN_SENSE);
}


/****************************************************************************
@brief function setADrefVolt() - Set ADC plus Ref to FVR or VDD

@remark This is specific to PIC16F1xxxx with FVR

@param[in] enum adPref_t adPref
@return none
*/

void setAdcPosRefVolt(enum adPref_t adPosRef)
{
    switch(adPosRef)
    { 
        case ADPREF_FVR :
            ADCON1bits.ADPREF = 0b11; /* ADC + ref is FVR */
            break;
        case ADPREF_VDD :
            ADCON1bits.ADPREF = 0b00; /* ADC + ref is VDD */
            break;
    }
}


/***************************************************************************
@brief function  yellowBlink - Yellow LED blink
     Called every 20ms 

@param[in] enum blink_t - BLINK_ON or BLINK_OFF
@return none
*/

void yellowBlink(enum blink_t blink)
{
    static _Bool pass = false;

    static count_20ms_t count20ms = 0; /* count of 20ms */

    PIN_LED2_SetDigitalInput();     /* red/green disabled */
    
    if(count20ms++ > 22)            /* GT 460ms - short part of blink, 40ms */ 
    {
        if(count20ms > 25)          /* reset after 500ms */
        {
            count20ms = 0;
            pass = ! pass;          /* switch to the other pass  */
        }
        else if(pass)               /* this pass is red on */
        {
            PIN_YELLOW_SetLow();    /* Yellow off  */         
        }
        else                        /* the other pass is yellow on  */
        {   
            PIN_YELLOW_SetHigh();   /* Yellow on  */       
        }
    }
    else                            /* long part of blink, 460ms  */
    {
        if(blink == BLINK_ON)       /* blink on or off ? */
        {    
            PIN_YELLOW_SetLow();    /* Yellow off   */   
        }
        else
        {   
            PIN_YELLOW_SetHigh();   /* Yellow on     */
        }
    }
}


/***************************************************************************
@brief function decodeAmpsToLED() - decode mA value to LED states

@remark Called every 20ms 

@param[in] adc_result_t mA  - See enum mA_threshold_t
@return none
*/

void decodeAmpsToLED(adc_result_t mA)
{
    static uint8_t count0 = 0;

    if(mA > mA_500) // greater than 500mA show red only
    {
        count0 = 0;
        PIN_YELLOW_SetLow();         /* Yellow pin low, so off*/
        PIN_LED2_SetDigitalOutput(); /* ensure LED2 is enabled */
        PIN_LED2_SetHigh();          /* Red on due to yellow low */
    }
    else
        if(mA >= mA_20)        /* >= 20mA show yellow + green */
        {
            count0 = 0;
            PIN_LED2_SetDigitalOutput(); /* ensure LED2 is enabled */
            PIN_LED2_SetLow();           /* Green on, Red off */
            PIN_YELLOW_SetHigh();        /* Yellow on */
        }
        else /* between less than 20mA, show yellow only*/
        {
            count0 = 0;
            PIN_LED2_SetDigitalInput(); /* red/green LED disabled */
            PIN_YELLOW_SetHigh();       /* yellow on */
        }
}


/***************************************************************************
@brief function processServo() - process servo duty cycle movement

@remark Uses globals g_pwmDCcount, g_isPwmDone, g_countIsr20ms

@param[in] enum phase_t phase
@param[in] count20ms_t duration20ms - duration count in 20ms steps
@return none
*/

void processServo(enum phase_t phase, count_20ms_t duration20ms)
{ 
    g_countIsr20ms = 0;
    do
    {
        while(! g_pwmDoneFlag);          /* sync to end of PWM pulse */
        g_pwmDoneFlag = false;
        
        switch(phase)
        {
            case PHASE_MINUS:   /* deduct sweep */
                g_pwmDCcount = DC_1point5ms - readPotDCcount(); 
                break;
            case PHASE_PLUS:   /* add sweep */
                g_pwmDCcount = DC_1point5ms + readPotDCcount();
                break;
            case PHASE_CENTER:              /* just 1.50 ms  */
                g_pwmDCcount = DC_1point5ms;
                break;
        };
        testForVDDfault();
        decodeAmpsToLED(readSenseAmps());
    }
    while(g_countIsr20ms < duration20ms);
}


/****************************************************************************
@brief function ISR_PWM_TMR0() - servo PWM generator, on TMR0 overflow at 10us

@remark  Before this...
            call TMR0_SetInterruptHandler(* PWM_TMR0_ISR)
            call TMR0_Initialize()
    This code must be very fast, since its called every 10us.
    Modifies globals g_countIsr20ms, g_isPwmDone and g_pwmDCcount.
 
@param  none
@return none
*/

void PWM_TMR0_ISR(void)
{
    static uint16_t count10us = 0;      /* increments every 10 us */

    if(count10us++ > 2000)              /* 20ms ? - servo signal period */
    {
        count10us = 0;
        PIN_SERVO_SetHigh();            /* set PWM pulse */
        g_countIsr20ms++;
        g_pwmDoneFlag = false;
        return;                         /* early exit */
    }
    if(count10us == g_pwmDCcount)
    {
        PIN_SERVO_SetLow();             /* clear PWM pulse */
        g_pwmDoneFlag = true;
    }
}                             /* return from interrupt */


/****************************************************************************
@brief function main() - Called on power up

@param  none
@return none
*/

void main(void)
{
    SYSTEM_Initialize();                      /* defined in MCC */
    TMR0_Initialize();
    TMR0_SetInterruptHandler(* PWM_TMR0_ISR); /* 10us interrupt, PWM maker */
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    static enum phase_t phase = PHASE_CENTER;

    processServo(phase, 100);                 /* 100 x 20ms = 2s */

    while(true) /* loop forever */
    {
        if(PIN_SW1_GetValue() == SW1_RUN)
        {
            if(phase == PHASE_MINUS)
            {
                phase = PHASE_PLUS;
            }
            else
            {
                phase = PHASE_MINUS;
            }
        }
        processServo(phase, 50);              /* 50 x 20ms = 1s */ 
    }
}

/**************************** End of File ***********************************/
