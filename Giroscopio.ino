// Programa : Teste Giroscopio L3G4200D
// autor: elton fernandes dos santos eltonfernando90@gmail.com
// data: 09/01/2018
// os filtros estao abilidatado com fc= 0.02HZ para o FPA e 32Hz 12.5Hz para FPB
#include <Wire.h>

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define GYRO_ADDRESS 105
//Endereco I2C do L3G4200D
//int L3G4200D_Address = 105;
#define OFF_SET_X 129//1.01
#define OFF_SET_Y 210 //1.77
#define OFF_SET_Z -15.67

float x = 0;
float y = 0;
float z = 0;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  Serial.println("Inicializando o L3G4200D");
  // Conf
  setupGyro();

  // Aguarda a resposta do sensor
  delay(100);
}

void loop()
{
  static unsigned long tempo = 0;
  static float x_1 = 0;
  static float y_1 = 0;
  static float z_1 = 0;
  static float anguloGz = 0;
  // Atualiza os valores de X, Y e Z

  getGyroValues();
  tempo = micros() - tempo; // fim get tempo
  float dt = ((float)tempo / 1000000); // converte para float
  anguloGz = anguloGz + (z) * dt;// integrador
  tempo = micros(); //dispara cronometro
z_1+=z;
x_1++;
y_1=z_1/x_1;
  Serial.print(x,8);
  Serial.print(" ");
  Serial.print(y,8);
  Serial.print(" ");
  Serial.println(z,8);
  

  delay(9);
}

void getGyroValues2()// leitura mais rapida mas a um erro que precisa ser corrigido para usar com esse sensor
{
  // Rotina para leitura dos valores de X, Y e Z
  Wire.beginTransmission(GYRO_ADDRESS);
  Wire.write(0X28); //registrado 0X28
  Wire.endTransmission();

  // ler os proximos 6 registradores apartir de DATAX0 (0X28 a 0X2D)
  Wire.requestFrom(GYRO_ADDRESS, 6); // read a byte


  byte xLSB = Wire.read(); // 0x28
  byte xMSB = Wire.read(); // 0x29
  byte yLSB = Wire.read(); // 0x2A
  byte yMSB = Wire.read(); // 0x2B
  byte zLSB = Wire.read(); // 0x2C
  byte zMSB = Wire.read(); // 0x2D


  x = ((xMSB << 8) | xLSB) * 0.00875; // 0.00875 sensibilidade para 250pds
  y = ((yMSB << 8) | yLSB) * 0.00875; // 0.00875 sensibilidade para 250pds
  z = ((zMSB << 8) | zLSB) * 0.00875; // 0.00875 sensibilidade para 250pds


}

int setupGyro()
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
void getGyroValues()
{
  // Rotina para leitura dos valores de X, Y e Z

  byte xMSB = readRegister(GYRO_ADDRESS, 0x29);
  byte xLSB = readRegister(GYRO_ADDRESS, 0x28);
  x =(((xMSB << 8) | xLSB)-OFF_SET_X)*0.00875;// * 0.00875-OFF_SET_X; // 0.00875 sensibilidade para 250pds
  //print2(xMSB);
  byte yMSB = readRegister(GYRO_ADDRESS, 0x2B);
  byte yLSB = readRegister(GYRO_ADDRESS, 0x2A);
  y = (((yMSB << 8) | yLSB)-OFF_SET_Y)*0.00875; // 0.00875 sensibilidade para 250pds

  byte zMSB = readRegister(GYRO_ADDRESS, 0x2D);
  byte zLSB = readRegister(GYRO_ADDRESS, 0x2C);
  z = (((zMSB << 8) | zLSB)-OFF_SET_Z)* 0.00875; // 0.00875 sensibilidade para 250pds
}
