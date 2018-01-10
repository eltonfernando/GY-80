// Programa : Teste Giroscopio L3G4200D
// autor: elton fernandes dos santos eltonfernando90@gmail.com
// data: 09/01/2018
// os filtros estao abilitado com fc= 0.02HZ para o FPA e 32Hz 12.5Hz para FPB
#include <Wire.h>

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

//Endereco I2C do L3G4200D
int L3G4200D_Address = 105;

float x=0;
float y=0;
float z=0;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  Serial.println("Inicializando o L3G4200D");
  // Conf
  setupL3G4200D();

  // Aguarda a resposta do sensor
  delay(1500);
}

void loop()
{
  static unsigned long tempo = 0;
  static float x_1 = 0;
  static float y_1 = 0;
  static float z_1 = 0;
  // Atualiza os valores de X, Y e Z
  getGyroValues();

  tempo = micros() - tempo; // fim get tempo
  float dt = ((float)tempo / 1000000); // converte para float
  x = x_1 + x *0.00875* dt; // integrador  x é velodidade vira posiçao
  y = y_1 + y *0.00875* dt;
  z = z_1 + z * dt*0.00875; // 0.00875 sensibilidade para 250pds (ou pratico 100/11428)
  x_1 = x;
  y_1 = y;
  z_1 = z;
  tempo = micros(); //dispara cronometro

  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" "); 
  Serial.println(z);

  delay(10);
}

void getGyroValues2()// leitura mais rapida mas a um erro que precisa ser corrigido para usar com esse sensor
{
  // Rotina para leitura dos valores de X, Y e Z
  Wire.beginTransmission(L3G4200D_Address);
  Wire.write(0x28); //registrado 0X28
  Wire.endTransmission();

  // ler os proximos 6 registradores apartir de DATAX0 (0X28 a 0X2C)
  Wire.requestFrom(L3G4200D_Address, 6); // read a byte

  byte xLSB = Wire.read(); // 0x28
  byte xMSB = Wire.read(); // 0x29
  byte yLSB = Wire.read(); // 0x2A
  byte yMSB = Wire.read(); // 0x2B
  byte zLSB = Wire.read(); // 0x2C
  byte zMSB = Wire.read(); // 0x2D
 
  x = ((xMSB << 8) | xLSB);
  y = ((yMSB << 8) | yLSB);
  z = ((zMSB << 8) | zLSB);

}

int setupL3G4200D()
{

  // abilita modo de operação para mormal ( leitura)  x, y, z 
  //bits 4 a 0 abilita modo normal (leitura de x y z)
  // bits  4 MSB frenquencia de FPB (FPB1 = 32HZ e FPB2 = 12.5) 
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // filtro passa alta 
  // bits 9 e 7 nao usado 00
  // bits 6 e 5 modo e operação do filtro 00
  // bits 3 a 0 frequencia de corte FPA 0101 (0.02Hz)
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00110101);

  // configura interrupcao
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controle de escala dps  definido com 250 dps
  // bit 7 DBU = 1 (amostra ooletada em um t comum)
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b10000000); 
 
  

 // bit  7 =0 desabilita FIFO (os dados vao direto para saida )
 // bit 6 nao usado
 // bit 5 Hpen em 1 abilita filtro FPA (filtro passa alta)
 // bit 4 INT1_Sel1 bit 3 INT1_Sel0 ( abilita interrupcao)
 // bit 2 Out_Sel1 bit 1  Out_Sel0 (abilita filtros)
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00010011);

// REG referencia
writeRegister(L3G4200D_Address, 0x25, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val)
{
  Wire.beginTransmission(deviceAddress); // start transmission to device
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address)
{
  int v;
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
  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  y = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);
}
