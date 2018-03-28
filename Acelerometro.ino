

/*
  Programa : Teste acelerometro ADXL345
  autor: eng. Elton fernandes dos Santos
  email: eltonfernand90@gmail.com
  data: 07 de janeiro de 2018

  dados frequencia de amostragem padrao 100Hz (10 ms) com BW 50HZ (isso depende do modo de consumo asociado ao bit LOW_POWER de BW_RATE)
  BW_RATE (x02C) = 0X0A
*/

#include <Wire.h>
#define ADXL345_POWER_CTL 0x2D
// registrador  DATA
#define ADXL345_RG_X0 0x32 // DATAX0
#define ADXL345_RG_X1 0x33
#define ADXL345_RG_Y0 0x34
#define ADXL345_RG_Y1 0x35
#define ADXL345_RG_Z0 0x36
#define ADXL345_RG_Z1 0x37
// Endereco I2C do sensor : 0x53 (8 bits)
#define ADXAddress 0x53  // the default 7-bit slave address

float Xg, Yg, Zg;

void setup()
{
  Wire.begin();
  // Wire.setClockStretchLimit(4000); // in us default 230us
  // Wire.setClock(200000);// dafalt (100000) vai de 1 a 4
  Serial.begin(115200);

  // enable to measute g data
  writeRegister(ADXAddress, ADXL345_POWER_CTL, 8); // abilita medição 0b1000
  Serial.println("inciando..."); 
 // Serial.println(sizeof(int)); 
  delay(100);
}

void loop()
{
  static float FPBxg = 0;
  static float FPByg = 0;
  static float FPBzg = 0;
  static float Fxg = 0;
  static float Fyg = 0;
  static float Fzg = 0;
  
  //static unsigned long tempo =0;
  getAdxValor(); // acelerometro

     //filtro PB com fc=50HZ (acelerometro)(baixo primeiro matlab)
    // FPBxg = 0.7184 * FPBxg + 0.2816 * Xg; //0.5HZ
    // FPByg = 0.7184 * FPByg + 0.2816 * Yg; // 1HZ
   //  FPBzg = 0.604922562764271 * FPBzg +  0.395077437235729 * Zg;//8Hz
   
   FPBxg = 0.718424999647472 * FPBxg + 0.281575000352528 * Xg; //5.26Hz ts=0.19
   FPByg = 0.718424999647472 * FPByg + 0.281575000352528 * Yg; //5.26Hz ts=0.19
   FPBzg = 0.718424999647472 * FPBzg +  0.281575000352528 * Zg;//5.26Hz ts=0.19

  // converte pra graus
Fzg=atan2(FPBzg,sqrt(FPByg*FPByg+FPBxg*FPBxg))*57.2958;// 180/pi= 57.2958

Fyg=atan2(FPByg, sqrt(FPBxg*FPBxg+FPBzg*FPBzg))*57.2958;// 180/pi= 57.2958
Fxg=atan2(FPBxg,sqrt(FPByg*FPByg+FPBzg*FPBzg))*57.2958;// 180/pi= 57.2958
  
  // converte pra graus
float x,y,z;
x=Xg;
y=Yg;
z=Zg;
Zg=atan2(z,sqrt(y*y+x*x))*57.2958;// 180/pi= 57.2958
Yg=atan2(y,sqrt(x*x+z*z))*57.2958;// 180/pi= 57.2958
Xg=atan2(x,sqrt(y*y+z*z))*57.2958;// 180/pi= 57.2958

 Serial.print(Xg);
 Serial.print(" ");
 Serial.print(Yg);
 Serial.print(" ");
 Serial.print(Zg);
  Serial.print(" ");
 Serial.print(Fxg);
 Serial.print(" ");
  Serial.print(Zg);
 Serial.print(" ");
 Serial.println(Fzg);


  delay(9);
}
void getAdxValor() {
int x=0; 
 int  y=0; 
 int  z=0;
  Wire.beginTransmission(ADXAddress);
  Wire.write(ADXL345_RG_X0); //registrado 0X32
  Wire.endTransmission();

  // ler os proximos 6 registradores apartir de DATAX0 (0X32 a 0X37)
  Wire.requestFrom(ADXAddress, 6); // read a byte

  byte xLSB = Wire.read(); // DATAX0
  byte xMSB = Wire.read(); // DATAX1
  byte yLSB = Wire.read(); // DATAY0
  byte yMSB = Wire.read(); // DATAY1
  byte zLSB = Wire.read(); // DATAZ0
  byte zMSB = Wire.read(); // DATAZ1
  // 256 é sensibidade (DATA esta na unit LSB/g)

 x= ((xMSB << 8) | xLSB);
 
y = ((yMSB << 8) | yLSB);

 z=((zMSB << 8) | zLSB);

  Xg = (float)x / 256;
  Yg = (float)y / 256;
  Zg = (float)z / 256;

}
void writeRegister(int deviceAddress, byte address, byte val)
{
  Wire.beginTransmission(deviceAddress); // start transmission to device
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}


