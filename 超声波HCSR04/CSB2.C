#include "CSB2.h"
#include "Systick.h"
#include "usart.h"
 void csb2_Init(void)
{


	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
	

	//????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(GPIOE, &GPIO_InitStructure);	               
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化外设GPIO 
	//????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					 //PC2接TRIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化外设GPIO 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	

	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//管脚设置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;	 //设置下拉输入模式
	GPIO_Init(GPIOC,&GPIO_InitStructure); 	   /* 初始化GPIO */
	
	TIM_TimeBaseInitStructure.TIM_Period=49999;   //自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=71; //分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);	
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1; //通道1
	TIM_ICInitStructure.TIM_ICFilter=0x00;  //滤波
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//捕获极性
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1; //分频系数
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//直接映射到TI1
	TIM_ICInit(TIM8,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2; //通道2
	TIM_ICInit(TIM8,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_3; //通道3
	TIM_ICInit(TIM8,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_4; //通道4
	TIM_ICInit(TIM8,&TIM_ICInitStructure);
	
	TIM_ITConfig(TIM8,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;//中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
		
	TIM_Cmd(TIM8,ENABLE); //使能定时器
	

	
	

	
}
u8  TIM8CH1_CAPTURE_STA=0;	//输入捕获状态
u16	TIM8CH1_CAPTURE_VAL;	//输入捕获值

u8  TIM8CH2_CAPTURE_STA=0;    				
u16	TIM8CH2_CAPTURE_VAL;	

u8  TIM8CH3_CAPTURE_STA=0;		    				
u16	TIM8CH3_CAPTURE_VAL;

u8  TIM8CH4_CAPTURE_STA=0;			    				
u16	TIM8CH4_CAPTURE_VAL;	


//??????????
void TIM8_CC_IRQHandler(void)
{ 
	
//通道1捕获中断函数
 	if((TIM8CH1_CAPTURE_STA&0X80)==0)//还未捕获成功
	{	  
	
		if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET) //开始捕获但还未捕获完成
		{	    
			
			if(TIM8CH1_CAPTURE_STA&0X40)//捕获到高电平
			{
				if((TIM8CH1_CAPTURE_STA&0X3F)==0X3F)//高电平时间过长
				{
				
					TIM8CH1_CAPTURE_STA|=0X80;//标记成功捕获一次
					TIM8CH1_CAPTURE_VAL=0XFFFF;
					
				}
				else  TIM8CH1_CAPTURE_STA++;  //一次溢出的时间
			}	 
		}
//		flag=TIM_GetITStatus(TIM3, TIM_IT_CC1);
//		printf("%d",flag);
	if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET)//捕获已发生的捕获事件
			{	
			if(TIM8CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿
			{	  			
				TIM8CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一个下降沿
				TIM8CH1_CAPTURE_VAL=TIM_GetCapture1(TIM8);
		   	TIM_OC1PolarityConfig(TIM8,TIM_ICPolarity_Rising); //设置为上升沿捕获
			}else  								//未开始，第一次捕获到上升沿
			{
				TIM8CH1_CAPTURE_STA=0;			//清零
				TIM8CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM8,0);             //计数器的值设为00
				TIM8CH1_CAPTURE_STA|=0X40;		//捕获到了上升沿
		   	TIM_OC1PolarityConfig(TIM8,TIM_ICPolarity_Falling);		//CC1P=1设置为下降沿捕获
				
			}		    
		}				
 	}
	//ic2
	 	if((TIM8CH2_CAPTURE_STA&0X80)==0)
	{	  
		if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET) 
		{	     
			if(TIM8CH2_CAPTURE_STA&0X40)
			{
				if((TIM8CH2_CAPTURE_STA&0X3F)==0X3F)
				{
					TIM8CH2_CAPTURE_STA|=0X80;
					TIM8CH2_CAPTURE_VAL=0XFFFF;
				}else TIM8CH2_CAPTURE_STA++;    
			}	 
		}
	if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET)  
			{	
			if(TIM8CH2_CAPTURE_STA&0X40)			
			{	  			
				TIM8CH2_CAPTURE_STA|=0X80;	
				TIM8CH2_CAPTURE_VAL=TIM_GetCapture2(TIM8);
		   		TIM_OC2PolarityConfig(TIM8,TIM_ICPolarity_Rising); 
			}else  							
			{
				TIM8CH2_CAPTURE_STA=0;			
				TIM8CH2_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM8,0);       
				TIM8CH2_CAPTURE_STA|=0X40;	
		   	TIM_OC2PolarityConfig(TIM8,TIM_ICPolarity_Falling);	
			}		    
		}				
 	}
//IC3??????
	 	if((TIM8CH3_CAPTURE_STA&0X80)==0)
	{	  
		if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET) 
		{	    
			if(TIM8CH3_CAPTURE_STA&0X40)
			{
				if((TIM8CH3_CAPTURE_STA&0X3F)==0X3F)
				{
					TIM8CH3_CAPTURE_STA|=0X80;
					TIM8CH3_CAPTURE_VAL=0XFFFF;
				}else TIM8CH3_CAPTURE_STA++;    
			}	 
		}
	if (TIM_GetITStatus(TIM8, TIM_IT_CC3) != RESET)
			{	
			if(TIM8CH3_CAPTURE_STA&0X40)	 		
			{	  			
				TIM8CH3_CAPTURE_STA|=0X80;		
				TIM8CH3_CAPTURE_VAL=TIM_GetCapture3(TIM8);
		   	TIM_OC3PolarityConfig(TIM8,TIM_ICPolarity_Rising); 
			}else  							
			{
				TIM8CH3_CAPTURE_STA=0;	
				TIM8CH3_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM8,0);        
				TIM8CH3_CAPTURE_STA|=0X40;	
		   	TIM_OC3PolarityConfig(TIM8,TIM_ICPolarity_Falling);		
			}		    
		}				
 	}
	//4
		if((TIM8CH4_CAPTURE_STA&0X80)==0)
	{	  
		if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET) 
		{	    
			if(TIM8CH4_CAPTURE_STA&0X40)
			{
				if((TIM8CH4_CAPTURE_STA&0X3F)==0X3F)
				{
					TIM8CH4_CAPTURE_STA|=0X80;
					TIM8CH4_CAPTURE_VAL=0XFFFF;
				}else TIM8CH4_CAPTURE_STA++;    
			}	 
		}
	if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET)
			{	
			if(TIM8CH4_CAPTURE_STA&0X40)	 		
			{	  			
				TIM8CH4_CAPTURE_STA|=0X80;		
				TIM8CH4_CAPTURE_VAL=TIM_GetCapture4(TIM8);
		   	TIM_OC4PolarityConfig(TIM8,TIM_ICPolarity_Rising); 
			}else  							
			{
				TIM8CH4_CAPTURE_STA=0;	
				TIM8CH4_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM8,0);        
				TIM8CH4_CAPTURE_STA|=0X40;	
		   	TIM_OC4PolarityConfig(TIM8,TIM_ICPolarity_Falling);		
			}		    
		}				
 	}
	
//清零中断标志位
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update); 
 		TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);  
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC3);
    TIM_ClearITPendingBit(TIM8, TIM_IT_CC4); 	 	
}
float csblenth_5; //长度 
float csb_5(void)
{
	
		u32 temp=0;
			delay_ms(5);
		
		if(TIM8CH1_CAPTURE_STA&0X80)//已经成功捕获到一次上升沿
		{
			temp=TIM8CH1_CAPTURE_STA&0X3F;
			temp*=65536;//溢出时间的总和
			temp+=TIM8CH1_CAPTURE_VAL;//得到总的高电平时间us
			csblenth_5 = temp*340/2000; //单位mm
			TIM8CH1_CAPTURE_STA=0;//开启下一次捕获
		}
		//printf("%f\r\n",csblenth_1);
		return csblenth_5;
}
//超声波2的测距换算
float csblenth_6; 
float csb_6(void)
{
	
		u32 temp=0;
		delay_ms(5);
		
		if(TIM8CH2_CAPTURE_STA&0X80)
		{
			temp=TIM8CH2_CAPTURE_STA&0X3F;
			temp*=65536;
			temp+=TIM8CH2_CAPTURE_VAL;

			csblenth_6 = temp*340/2000;
			TIM8CH2_CAPTURE_STA=0;
		}
		
		return csblenth_6 ;//50为机械误差
}
//超声波三
float csblenth_7;
float csb_7(void)
{
	
		u32 temp=0;
			delay_ms(5);		
		if(TIM8CH3_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
			temp=TIM8CH3_CAPTURE_STA&0X3F;
			temp*=65536;//溢出时间总和（溢出一次计数65536）
			temp+=TIM8CH3_CAPTURE_VAL;//得到总的高电平时间 us
			//公式为  长度=高电平时间*声速/2
			csblenth_7 = temp*340/2000; //单位为mm
//			printf("HIGH:%d us\r\n",temp);//打印总的高点平时间
			TIM8CH3_CAPTURE_STA=0;//开启下一次捕获
		}
		return csblenth_7;//54为机械误差
}
//超声波四
float csblenth_8;
float csb_8(void)
{
	
		u32 temp=0;
		delay_ms(5);
		
		if(TIM8CH4_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
			temp=TIM8CH4_CAPTURE_STA&0X3F;
			temp*=65536;//溢出时间总和（溢出一次计数65536）
			temp+=TIM8CH4_CAPTURE_VAL;//得到总的高电平时间 us
			//公式为  长度=高电平时间*声速/2
			csblenth_8 = temp*340/2000; //单位为mm
//			printf("HIGH:%d us\r\n",temp);//打印总的高点平时间
			TIM8CH4_CAPTURE_STA=0;//开启下一次捕获
		}

		return csblenth_8;//54为机械误差
		
}


void TIM8TRIG_1(void)
{

  GPIO_SetBits(GPIOE,GPIO_Pin_4); 		 
  delay_us(20);		              
  GPIO_ResetBits(GPIOE,GPIO_Pin_4);
	//delay_us(30);

	
}
void TIM8TRIG_2(void)
{	

	GPIO_SetBits(GPIOE,GPIO_Pin_5); 		
  delay_us(20);		               
  GPIO_ResetBits(GPIOE,GPIO_Pin_5);
	//delay_us(30);

	
}
void TIM8TRIG_3(void)
{

	GPIO_SetBits(GPIOE,GPIO_Pin_6); 	
  delay_us(20);		                  
  GPIO_ResetBits(GPIOE,GPIO_Pin_6);
	//delay_us(30);
}

void TIM8TRIG_4(void)
{

	GPIO_SetBits(GPIOE,GPIO_Pin_7); 	
  delay_us(20);		                  
  GPIO_ResetBits(GPIOE,GPIO_Pin_7);
	//delay_us(30);
}
