
// Glove Control Program
// Suported board: Arduino Mega 2560, Generic STM32F1 Series BluePill F103C8
//

#include <Arduino.h>

#include "gForceAdapter.h"

#ifdef ARDUINO_ARCH_STM32F1

  #define SerialToDevice Serial3 // PB10: TX, PB11: RX
  // #define SerialToGForce Serial2  // PA2 : TX, PA3 : RX (Only RX needed)
  #define SerialToGForce Serial1 // PA9 : TX, PA10: RX (Only RX needed)

  const int LED = PB1; // On when high

  #define LED_ON_STATE 1

#else

  #define SerialToDevice Serial1
  #define SerialToGForce Serial2

  const int LED = 13; // On when high

  #define LED_ON_STATE 1

#endif

#define DEGRES_OF_FREEDOM 6

#define USE_GFORCE
#define GFORCE_TIMEOUT 10

/* returns System time in milli seconds, used by gForce */
unsigned long SYS_GetTick(void)
{
  return millis();
}

/* read function used by gForce, returns char count */
int SYS_GetChar(unsigned char *data)
{
  int ret = SerialToGForce.read();

  if (ret == -1)
    return 0;

  *data = (unsigned char)ret;

  return 1;
}

GForceAdapter gForce(SYS_GetChar, SYS_GetTick);

/* FSR channels of ADC in sequence of "thumb, index, middle, ring, pinky, thumb
 * rotation" */
const int INDEX_CHANNELS[DEGRES_OF_FREEDOM] = {7, 6, 0, 3, 4, 5};

float rawData[DEGRES_OF_FREEDOM];
float minData[DEGRES_OF_FREEDOM] = {255, 255, 255, 255, 255, 255};
float maxData[DEGRES_OF_FREEDOM] = {0, 0, 0, 0, 0, 0};

bool getRawData(float rawData[])
{
  GF_Data gForceData;
  GF_Emgraw emgrawData;

  if (GF_Ret::GF_RET_OK == gForce.GetGForceData((&gForceData), GFORCE_TIMEOUT))
  {
    if (gForceData.type == GF_Data_Type::GF_EMGRAW)
    {
      emgrawData = gForceData.value.emgrawData;

      for (int i = 0; i < DEGRES_OF_FREEDOM; i++)
      {
        int channel = INDEX_CHANNELS[i];
        rawData[i] = (rawData[i] + emgrawData.raw[channel] + emgrawData.raw[channel + 8]) / 3;
      }

      return true;
    }
  }

  return false;
}

void setup()
{
  uint8_t err;

  pinMode(LED, OUTPUT);
  digitalWrite(LED, !LED_ON_STATE);

  Serial.begin(115200); // start serial for output
  // while (!Serial);

  SerialToGForce.begin(115200); // init SerialToGForce

  //
  // Wait for BLE connected

  GF_Data gForceData;

  while (GF_Ret::GF_RET_OK != gForce.GetGForceData((&gForceData), GFORCE_TIMEOUT))
  {
    delay(100);
    Serial.println(F("Waiting for BLE..."));
  }

  Serial.println(F("Please fist when led is on."));

  digitalWrite(LED, LED_ON_STATE);
  uint32_t startTime = millis();

  while ((uint32_t)(millis() - startTime) < 5000)
  {
    if (getRawData(rawData))
    {
      for (int i = 0; i < DEGRES_OF_FREEDOM; i++)
      {
        minData[i] = (rawData[i] + minData[i]) / 2;
        /*
          Serial.print(F("finger, raw data, min: "));
          Serial.print(i);
          Serial.print(F(", "));
          Serial.print(rawData[i]);
          Serial.print(F(", "));
          Serial.println(minData[i]);
        */
      }
    }
  }

  Serial.println(F("Please spread when led is off."));

  startTime = millis();
  digitalWrite(LED, !LED_ON_STATE);

  while ((uint32_t)(millis() - startTime) < 5000)
  {
    if (getRawData(rawData))
    {
      for (int i = 0; i < DEGRES_OF_FREEDOM; i++)
      {
        maxData[i] = (rawData[i] + maxData[i]) / 2;
        /*
          Serial.print(F("finger, raw data, max: "));
          Serial.print(i);
          Serial.print(F(", "));
          Serial.print(rawData[i]);
          Serial.print(F(", "));
          Serial.println(maxData[i]);
        */
      }
    }
  }

  digitalWrite(LED, LED_ON_STATE);

  for (int i = 0; i < DEGRES_OF_FREEDOM; i++)
  {
    Serial.print(F("finger, min, max: "));
    Serial.print(i);
    Serial.print(F(", "));
    Serial.print(minData[i]);
    Serial.print(F(", "));
    Serial.println(maxData[i]);
  }

  digitalWrite(LED, LED_ON_STATE);

  Serial.println(F("Begin."));
}

void loop()
{
  uint8_t i;
  uint8_t err;

  // digitalWrite(LED, LED_ON_STATE);
  // delay(1000);
  // digitalWrite(LED, !LED_ON_STATE);
  // delay(1000);
  // return;

  // static uint32_t lastMillis = 0;

  // Serial.print("curr time: ");
  // Serial.println(millis());

  // 手套模式控制

  if (getRawData(rawData))
  {
    for (int i = 0; i < DEGRES_OF_FREEDOM; i++)
    {
      Serial.print(F("finger, mapped pos: "));
      Serial.print(i);
      Serial.print(F(", "));
      Serial.println(map(rawData[i], minData[i], maxData[i], 0, 100));
    }
  }
}
