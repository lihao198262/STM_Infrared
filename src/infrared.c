
#include "infrared.h"


/*******************************************************************************
 * 名称: TIM1_PWM_Init
 * 功能: TIM1初始化函数 用作PWM输出 38khz
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void TIM1_PWM_Init(void)
{ 
  TIM1_DeInit();
  TIM1_TimeBaseInit(1-1, TIM1_COUNTERMODE_UP, 421, 0x00);       //  2kHz  (8000*1)/16000000
  
#ifdef CHANNEL1
  TIM1_OC1Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE,
               50, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET,
              TIM1_OCNIDLESTATE_RESET); 
#endif
  
#ifdef CHANNEL4
  TIM1_OC4Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, 50, TIM1_OCPOLARITY_LOW, TIM1_OCIDLESTATE_RESET);
  TIM1_CCxCmd(TIM1_CHANNEL_4, ENABLE); 
  TIM1_OC4PreloadConfig(ENABLE);
#endif

  TIM1_Cmd(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);
}

/*******************************************************************************
 * 名称: Infrared_Send
 * 功能: 红外发送的开关开关
 * 形参: signed char status
 * 返回: 无
 * 说明: TRUE 打开 FALSE 关闭 
 ******************************************************************************/
void Infrared_Send_Status(bool status)
{
  //打开PWM
  if(status)
  {
    TIM1_CtrlPWMOutputs(ENABLE);
  }
  //关闭PWM
  else
  {
    TIM1_CtrlPWMOutputs(DISABLE);
  }
}

#define MAKR            Infrared_Send_Status(TRUE);
#define SPACE           Infrared_Send_Status(FALSE);
#define D9000US         Delay_50Us(180);
#define D4500US         Delay_50Us(90);
#define D550US          Delay_50Us(11);
#define D1700US         Delay_50Us(34);
#define D250US          Delay_50Us(5);
/*******************************************************************************
 * 名称: Infrared_Send
 * 功能: 红外发射
 * 形参: unsigned long data
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Infrared_Send(unsigned long data)
{
  
  uint8_t i = 0;
  
  
  SPACE 
  
  MAKR  D9000US
  SPACE D4500US
  
  for(i = 0; i < 32; i++)
  {
    MAKR        D550US
    if((data & 0x80000000) == 0)
    {
      SPACE     D550US
    }
    else
    {
      SPACE     D1700US 
    }
    
    data <<= 1;
  }
  
  MAKR   D250US
  SPACE
}

#define NEC_HDR_MARK	Infrared_Send_Status(TRUE);Delay_50Us(180);
#define NEC_HDR_SPACE	Infrared_Send_Status(FALSE);Delay_50Us(90);
#define NEC_BIT_MARK	Infrared_Send_Status(TRUE);Delay_50Us(11);
#define NEC_ONE_SPACE	Infrared_Send_Status(FALSE);Delay_50Us(34);
#define NEC_ZERO_SPACE	Infrared_Send_Status(FALSE);Delay_50Us(11);
#define NEC_RPT_SPACE	Infrared_Send_Status(FALSE);Delay_50Us(45);


/*******************************************************************************
 * 名称: NEC_Infrared_Send
 * 功能: 红外发射 类型NEC
 * 形参: unsigned long data
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void NEC_Infrared_Send(unsigned long data)
{
  NEC_HDR_MARK
  NEC_HDR_SPACE
  for (int i = 0; i < 32; i++) {
    if (data & 0x80000000) {
      NEC_BIT_MARK
      NEC_ONE_SPACE
    } 
    else {
      NEC_BIT_MARK
      NEC_ZERO_SPACE
    }
    data <<= 1;
  }
  NEC_BIT_MARK
  Infrared_Send_Status(FALSE);
}


#define HAIER_HDR_MARK          Infrared_Send_Status(TRUE);     Delay_50Us(60);
#define HAIER_HDR_SPACE         Infrared_Send_Status(FALSE);    Delay_50Us(60);
#define HAIER_HDR_SPACE2        Infrared_Send_Status(FALSE);    Delay_50Us(90);
#define HAIER_BIT_MARK          Infrared_Send_Status(TRUE);     Delay_50Us(12);
#define HAIER_ONE_SPACE         Infrared_Send_Status(FALSE);    Delay_50Us(33);
#define HAIER_ZERO_SPACE	Infrared_Send_Status(FALSE);    Delay_50Us(12);
/*******************************************************************************
 * 名称: Haier_Infrared_Send
 * 功能: 红外发射 类型海尔
 * 形参: unsigned long data
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Haier_Infrared_Send(uint8_t data[], int len)
{
  HAIER_HDR_MARK
  HAIER_HDR_SPACE
  HAIER_HDR_MARK
  HAIER_HDR_SPACE2
    
  for(int i=0; i<len; i++)
  {
    uint8_t temp = data[i];
    for(int j=0; j<8; j++)
    {
      if (temp & 0x80) {
        HAIER_BIT_MARK
        HAIER_ONE_SPACE
      } 
      else 
      {
        HAIER_BIT_MARK
        HAIER_ZERO_SPACE
      }
      temp <<= 1;
    }
  }
  
  HAIER_BIT_MARK
  Infrared_Send_Status(FALSE);
}

u32 TimingDelay; 

/*******************************************************************************
 * 名称: TIM2_Time_Init
 * 功能: TIM2初始化操作 用作休眠计时
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void TIM2_Time_Init(void)
{
  //1分频，向上计数，每50us定时中断一次， 重复计数器值为0 
  TIM2_TimeBaseInit(TIM2_PRESCALER_1,800);
  TIM2_SetCounter(0);                           /* 将计数器初值设为0 */
  TIM2_ARRPreloadConfig(DISABLE);	        /* 预装载不使能 */
  TIM2_ITConfig(TIM2_IT_UPDATE , ENABLE);	/* 计数器向上计数/向下计数溢出更新中断 */
  TIM2_Cmd(ENABLE);			        /* 使能TIM1 */
}

/*******************************************************************************
 * 名称: delay_ms
 * 功能: 利用TIM1产生的1ms中断来计时
 * 形参: nms -> 计时值(ms)
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Delay_50Us(u32 nTime)
{
  TimingDelay = nTime;
  while(0 != TimingDelay)
    ;
}


INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{ 
  TimingDelay--;
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
}


/*******************************************************************************
 * 名称: Infrared_Init()
 * 功能: 初始化函数
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Infrared_Init(void)
{
  TIM1_PWM_Init();

  TIM2_Time_Init();
  
  enableInterrupts(); 
}

