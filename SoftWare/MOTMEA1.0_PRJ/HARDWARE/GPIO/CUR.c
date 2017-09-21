#include "CUR.h"

u16 cur_data;

 /**
* @ Function Name :  GPIO_CurMeasure_Init
* @ Author        : ygl
* @ Brief         : 电流采集引脚初始化
* @ Date          : 2017.09.19
* @ Modify        : 
 **/
void GPIO_CurMeasure_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_AFIO|RCC_APB2Periph_ADC3,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M  最大14M 设置ADC时钟（ADCCLK）
	ADC_DeInit(ADC3);

	GPIO_InitStructure.GPIO_Pin=CUR_SIM;//ADC  //1
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//模拟输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);	  //A


	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 1; 
	ADC_Init(ADC3, &ADC_InitStructure);
	
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	ADC_RegularChannelConfig(ADC3,ADC_Channel_4,1,ADC_SampleTime_239Cycles5); //1
	
	//内部温度传感器是在ADC1通道16的。
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_239Cycles5);
//	ADC_TempSensorVrefintCmd(ENABLE);//打开内部温度传感器使能

	ADC_DMACmd(ADC3,ENABLE);//将ADC与DMA链接在一起
	ADC_Cmd(ADC3,ENABLE);	

	ADC_ResetCalibration(ADC3);//重置指定的ADC的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC3));//获取ADC重置校准寄存器的状态	
	ADC_StartCalibration(ADC3);//开始指定ADC的校准状态
	while(ADC_GetCalibrationStatus(ADC3));//获取指定ADC的校准程序
	ADC_SoftwareStartConvCmd(ADC3, ENABLE);//使能或者失能指定的ADC的软件转换启动功能

	DMA_DeInit(DMA2_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&ADC3->DR;//DMA外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)&cur_data;//DMA内存地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//外设作为数据传输的来源
	DMA_InitStructure.DMA_BufferSize=1;//指定DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址寄存器递增
	DMA_InitStructure.DMA_MemoryInc=DMA_PeripheralInc_Enable;//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//外设数据宽度16
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//存储数据宽度16
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;//工作在循环缓存模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//DMA通道x拥有高优先级
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA2_Channel5,&DMA_InitStructure);	//ADC1在DMA1通道1内
	DMA_Cmd(DMA2_Channel5,ENABLE);//使能DMA1
	
}
