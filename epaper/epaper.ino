//#include <AM232X.h>
#include <stdio.h>
#include <Wire.h>
#include "EPD_1in9.h"
void blink (byte cnt) {
  delay(200);
  for (byte i = 0; i<cnt; i++){
    digitalWrite(4,HIGH);
    delay(200);
    digitalWrite(4,LOW);
    delay(100);
  }
}
void setup()
{
  pinMode(4,OUTPUT);
  // put your setup code here, to run once:
  blink(3);
  Wire.begin();
  GPIOInit();
  blink(1);
  EPD_1in9_init();
  blink(2);
  EPD_1in9_lut_5S();
  blink(3);
  EPD_1in9_Write_Screen(DSPNUM_1in9_off);
  delay(500);
  EPD_1in9_lut_GC();
  EPD_1in9_Write_Screen1(DSPNUM_1in9_on);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_off);
  delay(500);
  EPD_1in9_lut_DU_WB();
  EPD_1in9_Write_Screen(DSPNUM_1in9_W0);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W1);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W2);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W3);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W4);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W5);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W6);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W7);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W8);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_W9);
  delay(500);
  EPD_1in9_Write_Screen(DSPNUM_1in9_WB);
  delay(500);

  EPD_1in9_sleep();

  delay(1000);
  update_epaper(123.4, 34.5, true, false);
  Wire.end();
}


void loop()
{

}
//---------epaper driver, credit to upiir
char digit_left[] = {0xbf, 0x00, 0xfd, 0xf5, 0x47, 0xf7, 0xff, 0x21, 0xff, 0xf7, 0x00};  // individual segments for the left part od the digit, index 10 is empty
char digit_right[] ={0x1f, 0x1f, 0x17, 0x1f, 0x1f, 0x1d, 0x1d, 0x1f, 0x1f, 0x1f, 0x00};  // individual segments for the right part od the digit, index 10 is empty
char temp_dig[] = {1, 2, 3, 4}; // temperature digits > 1, 2, 3, 4 = 123.4°C
char hum_dig[] = {5, 6, 7}; // humidity digits > 5, 6, 7 = 56.7%
float temp = 1.2; // temperature value - will be used from the sensor
float hum = 3.4; // humidity value - will be used from the sensor
unsigned char eink_segments[]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};  // all white, updated later

void update_epaper(float temp, float hum, bool high_bat, bool low_bat) {
	//GPIOInit();
	//EPD_1in9_init();
  // create digits for temp and hum
  temp_dig[0] = int(temp / 100) % 10;
  temp_dig[1] = int(temp / 10) % 10;
  temp_dig[2] = int(temp ) % 10;
  temp_dig[3] = int(temp * 10) % 10;
  hum_dig[0] = int(hum / 10) % 10;
  hum_dig[1] = int(hum ) % 10;
  hum_dig[2] = int(hum * 10) % 10;
  // update display
  EPD_1in9_lut_5S(); // boot unit to remove ghosting
  EPD_1in9_Write_Screen(DSPNUM_1in9_off); //write all whites
  delay(500);
  EPD_1in9_lut_GC();
  EPD_1in9_lut_DU_WB();
  // do not show leading zeros for values <100 and <10 both temperature and humidity
  if (temp < 100) {temp_dig[0] = 10;}
  if (temp < 10) {temp_dig[1] = 10;}  
  if (hum < 10) {hum_dig[0] = 10;}    
  // temperature digits
  eink_segments[0] = digit_right[temp_dig[0]]; // can only be one for 100+ degrees
  eink_segments[1] = digit_left[temp_dig[1]]; // second digit of the temp, left side
  eink_segments[2] = digit_right[temp_dig[1]];   
  eink_segments[3] = digit_left[temp_dig[2]];  // third digit of the temp
  eink_segments[4] = digit_right[temp_dig[2]] | B00100000 /* decimal point */;   
  eink_segments[11] = digit_left[temp_dig[3]];  // lest temp digit is after the humidity date
  eink_segments[12] = digit_right[temp_dig[3]];    
  // humidity digits
  eink_segments[5] = digit_left[hum_dig[0]];
  eink_segments[6] = digit_right[hum_dig[0]];    
  eink_segments[7] = digit_left[hum_dig[1]];
  eink_segments[8] = digit_right[hum_dig[1]] | B00100000 /* decimal point */;        
  eink_segments[9] = digit_left[hum_dig[2]];
  eink_segments[10] = digit_right[hum_dig[2]] | B00100000 /* percentage sign */;   
  // special symbols - °C / °F, bluetooth, battery
  eink_segments[13] = 0x05; /* °C */ 
  if (high_bat) {eink_segments[13] |=  B00001000; /* bluetooth */}  
  if (low_bat) {eink_segments[13] |= B00010000; /* battery icon */}
  // write segments to the e-ink screen
  EPD_1in9_Write_Screen(eink_segments);
  EPD_1in9_sleep();
}