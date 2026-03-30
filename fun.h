#ifndef _fun_h
#define _fun_h

void led_show(uint8_t led,uint8_t mode);
void lcd_show();
double adc_vt(ADC_HandleTypeDef* hadc);
void key_scan();
void eeprom_write(uint8_t addr,uint8_t data);
uint8_t eeprom_read(uint8_t addr);
void main_process();

#endif
