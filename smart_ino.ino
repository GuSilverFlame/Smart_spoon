#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void draw(int temp);

#define THERMISTORPIN A0
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000    

uint16_t samples[NUMSAMPLES];

float read_temp();

int heat_pin = 10;

void setup(void) {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  analogReference(EXTERNAL);
  pinMode(heat_pin, OUTPUT);
}

void loop(void) 
{
  display.clearDisplay();
  int curr_temp = read_temp();
  draw(curr_temp);
  display.display();
  if(curr_temp <= 50.0)
    digitalWrite(heat_pin, HIGH);
  else
    digitalWrite(heat_pin, LOW);
  delay(50);
}

void draw(int temp){
  char out_temp[6];
  sprintf(out_temp, "%d C", temp);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(25,20);
  display.clearDisplay();
  display.println(out_temp);
  display.drawCircle(65, 20, 4, WHITE);
  display.drawRect(20, 15, 80, 30, WHITE);
  Serial.print(out_temp);
  if(temp >= 50.0){
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(8,1);
    display.println("Pode se servir!");
  }
  display.display();
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
  Serial.print("Average analog reading ");
  Serial.println(average);
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance ");
  Serial.println(average);

  float steinhart;
  steinhart = average / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
    
  delay(10);

  Serial.print("Temperature ");
  Serial.print(steinhart);
  Serial.println(" *C");

  return steinhart;
}
