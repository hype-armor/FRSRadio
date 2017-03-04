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

unsigned int ticks;
unsigned int updateinterval = 500;
unsigned int validation;
const int size = 9;
unsigned int buf[size];
unsigned char rxBuf[8];
long unsigned int rxId;
unsigned char len;
bool hit[9] = {true,true,true,true,true,true,true,true,true};

signed int rpm;
signed int rpmMSB;
signed int rpmLSB;
signed int pitch;
signed int roll;
signed int steeringAngle;
signed int steeringAngle0;
signed int steeringAngle1;
signed int slipAngle;
signed int slipAngleLSB;
signed int slipAngleMSB;
unsigned int accelerator;
unsigned int brake;
unsigned int fuelFlow;
unsigned int fuelFlow0;
unsigned int fuelFlow1;
unsigned int gear;
unsigned int sportMode;
unsigned int sportMode0;
unsigned int sportMode1;
unsigned int clutchStatus;
signed int mph;
signed int speed0;
signed int speed1;
unsigned int oiltemp;
unsigned int coolenttemp;

void loop()
{
  //CAN.readMsgBuf(&len, rxBuf);
  //rxId = CAN.getCanId();

  CAN.readMsgBufID(&rxId, &len, rxBuf);
  

  // This prevents doing calculations every loop. 
  // We still need to update incoming data because not all data will be hit in a single loop or even 100.

  if (ticks == updateinterval)
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
      delay(1000);
      updateinterval = 1000;
    }
    
    // sport mode math
    sportMode = sportMode0 + sportMode1;
    
    // steering angle math
    steeringAngle = steeringAngle0 + (steeringAngle1 * 256);

    // slip angle math
    slipAngle = slipAngleLSB + (slipAngleMSB * 256);
    
    // gear math
    if (gear > 7)
    {
      // max mpg shift light
      gear -= 6;
    }
    
    // mph math
    // 26mph=700, 21mph=580, 18mph=500
    mph = (speed0 + (speed1 * 256)) * 0.0359;// * 0.621371192; // m to k

    // fuel flow math
    fuelFlow = fuelFlow0 + (fuelFlow1 * 256); // injector volume.
    
    // clutch math
    if (clutchStatus > 100)
    {
      clutchStatus = 1;
    }
    else 
    {
      clutchStatus = 0;
    }

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
    Serial.print(oiltemp);Serial.print(",");
    Serial.print(coolenttemp);Serial.print(",");
    Serial.print(updateinterval);Serial.print(",");
    Serial.print(slipAngle);Serial.print(",");
    
    
    // print unknown data
    for (int i = 0; i < size; i++)
    {
      Serial.print(buf[i]);
      if (i != size - 1)
      {
        Serial.print(',');
      }
    }
    Serial.println();

    // turn on display if in sport mode.
    if (sportMode == 0)
    {
      // set pin high.
    }

    if (updateinterval > 1000)
    {
      updateinterval = 500;
    }
    else if (validation < 2)
    {
      updateinterval += 1;
    }
    else if (validation > 2)
    {
      updateinterval--;
    }

    validation = 0;
    hit[0] = true;
    hit[1] = true;
    hit[2] = true;
    hit[3] = true;
    hit[4] = true;
    hit[5] = true;
    hit[6] = true;
    hit[7] = true;
    hit[8] = true;
  }
  
  ticks++;

  if (hit[0] && rxId == 321)
  {
    hit[0] = false;
    rpmLSB = rxBuf[4];
    rpmMSB = rxBuf[5];

    buf[0] = (rxBuf[2] + (rxBuf[3] * 256)) - 9800; // load?

    buf[2] = (rxBuf[0] + (rxBuf[1] * 256) - 9800); // idles at 60 but when you touch the gas it jumps up 256.
    buf[3] = rxBuf[1]; // lsb
    buf[4] = rxBuf[4] + (rxBuf[5] * 256); // rpm when touching the gas pedal. Otherwise it is rpm + 33,000?
    buf[5] = rxBuf[6]; // when clutch is out this shows gear. shows if in nutral. shows some sort of decel value while not in gear and rpms are dropping.
    buf[6] = (rxBuf[2] + (rxBuf[3] * 256)) - 9800; // load?
    buf[7] = rxBuf[7]; // nothing
  }
  else if (hit[1] && rxId == 320)
  {
    hit[1] = false;
    accelerator = rxBuf[0];
    clutchStatus = rxBuf[1];

    //buf[1] = (rxBuf[2] + (rxBuf[3] * 256)); // rpm target?
    
    // this is a rpm but with some status which causes errors in the gauge.
    if (rxBuf[3] > 64)
    {
      buf[1] = (rxBuf[2] + ((rxBuf[3] - 64) * 256));
    }
    else
    {
      buf[1] = (rxBuf[2] + (rxBuf[3] * 256));
    }
    //buf[1] = rxBuf[3];
  }
  else if (hit[2] && rxId == 209)
  {
    hit[2] = false;
    speed0 = rxBuf[0];
    speed1 = rxBuf[1];

    brake = rxBuf[2];

    pitch = rxBuf[7];
    roll = rxBuf[6];



    validation++;
  }
  else if (hit[3] && rxId == 864)
  {
    hit[3] = false;
    //buf[5] = rxBuf[4]; // load 0-255 I think.
    
    oiltemp = rxBuf[2];
    coolenttemp = rxBuf[3];
    fuelFlow0 = rxBuf[0];
    fuelFlow1 = rxBuf[1];
  }/*
  else if (hit[4] && rxId == 322)
  {
    hit[4] = false;
    // has a status overtop some real data. Related to rpm
    // idle is 9,860, touching gas is 10,000
    // 3845 is 0
    if (rxBuf[1] == 38)
    {
      buf[6] = rxBuf[0];
    }
    else if (rxBuf[1] < 166)
    {
      buf[6] = (rxBuf[0] + ((rxBuf[1] - 39) * 256));
    }
    else if (rxBuf[1] >= 166)
    {
      buf[6] = (rxBuf[0] + ((rxBuf[1] - 166) * 256));
    }
    buf[7] = rxBuf[1];
  }*/
  else if (hit[5] && rxId == 324)
  {
    hit[5] = false;
    buf[8] = rxBuf[2]; // rpm related, MSB, I think this is a timing but could be another sensor reading.
  }
  else if (hit[6] && rxId == 865)
  {
    hit[6] = false;
    gear = rxBuf[0]; // gear
  }
  else if (hit[7] && rxId == 208)
  {
    hit[7] = false;
    // steering angle
    steeringAngle0 = rxBuf[0];
    steeringAngle1 = rxBuf[1];

    
  }
  else if (hit[8] && rxId == 211)
  {
    hit[8] = false;
    // sport modes
    sportMode0 = rxBuf[0];
    sportMode1 = rxBuf[1];
  }
  else if (rxId == 208)
  {
    slipAngleLSB = rxBuf[2];
    slipAngleMSB = rxBuf[3];
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/