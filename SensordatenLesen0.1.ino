#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

I2CSoilMoistureSensor sensor;
const int pwm_a = 3, dir_a = 12, brake_a = 9, sns_a = 0;
const int pwm_b = 11,dir_b = 13, brake_b = 8,sns_b = 1;
const int cdwateringmax = 20, cdwaitingmax = 1200;//maximum cooldown times in 50ms
int delaytime = 100;//time between two measurements in ms
int measurecount = 1;//how many measure are taken for the average
int dry; //used to toggle on the pump
int flooded; //used to toggle off the pump
int cdwatering, cdwaiting; //actual cooldown times in 50ms
boolean pump;//determens wether the pump should be on or off
int capacitance, temperature; 

void setup() {
  Wire.begin();
  Serial.begin(9600);
  dry = 362;
  flooded = 520;
  cdwaiting = 200;
  cdwatering = 0;

  sensor.begin(); // reset sensor
  delay(1000); // boot up
  Serial.println("\nstartup");

  pinMode(brake_a, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(brake_b, OUTPUT);
  pinMode(dir_b, OUTPUT);
  digitalWrite(brake_a, LOW);
  digitalWrite(dir_a, HIGH);
  digitalWrite(brake_b, LOW);
  digitalWrite(dir_b, HIGH);
  
  pump = false;
}

void loop() {
  while (sensor.isBusy()) delay(50); // available since FW 2.3

  capacitance = getCapacitance(delaytime,measurecount);
  temperature = getTemperature(delaytime,measurecount);
  Serial.print("Feuchtigkeit: ");
  Serial.print(capacitance); //read capacitance register
  Serial.print(", Temperatur: ");
  Serial.println(temperature); //temperature register
  //Serial.print(", Belichtung: ");
  //Serial.println(sensor.getLight(true)); //request light measurement, wait and read light register
  sensor.sleep();

  if(cdwaiting > 0){
    cdwaiting = cdwaiting - 1;
  }
  Serial.print("Cooldown Waiting: ");
  Serial.print(cdwaiting);
  
  if(cdwatering > 0){
    cdwatering = cdwatering - 1;
  }
  Serial.print("; Cooldown Watering: ");
  Serial.println(cdwatering);
  
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

boolean isflooded(){
  if(capacitance > flooded){
      return true;
  }
  return false;
}

void refreshpump(){
  if(cdwatering == 0 && pump){
     pump = false;
     cdwaiting = cdwaitingmax;
  }
  if(toodry() && cdwaiting == 0 && !pump){
    cdwatering = cdwateringmax;
    pump = true;
  }
  if(isflooded()){
    pump = false;
    cdwatering = 0;
    cdwaiting = cdwaitingmax;
  }
  
  if(pump){
    analogWrite(pwm_a, 255);
    analogWrite(pwm_b, 255);
    Serial.println("Pumpe sollte laufen");
  }
  else{
    analogWrite(pwm_a, 0);
    analogWrite(pwm_b, 0);
    Serial.println("Pumpe sollte nicht laufen");
  }
}

