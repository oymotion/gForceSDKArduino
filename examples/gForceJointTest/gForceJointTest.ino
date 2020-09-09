/*
 * Copyright 2017, OYMotion Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 */
#include <gForceAdapter.h>

#define gforceSerial Serial2

/* returns char count */
int SYS_GetChar(unsigned char *data)
{
  int ret = gforceSerial.read();

  if (ret == -1)
    return 0;

  *data = (unsigned char)ret;

  return 1;
}


/* returns System time */
unsigned long SYS_GetTick(void)
{
  return millis();
}

GForceAdapter gforce(SYS_GetChar, SYS_GetTick);

unsigned long gTimestamp = 0;


void setup()
{
  Serial.begin(115200);
  Serial.println("setup");
  gforce.Init();
  gTimestamp = millis();
}

void loop()
{
  GF_Data gForceData;

  if (GF_RET_OK == gforce.GetGForceData(&gForceData, 10))
  {
    GF_Gesture gesture;

    switch (gForceData.type)
    {
    case GF_Data::QUATERNION:
      if(millis() - gTimestamp > 5000)
      {
        gTimestamp = millis();
        Serial.println("Receive quaternion,Communication normal");
      }

      break;

    case GF_Data::GESTURE:
      gesture = gForceData.value.gesture;

      if (gesture == GF_FIST)
      {
        Serial.println("Fist");
      }
      else if (gesture == GF_SPREAD)
      {
        Serial.println("Spread");
      }
      else if (gesture == GF_WAVEIN)
      {
        Serial.println("WaveIn");
      }
      else if (gesture == GF_WAVEOUT)
      {
        Serial.println("WaveOut");
      }
      else if (gesture == GF_PINCH)
      {
        Serial.println("Pinch");
      }
      else if (gesture == GF_SHOOT)
      {
        Serial.println("Shoot");
      }
      else if (gesture == GF_RELEASE)
      {
        Serial.println("Release");
      }
      else if (gesture == GF_UNKNOWN)
      {
        Serial.println("Unknown");
      }

      break;

    default:
      break;
    }
  }
}
