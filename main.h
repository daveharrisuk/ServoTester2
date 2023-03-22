/**@headerfile main.h        PROJECT_NAME "ServoTester2"
*****************************************************************************

@author ©2023 Dave Harris, Andover, UK.  (MERG member 'WortingUK')
@copyright Creative Commons BY-NC-SA (Attribution-NonCommercial-ShareAlike)

@version 0.1
@history 23-Nov-2022 DH started
@history 26-Jan-2023 DH Stable version
*/

#pragma once /* header include guard */

/****************************************************************************
@brief include/libraries
*/

#include "mcc_generated_files/mcc.h" /* MPLAB Code Configurator generated */


/****************************************************************************
@brief  Data type, enum and constants
*/

enum blink_t 
{
    BLINK_OFF, /* Fault */
    BLINK_ON,  /* current LT 2mA */ 
};

enum phase_t 
{
    PHASE_MINUS,
    PHASE_PLUS, 
    PHASE_CENTER
};

enum sw1_t
{
    SW1_RUN = 1,
    SW1_HOLD = 0
};

enum adPref_t {ADPREF_VDD, ADPREF_FVR};

typedef uint8_t count_dc_t;

typedef uint8_t count_20ms_t;

/* typedef uint16_t adc_result_t // is from adc.h */

const count_dc_t DC_1point5ms = 150; /* 1.5ms mid travel at 10us intervals */

/* RSENSE = 0.5ohm, Volt/mA is 0.5mV  */
enum mA_threshold_t /* one ADC bit = 1mV, with ADC ref 1.024V */
{
    mA_2 = 1, /* RSENSE x 2mA = 1  - minimum detectable value */
    mA_10 = 5, /* RSENSE x 10mA = 5  */
    mA_20 = 10, /* RSENSE x 20mA = 10  */
    mA_25 = 12,
    mA_30 = 15,
    mA_40 = 20,
    mA_50 = 25,
    mA_100 = 50,
    mA_500 = 250,
    mA_600 = 300,
    mA_700 = 350,
    mA_800 = 400,
    mA_900 = 450,
    mA_1000 = 500,  /* RSENSE x 1000mA = 500  */
};

enum VddThres_t /* VDD measure = ADC of FVR chan & ADPREF is variable VDD */
{
    VDDTHRES_4_9 = 214, /* = (1024 x 1.024) / VDD = 1048.576 / 4.9 = 213.99 */
    VDDTHRES_4_8 = 218,
    VDDTHRES_4_7 = 223,
    VDDTHRES_4_6 = 228,
    VDDTHRES_4_5 = 233,
    VDDTHRES_4_4 = 238,
};


/****************************************************************************
@brief function prototypes
*/

void main(void); /* Power up code */

void PWM_TMR0_ISR(void); /* PWM generator, on TMR0 overflow every 10us */

void processServo(enum phase_t phase, count_20ms_t duration20ms);

count_dc_t readPotDCcount(void); /* Read the pot value (0 to 100) */

adc_result_t readSenseAmps(void); /* Read the current sense resistor */

void testForVDDfault(void); /* Fault indicate if VDD is low */

void setAdcPosRefVolt(enum adPref_t adPosRef); /* set ADC + ref Volt */

void decodeAmpsToLED(adc_result_t mA); /* decode mA to LED states */

void yellowBlink(enum blink_t blink); /* blink or flash LED  */


/****************************************************************************
@brief Global variables
*/

volatile count_dc_t g_pwmDCcount = 150; /* target PWM duty cycle */ 
                                        /* 150x10us =1.50ms mid travel pulse */
                                        /* limits: 50 = 0.5ms to 250 =2.50ms */

volatile count_20ms_t g_countIsr20ms = 0;   /* 0 to 255. 255 x 20ms = 5.1s */


volatile uint8_t g_pwmDoneFlag;             /* PWM pulse complete flag. */


/**************************** End of File **********************************/
