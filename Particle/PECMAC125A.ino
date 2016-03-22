// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// PECMAC125A
// This code is designed to work with the PECMAC125A_DLCT03C20 I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Current?sku=PECMAC125A_DLCT03C20#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// PECMAC125A I2C address is 2A(42)
#define Addr 0x2A

unsigned int data[36];
int typeOfSensor = 0;
int maxCurrent = 0;
int noOfChannel = 0;
void setup() 
{
    // Set variable
    Particle.variable("i2cdevice", "PECMAC125A");
    Particle.variable("typeOfSensor", typeOfSensor);
    Particle.variable("maxCurrent", maxCurrent);
    Particle.variable("noOfChannel", noOfChannel);
    
    // Initialise I2C communication as MASTER
    Wire.begin();
    // Initialise Serial Communication, set baud rate = 9600
    Serial.begin(9600);
    
    // Read configuration command
    // Header byte-1, Header byte-2, command-2, byte 3, 4, 5 and 6 are reserved, checksum
    byte readConfigCommand[8] = {0x92, 0x6A, 0x02, 0x00, 0x00, 0x00, 0x00, 0xFE};
    
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Send read configuration command   
    Wire.write(readConfigCommand, 8);
    // Stop I2C Transmission
    Wire.endTransmission();
    
    for(int i = 0; i < 3; i++)
    {
        // Start I2C Transmission
        Wire.beginTransmission(Addr);
        // Stop I2C Transmission
        Wire.endTransmission();
        
        // Request 1 byte of data
        Wire.requestFrom(Addr, 1);
        
        // Read 3 bytes of data
        // typeOfSensor, maxCurrent, noOfChannel
        if(Wire.available() == 1)
        {
            data[i] = Wire.read();
        }
    }
    
    typeOfSensor = data[0];
    maxCurrent = data[1];
    noOfChannel = data[2];
    
    // Output data to dashboard
    Particle.publish(" Type Of Sensor : ", String(typeOfSensor));
    Particle.publish(" Max Current : ", String(maxCurrent));
    Particle.publish(" No Of Channel : ", String(noOfChannel));
    delay(300);
}

void loop()
{
    // Read current command
    // Header byte-1, Header byte-2, command-1, start channel-1, stop channel-12,
    // byte 5 and 6 reserved, checksum
    byte readCurrentCommand[8] = {0x92, 0x6A, 0x01, 0x01, noOfChannel, 0x00, 0x00, 0x0A};
    
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Send configuration command   
    Wire.write(readCurrentCommand, 8);
    // Stop I2C Transmission
    Wire.endTransmission();
    
    for(int i = 0; i < noOfChannel * 3; i++)
    {
        // Start I2C Transmission
        Wire.beginTransmission(Addr);
        // Stop I2C Transmission
        Wire.endTransmission();
        
        // Request 1 byte of data
        Wire.requestFrom(Addr, 1);
        
        // Read 3 bytes of data
        // typeOfSensor, maxCurrent, noOfChannel
        if(Wire.available() == 1)
        {
            data[i] = Wire.read();
        }
    }
    
    for(int i = 0; i < noOfChannel; i++)
    {
        // Read current data
        // msb1, msb, lsb
        int msb1 = data[i * 3] & 0xff;
        int msb = data[1 + i * 3] & 0xff ;
        int lsb = data[2 + i * 3] & 0xff;
        Serial.print(msb1);
        Serial.print(msb);
        Serial.print(lsb);
        float current = (msb1 * 65536) + (msb * 256) + lsb;
        
        // Convert the data to ampere
        current = current / 1000;
        
        // Output to the dashboard
        Particle.publish("Channel : ", String(i + 1));
        Particle.publish("Current : ", String(current));
    }
    delay(500);
}

