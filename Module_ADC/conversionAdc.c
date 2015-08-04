
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "conversionAdc.h"

double conversionAdc()
{
   double Adc_count;
   ADC0_PSSI_R |= ADC_PSSI_SS0;                //Start SS0
   //while((ADC0_ACTSS_R & ADC_ACTSS_BUSY) == 0){};
   while(ADC0_RIS_R & ADC_RIS_INR0 == 0){};      //A/D conversion wait-time
   Adc_count = ADC0_SSFIFO0_R & 0xFFF;  //Read Adc count from SS0FIFO
   Adc_count = (3.3*Adc_count)/4096;
   ADC0_ISC_R = 0x1;                    //Set ISC to indicate converted data has been sampled
   return Adc_count;

}

