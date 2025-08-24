#include "stm32f10x.h"

uint16_t AudioSample[256];
void TIM1_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 		//Ê±ÖÓÊ¹ÄÜ

	//¶¨Ê±Æ÷TIM2³õÊ¼»¯
	TIM_TimeBaseStructure.TIM_Period = arr; 		//ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 			//ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);			//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = 50;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//Êä³ö¼«ÐÔ:TIMÊä³ö±È½Ï¼«ÐÔµÍ
	TIM_OC2Init(TIM1, & TIM_OCInitStructure);		//³õÊ¼»¯ÍâÉèTIM2_CH2
	
	TIM_Cmd(TIM1, ENABLE); 			//Ê¹ÄÜTIMx
	TIM_CtrlPWMOutputs(TIM1, ENABLE); 
}

void Sample_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;            //GPIOÅäÖÃ½á¹¹Ìå¶¨Òå
	ADC_InitTypeDef ADC_InitStructure;              //ADCÅäÖÃ½á¹¹Ìå¶¨Òå
	DMA_InitTypeDef DMA_InitStructure;              //DMAÅäÖÃ½á¹¹Ìå¶¨Òå
	NVIC_InitTypeDef NVIC_InitStructure;            //NVICÅäÖÃ½á¹¹Ìå¶¨Òå
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	        //Ê¹ÄÜGPIOAÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	        //Ê¹ÄÜADC1Í¨µÀÊ±ÖÓ
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	  			//Ê¹ÄÜADC1Í¨µÀÊ±ÖÓ
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                      //ADC²ÉÑù£¬¹ÊÐèÒªÄ£ÄâÊäÈë
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
	//ADC1³õÊ¼»¯
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 		        	//¶ÀÁ¢ADCÄ£Ê½
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  	            		//¹Ø±ÕÉ¨Ãè·½Ê½
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	        		//¹Ø±ÕÁ¬Ðø×ª»»Ä£Ê½
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC2;//´¥·¢Ô´Ñ¡Ôñ¶¨Ê±Æ÷1µÄÍ¨µÀ¶þ´¥·¢
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	    		//²É¼¯Êý¾ÝÓÒ¶ÔÆë
	ADC_InitStructure.ADC_NbrOfChannel = 1; 			                //Òª×ª»»µÄÍ¨µÀÊýÄ¿
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);				                    //ÅäÖÃADCÊ±ÖÓ£¬ÎªPCLK2µÄ6·ÖÆµ£¬72M/6=12M,×îºÃ²»Òª³¬¹ý14M
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);//ÅäÖÃADC1Í¨µÀ6Îª1.5¸ö²ÉÑùÖÜÆÚ,Êý¾Ý½Ï¶à£¬ÒÔ×î¿ìËÙ¶È²ÉÑù 
	
	ADC_DMACmd(ADC1,ENABLE);//Ê¹ÄÜDMA×ªÔËADC1µÄÊý¾Ý
	ADC_Cmd(ADC1,ENABLE);   //Ê¹ÄÜADC1
 
	ADC_ResetCalibration(ADC1);			                	//¸´Î»Ð£×¼¼Ä´æÆ÷
	while(ADC_GetResetCalibrationStatus(ADC1));				//µÈ´ýÐ£×¼¼Ä´æÆ÷¸´Î»Íê³É
 
	ADC_StartCalibration(ADC1);			                	//ADCÐ£×¼
	while(ADC_GetCalibrationStatus(ADC1));	    			//µÈ´ýÐ£×¼Íê³É
	
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);	            	//ÉèÖÃÍâ²¿´¥·¢Ä£Ê½Ê¹ÄÜ
	
	//DMA1³õÊ¼»¯
	DMA_DeInit(DMA1_Channel1);                   //ÅäÖÃDMAÇ°ÐèÒªÏÈ¹Ø±ÕDMA£¬ÒòÎª±¾ÊµÑéÖ»³õÊ¼»¯Ò»´Î£¬ÔÚ±¾ÊµÑéÖÐÊµ²âÉ¾È¥Ó°Ïì²»´ó
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;				//ADC1µØÖ·
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AudioSample; 		            //ÄÚ´æµØÖ·
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				            //·½Ïò(´ÓÍâÉèµ½ÄÚ´æ)
	DMA_InitStructure.DMA_BufferSize = 256; 						            //´«ÊäÄÚÈÝµÄ´óÐ¡
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	    	//ÍâÉèµØÖ·¹Ì¶¨½öÓÐÒ»¸ö´«¸ÐÆ÷
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			        //ÄÚ´æµØÖ·×ÔÔö£¬ÐèÒª256¸öÊý¾Ý
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; //ÒÔ°ë×Ö½Ú³¤¶È½øÐÐ´«Êä£¨16Î»£©
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;         //ÒÔ°ë×Ö½Ú³¤¶È½øÐÐ´«Êä£¨16Î»£©
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; 		                    //Ñ­»·´«ÊäÄ£Ê½
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ; 	            	    //ÓÅÏÈ¼¶°´ÐèÖ¸¶¨
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		                        //Ê§ÄÜÄÚ´æµ½ÄÚ´æµÄ´«Êä
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);		//Ê¹ÄÜ´«ÊäÍê³ÉÖÐ¶Ï
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
    
    TIM1_Init(99,71);//72000000/7200=10000Hz£¬Ã¿100us²É¼¯Ò»´Î
}
