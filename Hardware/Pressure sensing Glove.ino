/*
 ESP8266 4051 Multiplexer by Brian Lough
 
 An example showing how to use a 4051 multiplexer with an esp8266
 to connect up to 8 analog sensors.
 Wiring:
 Wemos ->  4051
 ---------------
 D4    ->   S0 (A)
 D3    ->   S1 (B)
 D2    ->   S2 (C)
 A0    ->   Common
 3.3v  ->   VCC
 G     ->   GND
 G     ->   Inhibit
 G     ->   VEE  
 
 4051 Option pins are then wired to whatever Analog sensors required
 One thing to note: say for example if you only require 2 analog sensors,
 You can just wire up S0(A) and connect S1 & S2 to GND and you will be 
 able to switch between option 1 and option 2 pins.
 Same goes for up to 4 pins (just use S0 & S1)
*/
#include <Wire.h>
#include<Math.h>
#include<Ticker.h>

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>


#define FIREBASE_HOST "stroke-rehab.firebaseio.com"
#define FIREBASE_AUTH "Z1a1gk928gIXrcKebkVI6BZCc8mpmxFHlSkqndkp"
#define WIFI_SSID "Pakka nahi milega"
#define WIFI_PASSWORD "chahtekyaho"

Ticker blinker;

int MUX_A=16;             //D0 PIN
int MUX_B=5;              //D1 PIN
int MUX_C=2;              //D4 PIN
int MUX_D=14;             //D5 PIN
int MUX_E=12;             //D6 PIN
int PALM=13;              //D7 PIN

int ANALOG_INPUT=17;      //A0 PIN


int va, vb, vc, vd, ve, vp ;
int acc_status, count=0;

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = 0;
const uint8_t sda = 4;


double Ax, Ay, Az, T, Gx, Gy, Gz, Ax0=0, Ay0=0, Az0=0;
int ta=0,tb=0,tc=0,td=0,te=0,tp=0;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

void init_sensors();
void changeMux();
void MPU6050_Init();
void acc_value();




void changeMux(int a, int b, int c, int d, int e, int p) {
  digitalWrite(MUX_A, a);
  digitalWrite(MUX_B, b);
  digitalWrite(MUX_C, c);
  digitalWrite(MUX_D, d);
  digitalWrite(MUX_E, e);
  digitalWrite(PALM, p);
}

void init_sensors(){
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);     
  pinMode(MUX_C, OUTPUT);
  pinMode(MUX_D, OUTPUT);
  pinMode(MUX_E, OUTPUT);
  pinMode(PALM, OUTPUT);
  pinMode(ANALOG_INPUT, INPUT); 
}

void sensorout(){

  
  changeMux(HIGH, LOW, LOW, LOW, LOW, LOW);
  va = analogRead(ANALOG_INPUT); //Value of the sensor connected D0 pin of Mux
  
  changeMux(LOW, HIGH, LOW, LOW, LOW, LOW);
  vb = analogRead(ANALOG_INPUT); //Value of the sensor connected Option D1 pin of Mux
  
  changeMux(LOW, LOW, HIGH, LOW, LOW, LOW);
  vc = analogRead(ANALOG_INPUT); //Value of the sensor connected Option D4 pin of Mux

  changeMux(LOW, LOW, LOW, HIGH, LOW, LOW);
  vd = analogRead(ANALOG_INPUT); //Value of the sensor connected Option D5 pin of Mux

  changeMux(LOW, LOW, LOW, LOW, HIGH, LOW);
  ve = analogRead(ANALOG_INPUT); //Value of the sensor connected Option D6 pin of Mux

  changeMux(LOW, LOW, LOW, LOW, LOW, HIGH);
  vp = analogRead(ANALOG_INPUT); //Value of the sensor connected Option D7 pin of Mux

//  Serial.print(va);
//  Serial.print(" ");
//  Serial.print(vb);
//  Serial.print(" ");
//  Serial.print(vc);
//  Serial.print(" ");
//  Serial.print(vd);
//  Serial.print(" ");
//  Serial.print(ve);
//  Serial.print(" ");
//  Serial.print(vp);
//  Serial.print(" ");
//  Serial.print("\n");
//  delay(100);
}

void acc_value(){
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  //divide each with their sensitivity scale factor
  Ax = (double)AccelX/AccelScaleFactor;
  Ay = (double)AccelY/AccelScaleFactor;
  Az = (double)AccelZ/AccelScaleFactor;
  T = (double)Temperature/340+36.53; //temperature formula
  Gx = (double)GyroX/GyroScaleFactor;
  Gy = (double)GyroY/GyroScaleFactor;
  Gz = (double)GyroZ/GyroScaleFactor;

//  Serial.print("Ax: "); Serial.print(Ax);
//  Serial.print(" Ay: "); Serial.print(Ay);
//  Serial.print(" Az: "); Serial.print(Az);
//  Serial.print("\n");
//  Serial.print(" T: "); Serial.print(T);
//  Serial.print(" Gx: "); Serial.print(Gx);
//  Serial.print(" Gy: "); Serial.print(Gy);
//  Serial.print(" Gz: "); Serial.println(Gz);
  double a=Ax-Ax0;
  double b=Ax-Ax0;
  double c=Ax-Ax0;
  a=(a>0?a:-a);
  b=(b>0?b:-b);
  c=(c>0?c:-c);
  if (a>0.03 || b>0.03 || c>0.03){
    //Serial.println("1");
    acc_status=1;
    Ax0=Ax;
    Ay0=Ay;
    Az0=Az;
    
  }
  else{
    //Serial.println("0");
    acc_status=0;
  }

}

 void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}

//void firedata(){
//  ta=ta/count;
//  tb=tb/count;
//  tc=tc/count;
//  td=td/count;
//  te=te/count;
//  tp=tp/count;
//  Firebase.set("/devices/gloveid/f1",ta);
//  Firebase.set("/devices/gloveid/f2",tb);
//  Firebase.set("/devices/gloveid/f3",tc);
//  Firebase.set("/devices/gloveid/f4",td);
//  Firebase.set("/devices/gloveid/f5",te);
//  Firebase.set("/devices/gloveid/flexibilty",tp);
//  Firebase.set("/devices/gloveid/motion",acc_status);        //acc_status
////  if (Firebase.failed()){
////    Serial.println("Failed");
////  }
//}

void setup() {
  init_sensors();         //Deifne output pins for Mux
  Serial.begin(9600);         
  Wire.begin(sda, scl);
  MPU6050_Init(); 

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //y = Firebase.getInt("/value");
  //Serial.println(y);
  
  if (Firebase.failed()){
    Serial.println("Connection Failed!!");
    }
  

  //blinker.attach(10,firedata);

  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  delay(10000);
  digitalWrite(10,LOW);
  
}

void loop() {
  sensorout();
  acc_value();

  ta+=va;
  tb+=vb;
  tc+=vc;
  td+=vd;
  te+=ve;
  tp+=vp;
  count++;
  
  Serial.println(acc_status);
  Serial.print(va);
  Serial.print(" ");
  Serial.print(vb);
  Serial.print(" ");
  Serial.print(vc);
  Serial.print(" ");
  Serial.print(vd);
  Serial.print(" ");
  Serial.print(ve);
  Serial.print(" ");
  Serial.print(vp);
  Serial.print(" ");
  Serial.print("\n");
  
   Firebase.setInt("/devices/gloveid/f1",va);
  Firebase.setInt("/devices/gloveid/f2",vb);
  Firebase.setInt("/devices/gloveid/f3",vc);
  Firebase.setInt("/devices/gloveid/f4",vd);
  Firebase.setInt("/devices/gloveid/f5",ve);
  Firebase.setInt("/devices/gloveid/flexibilty",vp);
  Firebase.setInt("/devices/gloveid/motion",acc_status);        //acc_status
   if (Firebase.failed()){
    Serial.println("Connection Failed!!");
    }
  delay(2000);
  
  
}
