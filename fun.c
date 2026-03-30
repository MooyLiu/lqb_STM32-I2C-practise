#include "headfile.h"

uint8_t led_mode;
char text[20];
int count;
uint32_t fre1,fre2;

void led_show(uint8_t led,uint8_t mode){

	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if(mode){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_RESET);
	}
	else{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	
}

uint8_t b1now,b2now,b3now,b4now;
uint8_t b1last=1,b2last=1,b3last=1,b4last=1;
void key_scan(){

	b1now=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	b2now=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	b3now=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	b4now=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	if(b1now==0&&b1last==1){
		TIM4->CNT=0;
	}
	else if(b1now==0&&b1last==0){
		if(TIM4->CNT>=10000){
			count++;
		}
	}
	else if(b1now==1&&b1last==0){
		if(TIM4->CNT<10000){
			count=count+2;
		}
	}
	if(b2now==0&&b2last==1){
		led_show(1,0);
	}
	if(b3now==0&&b3last==1){
		led_show(3,1);
	}
	if(b4now==0&&b4last==1){
		led_show(3,0);
	}
	b1last=b1now;
	b2last=b2now;
	b3last=b3now;
	b4last=b4now;
	
}

void lcd_show(){

	uint16_t temp=GPIOC->ODR;
	sprintf(text,"  hello:%.2f %.2f ",(double)fre1,(double)fre2);
	LCD_DisplayStringLine(Line2,(uint8_t *)text);
	GPIOC->ODR=temp;

}

double adc_vt(ADC_HandleTypeDef* hadc){

	HAL_ADC_Start(hadc);
	uint32_t adc_value=HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4030;

}

void eeprom_write(uint8_t addr,uint8_t data){

	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(data);
	I2CWaitAck();
	I2CStop();
	
	HAL_Delay(50);

}

uint8_t eeprom_read(uint8_t addr){

	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	uint8_t dat=I2CReceiveByte();
	I2CSendNotAck();
	I2CStop();
	return dat;

}

void main_process(){

	led_show(3,led_mode);
	lcd_show();

}

/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance==TIM4){
		led_mode=!led_mode;
	}

}*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance==TIM2){
		uint32_t pwm_value1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		TIM2->CNT=0;
		fre1=80000000/(80*pwm_value1);
	}
	if(htim->Instance==TIM3){
		uint32_t pwm_value2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		TIM3->CNT=0;
		fre2=80000000/(80*pwm_value2);
	}

}

