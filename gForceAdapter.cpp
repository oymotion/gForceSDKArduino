#include "gForceAdapter.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////////////////////
////////////private function in class gForceAdapter
 bool  gForceAdapter::GetGesture(Gesture_t *gesture)
 {
      if(mDataAvailable) {
          if((mGforceData[EVENTTYP_INDEX] & 0x7F) == GFORCE_GESTURE)
          {
              *gesture = (mGforceData[EVENTTYP_INDEX]& 0x80) ? (Gesture_t)mGforceData[MSG_LEN_INDEX+2] : (Gesture_t)mGforceData[MSG_LEN_INDEX+1];
              return true; 
          } else {
            return false;
          }
      }
      return false;
 }

 bool  gForceAdapter::GetQuaternion(quaternion_t *quat)
 {
      if(mDataAvailable) {
          if((mGforceData[EVENTTYP_INDEX] & 0x7F) == GFORCE_QUATERNION) {
                  unsigned int tmpindex = (mGforceData[EVENTTYP_INDEX]&0x80) ? 2 : 1;
		  quat -> W = *((float*)(&mGforceData[MSG_LEN_INDEX+tmpindex]));
		  quat -> X =*((float*)(&mGforceData[MSG_LEN_INDEX+4+tmpindex]));
		  quat -> Y =*((float*)(&mGforceData[MSG_LEN_INDEX+8+tmpindex]));
		  quat -> Z =*((float*)(&mGforceData[MSG_LEN_INDEX+12+tmpindex]));
          }
          return false;
      }
      return false;
 }

long gForceAdapter::ConvertFloatToLong(float q0)
{
	long retvalue = q0 * (1L<< 30);
	return retvalue;
}


long  gForceAdapter::MultiplyThenShift29(long a, long b)
{
	long result;
	result = (long)((float)a * b/(1L << 29));
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////
////////////public function in class gForceAdapter
/**
 * @brief Constructor-Instantiation gForceAdapter object
 */
gForceAdapter::gForceAdapter()
{
      mDataAvailable = false;
      mReceiveDataIndex = 0; 
      GforceSerial.begin(115200);
}

bool gForceAdapter::updateData(void)
{              
            while(1)
            {
              if(GforceSerial.available()) {
                     int tmpData = GforceSerial.read();
                     if(tmpData == -1) {
                          return false;
                     }
                 mDataAvailable = false;
		 unsigned char ctmpData = (unsigned char)tmpData;
                if(mReceiveDataIndex == MAGNUM_LOW_INDEX) {
                      if(tmpData == MAGNUM_LOW) {
                          mGforceData[mReceiveDataIndex] = ctmpData;
                          mReceiveDataIndex ++;
                      }
                 } else if(mReceiveDataIndex == MAGNUM_HIGH_INDEX) {
                        if(tmpData == MAGNUM_HIGH) {
                            mGforceData[mReceiveDataIndex] = ctmpData;
                            mReceiveDataIndex ++;
                          }
                  } else if(mReceiveDataIndex <=3) {
                            mGforceData[mReceiveDataIndex] = ctmpData;
                            mReceiveDataIndex ++;
                  } else if(mReceiveDataIndex > 3) {
                            mGforceData[mReceiveDataIndex] = ctmpData;
                            mReceiveDataIndex++;
                  if(mReceiveDataIndex == mGforceData[MSG_LEN_INDEX] + GFORCE_HEADER_LEN){
                          mReceiveDataIndex = 0;
                          mDataAvailable = true;
                          return true;
                      }
                 }      
              }
              return false;
            }
}

bool  gForceAdapter::GetAvaliableData(GforceData_t *gForcepkg)
{
    bool bAvaliableFlag = true;
    if(mDataAvailable){
        GforceMsg_t msgtype = GetMsgType();
        switch(msgtype) {
          case Gforce_Quaternion:
                gForcepkg-> msgType = Gforce_Quaternion;
                 GetQuaternion(&((gForcepkg->value).quaternion));
          break;
          case Gforce_gesture:
                gForcepkg-> msgType = Gforce_gesture;
                GetGesture(&((gForcepkg->value).gesture));
          break;
          default:
                gForcepkg-> msgType = Gforce_Unknown;
                bAvaliableFlag = false;
          break;
        }
    } else {
        bAvaliableFlag = false;
    }
    return bAvaliableFlag;
}

bool  gForceAdapter::avaliable(void)
{
  return mDataAvailable;
}

GforceMsg_t gForceAdapter::GetMsgType(void)
{
   if(mDataAvailable) {
        return (GforceMsg_t)(mGforceData[EVENTTYP_INDEX]&0x7F);
   }
   return Gforce_Unknown;
}

bool gForceAdapter::ConvertQuatToEuler(quaternion_t *quat,euler_t *euler)
{
   long t1,t2,t3;
   long q00,q01,q02,q03,q11,q12,q13,q22,q23,q33;
   long quat0,quat1,quat2,quat3;
   quat0 = ConvertFloatToLong(quat->W);
   quat1 = ConvertFloatToLong(quat->X);
   quat2 = ConvertFloatToLong(quat->Y);
   quat3 = ConvertFloatToLong(quat->Z);
   q00 =  MultiplyThenShift29(quat0,quat0);
   q01 =  MultiplyThenShift29(quat0,quat1);
   q02 =  MultiplyThenShift29(quat0,quat2);
   q03 =  MultiplyThenShift29(quat0,quat3);
   q11 =  MultiplyThenShift29(quat1,quat1);
   q12 =  MultiplyThenShift29(quat1,quat2);
   q13 =  MultiplyThenShift29(quat1,quat3);
   q22 =  MultiplyThenShift29(quat2,quat2);
   q23 =  MultiplyThenShift29(quat2,quat3);
   q33 =  MultiplyThenShift29(quat3,quat3);

   /* X component of the Ybody axis in World frame */
   t1 = q12 - q03;

	/* Y component of the Ybody axis in World frame */
	t2 = q22 + q00 - (1L << 30);
	euler -> Yaw = -atan2f((float) t1, (float) t2) * 180.f / (float) GFORCE_PI;

	/* Z component of the Ybody axis in World frame */
	t3 = q23 + q01;
	euler -> Pitch = atan2f((float) t3, sqrtf((float) t1 * t1 + (float) t2 * t2)) * 180.f / (float) GFORCE_PI;
	/* Z component of the Zbody axis in World frame */
	t2 = q33 + q00 - (1L << 30);
	if (t2 < 0) {
		if (euler -> Pitch >= 0)
			euler -> Pitch = 180.f - (euler->Pitch);
		else
			euler -> Pitch = -180.f - (euler->Pitch);
	}

	/* X component of the Xbody axis in World frame */
	t1 = q11 + q00 - (1L << 30);
	/* Y component of the Xbody axis in World frame */
	t2 = q12 + q03;
	/* Z component of the Xbody axis in World frame */
	t3 = q13 - q02;

	euler->Roll = (atan2((float) (q33 + q00 - (1L << 30)), (float) (q13 - q02)) * 180.f / (float) GFORCE_PI - 90);
	if (euler->Roll >= 90)
		euler->Roll = 180 - euler->Roll;

	if (euler->Roll < -90)
		euler->Roll = -180 - euler->Roll;
	return true;
}
