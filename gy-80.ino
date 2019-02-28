// implementação de fusão do acelerometro e giroscopio com filtro complementar
// autor: Elton fernandes dos Santos eltonfernando90@gmail.com
// data: 07 de janeiro de 2018

/*
  esse algoritimo foi implementado para o sensor GY-80
  a acelerometro foi configurado para uma taxas de amostrogem de 100Hz e BW 50Hz

*/
#include <Wire.h>

#define ADXL345_RG_ID 0
#define ADXL345_RG_2D 0x2D
#define ADXL345_RG_X0 0x32
#define ADXL345_RG_X1 0x33
#define ADXL345_RG_Y0 0x34
#define ADXL345_RG_Y1 0x35
#define ADXL345_RG_Z0 0x36
#define ADXL345_RG_Z1 0x37


// Endereco I2C do sensor : 83 em decimal ou 0x53
int ADXAddress = 0x53;  // the default 7-bit slave address

float Xg, Yg, Zg;
// giroscopio
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define GYRO_ADDRESS 105
#define OFF_SET_X 129
#define OFF_SET_Y 210
#define OFF_SET_Z -15.67
//Endereco I2C do L3G4200D
//int L3G4200D_Address = 105;//0x69

float x = 0;
float y=0;
float z=0;


void setup()
{

  Wire.begin();
  Serial.begin(115200);
  // setup acele
  writeRegister(ADXAddress, ADXL345_RG_2D, 8); // abilita medição 0b1000
  setupGyro();

  // Aguarda a resposta do sensor
  delay(40);
}

void loop()
{
  //

  static float filtroComplementar=0;
  static unsigned long tempo = 0;
  static float anguloGx = 0;
    static float FPBxg = 0;
  static float FPByg = 0;
  static float FPBzg = 0;
  static float Fxg = 0;
  static float Fyg = 0;
  static float Fzg = 0;
  getAdxValor();
  getGyroValues();
 FPBxg = 0.718424999647472 * FPBxg + 0.281575000352528 * Xg; //5.26Hz ts=0.19
 FPByg = 0.718424999647472 * FPByg + 0.281575000352528 * Yg; //5.26Hz ts=0.19
 FPBzg = 0.718424999647472 * FPBzg +  0.281575000352528 * Zg;//5.26Hz ts=0.19
 
  //integrador
  Zg=atan2(Zg,sqrt(Yg*Yg+Xg*Xg))*57.2958;// 180/pi= 57.2958
  Fzg=atan2(FPBzg,sqrt(FPByg*FPByg+FPBxg*FPBxg))*57.2958;
  tempo = micros() - tempo; // fim get tempo
  float dt = (float)tempo / 1000000; // converte para float

  filtroComplementar =  0.95*(filtroComplementar + (x) * dt) + Fzg*0.05;
  anguloGx+=(x) * dt;
 tempo = micros(); //dispara cronometro
  Serial.print(filtroComplementar);
  Serial.print(" ");
  Serial.print(anguloGx);
  Serial.print(" ");
  Serial.println(Fzg);
 // Serial.print(" ");
  //Serial.println(Zg); 
  
 
  delay(9);
}
byte readRegister(int deviceAddress, byte address)
{
  byte v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address); // register to read
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1); // read a byte

  while (Wire.available())
  {
    v = Wire.read();
  }

  return v;
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

void getGyroValues()
{
  // Rotina para leitura dos valores de X, Y e Z

  byte xMSB = readRegister(GYRO_ADDRESS, 0x29);
  byte xLSB = readRegister(GYRO_ADDRESS, 0x28);
  x =-(((xMSB << 8) | xLSB)-OFF_SET_X)*0.00875; // 0.00875 sensibilidade para 250pds
  //print2(xMSB);
  byte yMSB = readRegister(GYRO_ADDRESS, 0x2B);
  byte yLSB = readRegister(GYRO_ADDRESS, 0x2A);
   y = (((yMSB << 8) | yLSB)-OFF_SET_Y)*0.00875; // 0.00875 sensibilidade para 250pds

  byte zMSB = readRegister(GYRO_ADDRESS, 0x2D);
  byte zLSB = readRegister(GYRO_ADDRESS, 0x2C);
  z = (((zMSB << 8) | zLSB)-OFF_SET_Z)* 0.00875; // 0.00875 sensibilidade para 250pds
}
void setupGyro()
{
 //L3G4200D
  // abilita modo de operação para mormal ( leitura)  x, y, z
  //bits 8-7 DR1-DR0 taxa de amostragem
  // bits 6-5 BW1-BW0 largura de banda => frenquencia de FPB (FPB1 = 32HZ e FPB2 = 25)
  //bits 4 PD mode de operacao ( 1 normal modo para leitura)
  // bits 4-2-1 abilitas as saída de dados dos eixos x y z
  writeRegister(GYRO_ADDRESS, CTRL_REG1, 0b00011111);

  // filtro passa alta
  // bits 8-7 nao usado 00
  // bits 6-5 HPM1-HPM0 modo e operação do filtro 00
  // bits 4-3-2-1 HPCF3-HPCF2-HPCF1-HPCF0 frequencia de corte FPA 0001 (4Hz)
  writeRegister(GYRO_ADDRESS, CTRL_REG2, 0b00110001);

  // configura interrupcao
  writeRegister(GYRO_ADDRESS, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controle de escala dps  definido com 250 dps
  // bit 8 DBU = 1 (amostra ooletada em um t comum)
  //bit 7 BLE se 1 troca MSB por LSB (default=0)
  //bit 6-5 FS1-FS0 define escala 00=250dps 01=500dps 10=2000dps=11
  // bit 4 não usado
  //bit 3-2 ST1-ST0 auto teste default=00
  //bit 1 sim define interface de comunicação spi 1= 3fios 0=4fios (default=0)
  writeRegister(GYRO_ADDRESS, CTRL_REG4, 0b10000000);


  // bit 8 BOOT 0= modo normal
  // bit  7 FIFO_EN =0 desabilita FIFO (os dados vao direto para saida )
  // bit 6 nao usado
  // bit 5 Hpen em 1 seleciona filtro FPA (filtro passa alta)
  // bit 4 INT1_Sel1 bit 3 INT1_Sel0 ( seleciona interrupcao)
  // bit 2 Out_Sel1 bit 1  Out_Sel0 (abilita filtros) figura 19 no datasheet
  writeRegister(GYRO_ADDRESS, CTRL_REG5, 0b00010011);

  // REG referencia
  writeRegister(GYRO_ADDRESS, 0x25, 0b00000000);

}
void writeRegister(int deviceAddress, byte address, byte val)
{
  Wire.beginTransmission(deviceAddress); // start transmission to device
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}
