#include <gForceAdapter.h>

gForceAdapter gforce = gForceAdapter(&Serial);
void setup()
{
      gforce.SetupSerial(115200);
}

void loop()
{
      GForceData_t gForceData;
      if (OK == gforce.GetGForceData(&gForceData))
      {
            
            Gesture_t gesture;
            switch (gForceData.type)
            {
            case GForceData::QUATERNION :
                  break;
            case GForceData::GESTURE :
                  gesture = gForceData.value.gesture;
                  if (gesture == Fist)
                  {
                        Serial.write("Fist\n");
                  }
                  else if (gesture == Spread)
                  {
                        Serial.write("Spread\n");
                  }
                  else if (gesture == WaveIn)
                  {
                        Serial.write("WaveIn\n");
                  }
                  else if (gesture == WaveOut)
                  {
                        Serial.write("WaveOut\n");
                  }
                  else if (gesture == Pinch)
                  {
                        Serial.write("Pinch\n");
                  }
                  else if (gesture == Shoot)
                  {
                        Serial.write("Shoot\n");
                  }
                  else if (gesture == Release)
                  {
                        Serial.write("Release\n");
                  }
                  else if (gesture == Unknown)
                  {
                        Serial.write("Unknown\n");
                  }
                  break;
            default:
                  break;
            }
      }
}
