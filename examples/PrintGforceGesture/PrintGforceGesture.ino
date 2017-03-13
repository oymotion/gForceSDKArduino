#include <gForceAdapter.h>

GForceAdapter gforce = GForceAdapter(&Serial);
void setup()
{
      gforce.Init();
}

void loop()
{
      GF_Data gForceData;
      if (OK == gforce.GetGForceData(&gForceData))
      {
            
            GF_Gesture gesture;
            switch (gForceData.type)
            {
            case GF_Data::QUATERNION :
                  break;
            case GF_Data::GESTURE :
                  gesture = gForceData.value.gesture;
                  if (gesture == FIST)
                  {
                        Serial.write("Fist\n");
                  }
                  else if (gesture == SPREAD)
                  {
                        Serial.write("Spread\n");
                  }
                  else if (gesture == WAVEIN)
                  {
                        Serial.write("WaveIn\n");
                  }
                  else if (gesture == WAVEOUT)
                  {
                        Serial.write("WaveOut\n");
                  }
                  else if (gesture == PINCH)
                  {
                        Serial.write("Pinch\n");
                  }
                  else if (gesture == SHOOT)
                  {
                        Serial.write("Shoot\n");
                  }
                  else if (gesture == RELEASE)
                  {
                        Serial.write("Release\n");
                  }
                  else if (gesture == UNKNOWN)
                  {
                        Serial.write("Unknown\n");
                  }
                  break;
            default:
                  break;
            }
      }
}
