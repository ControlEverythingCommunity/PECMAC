// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// PECMAC125A
// This code is designed to work with the PECMAC125A_DLCT03C20 I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Current?sku=PECMAC125A_DLCT03C20#tabs-0-product_tabset-2

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class PECMAC125A
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, PECMAC125A I2C address is 0x2A(42)
		I2CDevice device = bus.getDevice(0x2A);

		// Read configuration command
		// Header byte-1, Header byte-2, command-2, byte 3, 4, 5 and 6 are reserved, checksum
		byte[] readConfigCommand = {(byte)0x92, (byte)0x6A, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
		populateChecksum(readConfigCommand);

		// Send read configuration command
		device.write(readConfigCommand, 0, 8);
		Thread.sleep(300);

		byte[] data = new byte[36];
		// Read 3 bytes of data
		// TypeOfSensor, maxCurrent, noOfChannel
		device.read(data, 0, 3);

		int typeOfSensor = data[0];
		int maxCurrent = data[1];
		byte noOfChannel = (byte)data[2];

		// Output data to screen
		System.out.printf("Type Of Sensor : %d %n", typeOfSensor);
		System.out.printf("Max Current : %d A %n", maxCurrent);
		System.out.printf("No Of Channel : %d %n", noOfChannel);

		// Read current command
		// Header byte-1, Header byte-2, command-1, start channel-1, stop channel-12, byte 5 and 6 reserved, checksum
		byte[] readCurrentCommand = {(byte)0x92, (byte)0x6A, 0x01, 0x01, noOfChannel, 0x00, 0x00, 0x00};
		populateChecksum(readCurrentCommand);

		// Send read current command
		device.write(readCurrentCommand, 0, 8);
		Thread.sleep(300);

		// Read Current data
		// msb1, msb, lsb
		device.read(data, 0, noOfChannel * 3 + 1);
		Thread.sleep(500);

		for(int i = 0; i < noOfChannel; i++)
		{
			int msb1 = (data[i * 3] & 0xFF);
			int msb = (data[1 + i * 3] & 0xFF);
			int lsb = (data[2 + i * 3] & 0xFF); 
			float current = (float)((msb1 * 65536.0) + (msb * 256.0) + lsb);

			// Convert the data to ampere
			current = current / 1000;

			// Output data to screen
			System.out.printf("Channel : %d %n", i+1);
			System.out.printf("Current : %.3f A %n", current);
		}
	}

	private static void populateChecksum(byte[] command)
	{
		byte sum = 0x00;
		for(int i = 0; i < command.length - 1; i++)
		{
			sum += command[i];
		}
		command[command.length - 1] = sum;
	}
}
