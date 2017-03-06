#include <gForceAdapter.h>

gForceAdapter gforce = gForceAdapter();
void setup() {
  
}

void loop() {
      if(gforce.updateData()) {
                GforceData_t  gForceData;
                gforce.GetAvaliableData(&gForceData);
                GforceMsg_t msgtype = gForceData.msgType;
                 Gesture_t gesture;
                switch (msgtype){
                      case Gforce_Quaternion:
                          break;
                      case Gforce_gesture:
                           gesture = gForceData.value.gesture;
                          if(gesture == Fist) {
                                Serial.write("Fist\n"); 
                          } else if(gesture == Spread) {
                                Serial.write("Spread\n");
                          } else if(gesture == WaveIn) {
                                Serial.write("WaveIn\n");  
                          } else if(gesture == WaveOut) {
                                 Serial.write("WaveOut\n"); 
                          } else if(gesture == Pinch){
                                  Serial.write("Pinch\n");
                          } else if(gesture == Shoot) {
                                  Serial.write("Shoot\n");
                          } else if(gesture == Release) {
                                  Serial.write("Release\n");
                          } else if(gesture == Unknown) {
                                  Serial.write("Unknown\n");
                          }
                          break;
                      default:
                           break;
                }
      }
}
