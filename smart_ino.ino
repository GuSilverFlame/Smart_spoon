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

void draw_temp(int temp);
void draw_wait();
void power_off();

#define THERMISTORPIN A0
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000    

uint16_t samples[NUMSAMPLES];

float read_temp();

int heat_pin = 10;

int interrupto_pin = 7;
int powr_flag = 1;
int heating_flag = 0;

void setup(void) {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  pinMode(interrupto_pin, INPUT);
  analogReference(EXTERNAL);
  pinMode(heat_pin, OUTPUT);
}

void loop(void) 
{
  if(powr_flag > 0){
    display.clearDisplay();
    int curr_temp = read_temp();
    if(powr_flag == 1)
      draw_wait(curr_temp);
    else
      draw_temp();
    if(curr_temp <= 50.0){
      heating_flag = 1;
      digitalWrite(heat_pin, HIGH);
    }
    else{
      heating_flag = 0;
      digitalWrite(heat_pin, LOW);
    }
    delay(50);
  }
  else{
    turn_off();
  }
  display.display();
  if(digitalRead(interrupto_pin)==HIGH)
    powerCycle();

}

void powerCycle(){
  int timer = 0;
  int state = digitalRead(interrupto_pin) == HIGH ? 1 : 0;
  while (state) {
    delay(100);
    timer++;
    for(int i = 0;i< NUMSAMPLES && state == 1; i++){
      Serial.println(state);
      state = digitalRead(interrupto_pin) == HIGH ? 1 : 0;
    }
    if(timer > 10){
      turn_off();
      display.display();
    }
  }
  if (timer < 10) {
    if(powr_flag == 2 || powr_flag == 0)
      powr_flag = 1;
    else
      powr_flag = 2;
  }
  else
    powr_flag = 0;
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
  display.drawRect(0, 0, 5, 64, WHITE);
  if(temp >= 25){
    int progress = (((temp-25)*100)/25)*0.64;
    display.fillRect(1, 64 - progress, 4, progress , WHITE);
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
void turn_off(){
  display.clearDisplay();
  if(heating_flag){
    heating_flag = 0;
    digitalWrite(heat_pin, LOW);
  }
}

