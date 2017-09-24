#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
void draw(int temp);

#define THERMISTORPIN A0
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000    

uint16_t samples[NUMSAMPLES];

float read_temp();

int heat_pin = 11;

void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  pinMode(heat_pin, OUTPUT);
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
}

void loop(void) 
{
  u8g.firstPage();  
  do 
  {
    int curr_temp = read_temp();
    draw(curr_temp);
    if(curr_temp >= 30.0)
      digitalWrite(heat_pin, LOW);
    else
      digitalWrite(heat_pin, HIGH);
  } while( u8g.nextPage() );
  
  delay(50);
}

void draw(int temp){
  char out_temp[6];
  sprintf(out_temp, "%d", temp);
  u8g.setFont(u8g_font_fub30);
  u8g.drawStr( 30, 57, out_temp);
  u8g.drawCircle(97,28,5);
  u8g.drawStr( 100, 57, "C");
  //moldura relogio
  u8g.drawRFrame(0,18, 128, 46, 4);
  if(temp > 30.0){
    u8g.setFont(u8g_font_8x13B);
    u8g.drawStr( 5, 15, "Pronto!");
  }
}
float read_temp(){
  uint8_t i;
  float average;
  
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
   
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
  
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
   
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
    
  delay(10);
  return steinhart;
}
