
#include "stm8s.h"

#include "infrared.h"

int main( void )
{
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1); 
  
  Infrared_Init();
  
  //uint8_t data[] = {0xA6,0x12,0x00,0x02,0x40,0x40,0x00,0x20,0x00,0x00,0x00,0x00,0x84, 0xDE};
  while(1)
  { 
    // unsigned long b = 0x00FF00FFL;
    // NEC_Infrared_Send(b);
    Media_Infrared_Send(0xB2, 0xBF, 0x40);
    // Haier_Infrared_Send(data, 14);
    Delay_50Us(40000); // sleep 1s
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
