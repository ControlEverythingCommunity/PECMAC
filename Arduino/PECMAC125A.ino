// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// PECMAC125A
// This code is designed to work with the PECMAC125A_DLCT03C20 I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Current?sku=PECMAC125A_DLCT03C20#tabs-0-product_tabset-2

#include<Wire.h>

// PECMAC125A I2C address is 2A(42)
#define Addr 0x2A

unsigned int data[36];
int typeOfSensor = 0;
int maxCurrent = 0;
int noOfChannel = 0;
void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Command header byte-1
  Wire.write(0x92);
  // Command header byte-2
  Wire.write(0x6A);
  // Command 2 is used to read no of sensor type, Max current, No. of channel
  Wire.write(0x02);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // CheckSum
  Wire.write(0xFE);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 6 bytes of data
  Wire.requestFrom(Addr, 6);

  // Read 6 bytes of data
  if (Wire.available() == 6)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  }

  typeOfSensor = data[0];
  maxCurrent = data[1];
  noOfChannel = data[2];

  // Output data to serial monitor
  Serial.print("Type Of Sensor : ");
  Serial.println(typeOfSensor);
  Serial.print("Max Current : ");
  Serial.print(maxCurrent);
  Serial.println(" Amp");
  Serial.print("No. Of Channel : ");
  Serial.println(noOfChannel);
  delay(300);
}

void loop()
{
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Command header byte-1
  Wire.write(0x92);
  // Command header byte-2
  Wire.write(0x6A);
  // Command 1
  Wire.write(0x01);
  // Start Channel No.
  Wire.write(0x01);
  // End Channel No.
  Wire.write(noOfChannel);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // CheckSum
  Wire.write((0x92 + 0x6A + 0x01 + 0x01 + noOfChannel + 0x00 + 0x00) & 0xFF);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(500);

  // Request (noOfChannel * 3) bytes of data
  Wire.requestFrom(Addr, noOfChannel * 3);

  for (int j = 0; j < noOfChannel; j++)
  {
    // Read 3 bytes of data
    // msb1, msb, lsb
    int msb1 = Wire.read();
    int msb = Wire.read();
    int lsb = Wire.read();
    float current = (msb1 * 65536) + (msb * 256) + lsb;

    // Convert the data to ampere
    current = current / 1000;

    // Output to the serial monitor
    Serial.print("Channel : ");
    Serial.println(j + 1);
    Serial.print("Current Value : ");
    Serial.println(current);
    delay(1000);
  }
}
