#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

I2CSoilMoistureSensor sensor;
int delaytime = 10;//time between two measurements in ms
int measurecount = 5;//how many measure are taken for the average
int dry; //used to toggle on the pump
int wet; //used to toggle off the pump
boolean pump;//determens wether the pump should be on or off
int pin;//pinNumber used for ScienceFair
int looptime = 50;//time between to measurements in ms

void setup() {
  Wire.begin();
  Serial.begin(9600);

  sensor.begin(); // reset sensor
  delay(1000); // boot up
  Serial.println("\nstartup");

  pump = false;

  //ScinceFair only
  pinMode(pin, OUTPUT);
}

void loop() {
  while (sensor.isBusy()) delay(50); // available since FW 2.3
  //Serial.print("Feuchtigkeit: ");
  //Serial.print(getCapacitance(delaytime,measurecount)); //read capacitance register
  //Serial.print(", Temperatur: ");
  //Serial.println(getTemperature(delaytime,measurecount)); //temperature register
  //Serial.print(", Belichtung: ");
  //Serial.println(sensor.getLight(true)); //request light measurement, wait and read light register
p();

  //logic:  sensor.slee
  int mois = getCapacitance(delaytime,measurecount);
  if(toodry(mois)){
    pump = true;
  }
  if(toowet(mois)){
    pump = false;
  }
  refreshpump();
  delay(looptime);
}

int getCapacitance(int dly, int mc){
  int r = 0;
  for(int i = 0;i < mc; i++){
    while(sensor.isBusy()) delay(50);
     int C = -1;
    while(C == -1){
      C = sensor.getCapacitance();
    }
    r = r + C;
    delay(dly);
  }
  return r/mc;
}

int getTemperature(int dly, int mc){
  int r = 0;
  for(int i = 0;i < mc; i++){
    while(sensor.isBusy()) delay(50);
    int T = -1;
    while(T == -1){
      T = sensor.getTemperature();
    }
    r = r + T;
    delay(dly);
  }
  return r/mc;
}

boolean toodry(int mois){
  if(mois < dry){
    return true;
  }
  return false;
}

boolean toowet(int mois){
  if(mois > wet){
      return true;
  }
  return false;
}

void refreshpump(){
  if(pump){
    digitalWrite(pin, HIGH);
  }else{
    digitalWrite(pin, LOW);
  }
}

