#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

I2CSoilMoistureSensor sensor;
const int pwm_a = 3, dir_a = 12, brake_a = 9, sns_a = A0;
int delaytime = 100;//time between two measurements in ms
int measurecount = 1;//how many measure are taken for the average
int dry; //used to toggle on the pump
int wet; //used to toggle off the pump
boolean pump;//determens wether the pump should be on or off
int capacitance, temperature; 

void setup() {
  Wire.begin();
  Serial.begin(9600);
  dry = 350;
  wet = 500;

  sensor.begin(); // reset sensor
  delay(1000); // boot up
  Serial.println("\nstartup");

  pinMode(brake_a, OUTPUT);
  pinMode(dir_a, OUTPUT);
  digitalWrite(brake_a, LOW);
  digitalWrite(dir_a, HIGH);
  
  pump = false;
}

void loop() {
  while (sensor.isBusy()) delay(50); // available since FW 2.3

  capacitance = getCapacitance(delaytime,measurecount);
  temperature = getTemperature(delaytime,measurecount);
  Serial.print("Feuchtigkeit: ");
  Serial.print(dry);
  Serial.print(capacitance); //read capacitance register
  Serial.print(", Temperatur: ");
  Serial.println(temperature); //temperature register
  //Serial.print(", Belichtung: ");
  //Serial.println(sensor.getLight(true)); //request light measurement, wait and read light register
  sensor.sleep();

  //logic:
  refreshpump();
  delay(50);
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

boolean toodry(){
  if(capacitance < dry){
    return true;
  }
  return false;
}

boolean toowet(){
  if(capacitance > wet){
      return true;
  }
  return false;
}

void refreshpump(){
  if(toodry()){
    pump = true;
  }
  if(toowet()){
    pump = false;
  }
  Serial.print(pump);
  if(pump){
    analogWrite(pwm_a, 255);
    Serial.println("Pumpe sollte laufen");
  }
  else{
    analogWrite(pwm_a, 0);
    Serial.println("Pumpe sollte nicht laufen");
  }
}

