#include <gForceAdapter.h>

#define 	GFORCE_FIST_PIN		2 
#define		GFORCE_SPREAD_PIN	3 
#define		GFORCE_WAVEIN_PIN	4 
#define 	GFORCE_WAVEOUT_PIN	5 
#define		GFORCE_PINCH_PIN	6 
#define		GFORCE_SHOOT_PIN	7

GForceAdapter gforce = GForceAdapter(&Serial);

void setup()
{	
	//set pin mode to output
	pinMode(GFORCE_FIST_PIN, OUTPUT);
	pinMode(GFORCE_SPREAD_PIN, OUTPUT);
	pinMode(GFORCE_WAVEIN_PIN, OUTPUT);
	pinMode(GFORCE_WAVEOUT_PIN, OUTPUT);
	pinMode(GFORCE_PINCH_PIN, OUTPUT);
	pinMode(GFORCE_SHOOT_PIN, OUTPUT);

	// default set output low
	digitalWrite(GFORCE_FIST_PIN, LOW);
	digitalWrite(GFORCE_SPREAD_PIN, LOW);
	digitalWrite(GFORCE_WAVEIN_PIN, LOW);
	digitalWrite(GFORCE_WAVEOUT_PIN, LOW);
	digitalWrite(GFORCE_PINCH_PIN, LOW);
	digitalWrite(GFORCE_SHOOT_PIN, LOW);
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
			digitalWrite(GFORCE_FIST_PIN, HIGH);
                  }
                  else if (gesture == SPREAD)
                  {
                        digitalWrite(GFORCE_SPREAD_PIN, HIGH);
                  }
                  else if (gesture == WAVEIN)
                  {
                        digitalWrite(GFORCE_WAVEIN_PIN, HIGH);
                  }
                  else if (gesture == WAVEOUT)
                  {
                        digitalWrite(GFORCE_WAVEOUT_PIN, HIGH);
                  }
                  else if (gesture == PINCH)
                  {
                        digitalWrite(GFORCE_PINCH_PIN, HIGH);
                  }
                  else if (gesture == SHOOT)
                  {
                        digitalWrite(GFORCE_SHOOT_PIN, HIGH);
                  }
                  else if (gesture == RELEASE)
                  {
                       	digitalWrite(GFORCE_FIST_PIN, LOW);
			digitalWrite(GFORCE_SPREAD_PIN, LOW);
			digitalWrite(GFORCE_WAVEIN_PIN, LOW);
			digitalWrite(GFORCE_WAVEOUT_PIN, LOW);
			digitalWrite(GFORCE_PINCH_PIN, LOW);
			digitalWrite(GFORCE_SHOOT_PIN, LOW);
                  }
                  else if (gesture == UNKNOWN)
                  {
                        digitalWrite(GFORCE_FIST_PIN, HIGH);
			digitalWrite(GFORCE_SPREAD_PIN, HIGH);
			digitalWrite(GFORCE_WAVEIN_PIN, HIGH);
			digitalWrite(GFORCE_WAVEOUT_PIN, HIGH);
			digitalWrite(GFORCE_PINCH_PIN, HIGH);
			digitalWrite(GFORCE_SHOOT_PIN, HIGH);
                  }
                  break;
            default:
                  break;
            }
      }
}
