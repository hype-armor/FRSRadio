#include <SPI.h>
#include "mcp_can.h"
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* strtol */

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);


void setup()
{
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS))
  {
      delay(100);
  }
}

signed int ticks = 0;
const int size = 9;
double buf[size];
unsigned char rxBuf[8];
long unsigned int rxId;
unsigned char len = 0;

signed int rpm = 0;
signed int rpmMSB = 0;
signed int rpmLSB = 0;
signed int pitch = 0;
signed int roll = 0;
signed int steeringAngle = 0;
unsigned int steeringAngle0 = 0;
unsigned int steeringAngle1 = 0;
unsigned int accelerator = 0;
unsigned int brake = 0;
unsigned int fuelFlow = 0;
unsigned int fuelFlow0 = 0;
unsigned int fuelFlow1 = 0;
unsigned int gear = 0;
unsigned int sportMode = 0;
unsigned int sportMode0 = 0;
unsigned int sportMode1 = 0;
double clutchStatus = 0.0;
signed int mph = 0;
signed int speed0 = 0;
signed int speed1 = 0;


String line = "";
void loop()
{
  //CAN.readMsgBuf(&len, rxBuf);
  //rxId = CAN.getCanId();

  CAN.readMsgBufID(&rxId, &len, rxBuf);

  if (ticks == 1000)
  {
    ticks = 0;
    
    // rpm math
    if (rpmMSB > 128)
    {
      rpm = (rpmLSB + ((rpmMSB - 128) * 256));
    }
    else
    {
      rpm = (rpmLSB + (rpmMSB * 256));
    }
    
    // limit the data sent while off
    if (rpm <= 0 && accelerator == 0 && mph == 0)
    {
      ticks = -20000; 
    }
    
    // sport mode math
    sportMode = sportMode0 + sportMode1;
    
    // steering angle math
    steeringAngle = steeringAngle0 + (steeringAngle * 256);
    
    // gear math
    if (gear > 7)
    {
      // max mpg shift light
      gear -= 6;
    }
    
    // mph math
    mph = (speed0 + (speed1 * 256));// * 0.621371192; // m to k

    // fuel flow math
    fuelFlow = fuelFlow0 + (fuelFlow1 * 256);
    
    
    // print known data 
    Serial.print(rpm);Serial.print(",");
    Serial.print(accelerator);Serial.print(",");
    Serial.print(brake);Serial.print(",");
    Serial.print(clutchStatus);Serial.print(",");
    Serial.print(fuelFlow);Serial.print(",");
    Serial.print(gear);Serial.print(",");
    Serial.print(mph);Serial.print(",");
    Serial.print(steeringAngle);Serial.print(",");
    Serial.print(pitch);Serial.print(",");
    Serial.print(roll);Serial.print(",");
    Serial.print(sportMode);Serial.print(",");
    
    // print unknown data
    for (int i =0; i < size; i++)
    {
      Serial.print(buf[i]);
      if (i != size - 1)
      {
        Serial.print(',');
      }
    }
    Serial.println();

    // start logging?
    if (sportMode == 0)
    {
      // set log flag
    }

    // send power to pi?
    // if some trigger then send power to pi.
  }
  
  ticks++;

  if (rxId == 321)
  {
    rpmLSB = rxBuf[4];
    rpmMSB = rxBuf[5];

    buf[0] = (rxBuf[2] + ((rxBuf[3] - 37) * 256));
  }
  else if (rxId == 320)
  {
    accelerator = rxBuf[0];
    clutchStatus = rxBuf[1];

    //buf[1] = (rxBuf[2] + (rxBuf[3] * 256)); // rpm target?
    
    if (rxBuf[3] > 64)
    {
      buf[1] = (rxBuf[2] + ((rxBuf[3] - 64) * 256));
    }
    else
    {
      buf[1] = (rxBuf[2] + (rxBuf[3] * 256));
    }
    buf[1] = rxBuf[3];
  }
  else if (rxId == 209)
  {
    brake = rxBuf[2];
    speed0 = rxBuf[0];
    speed1 = rxBuf[1];

    buf[2] = rxBuf[4];
    buf[3] = rxBuf[5];
    buf[4] = rxBuf[6];
    
    if (rxBuf[7] > 128)
    {
      buf[5] = rxBuf[7] - 256;
    }
    else
    {
      buf[5] = rxBuf[7];
    }
    buf[5] = rxBuf[7];
  }
  else if (rxId == 864)
  {
    buf[6] = rxBuf[4]; // load
    
    fuelFlow0 = rxBuf[0];
    fuelFlow1 = rxBuf[1];
  }
  else if (rxId == 322)
  {
    buf[7] = rxBuf[0] + (rxBuf[1] * 256);    
  }
  else if (rxId == 324)
  {
    buf[8] = rxBuf[2]; // flow
  }
  else if (rxId == 865)
  {
    gear = rxBuf[0]; // gear
  }
  else if (rxId == 208)
  {
    // steering angle
    steeringAngle0 = rxBuf[0];
    steeringAngle1 = rxBuf[1];
  }
  else if (rxId == 211)
  {
    // sport modes
    sportMode0 = rxBuf[0];
    sportMode1 = rxBuf[1];
    
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
