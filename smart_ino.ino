
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
  analogReference(EXTERNAL);
  pinMode(heat_pin, OUTPUT);
}

void loop(void) 
{
  int curr_temp = read_temp();
  draw(curr_temp);
  if(curr_temp >= 30.0)
    digitalWrite(heat_pin, HIGH);
  else
    digitalWrite(heat_pin, LOW);
  delay(50);
}

void draw(int temp){
  char out_temp[6];
  sprintf(out_temp, "%dºC\n", temp);
  Serial.print(out_temp);
  if(temp > 30.0){
    Serial.print("Pronto!");
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
