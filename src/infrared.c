
#include "infrared.h"


/*******************************************************************************
 * ����: TIM1_PWM_Init
 * ����: TIM1��ʼ������ ����PWM��� 38khz
 * �β�: ��
 * ����: ��
 * ˵��: �� 
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
 * ����: Infrared_Send
 * ����: ���ⷢ�͵Ŀ��ؿ���
 * �β�: signed char status
 * ����: ��
 * ˵��: TRUE �� FALSE �ر� 
 ******************************************************************************/
void Infrared_Send_Status(bool status)
{
  //��PWM
  if(status)
  {
    TIM1_CtrlPWMOutputs(ENABLE);
  }
  //�ر�PWM
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
 * ����: Infrared_Send
 * ����: ���ⷢ��
 * �β�: unsigned long data
 * ����: ��
 * ˵��: �� 
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
 * ����: NEC_Infrared_Send
 * ����: ���ⷢ�� ����NEC
 * �β�: unsigned long data
 * ����: ��
 * ˵��: �� 
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
 * ����: Haier_Infrared_Send
 * ����: ���ⷢ�� ���ͺ���
 * �β�: unsigned long data
 * ����: ��
 * ˵��: �� 
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
 * ����: TIM2_Time_Init
 * ����: TIM2��ʼ������ �������߼�ʱ
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void TIM2_Time_Init(void)
{
  //1��Ƶ�����ϼ�����ÿ50us��ʱ�ж�һ�Σ� �ظ�������ֵΪ0 
  TIM2_TimeBaseInit(TIM2_PRESCALER_1,800);
  TIM2_SetCounter(0);                           /* ����������ֵ��Ϊ0 */
  TIM2_ARRPreloadConfig(DISABLE);	        /* Ԥװ�ز�ʹ�� */
  TIM2_ITConfig(TIM2_IT_UPDATE , ENABLE);	/* ���������ϼ���/���¼�����������ж� */
  TIM2_Cmd(ENABLE);			        /* ʹ��TIM1 */
}

/*******************************************************************************
 * ����: delay_ms
 * ����: ����TIM1������1ms�ж�����ʱ
 * �β�: nms -> ��ʱֵ(ms)
 * ����: ��
 * ˵��: �� 
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
 * ����: Infrared_Init()
 * ����: ��ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Infrared_Init(void)
{
  TIM1_PWM_Init();

  TIM2_Time_Init();
  
  enableInterrupts(); 
}

