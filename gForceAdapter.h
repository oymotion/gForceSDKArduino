#ifndef GFORCEADAPTER_H
#define GFORCEADAPTER_H

#include <Arduino.h>
////////////////////////////////////////////////////
////marco define about gforce data protocol
#define MAGNUM_LOW_INDEX      0       // 0xFF
#define MAGNUM_HIGH_INDEX     1       // 0xAA
#define EVENTTYP_INDEX        2       // event type
#define MSG_LEN_INDEX         3       // avaliable data length


#define MAGNUM_LOW           0xFF
#define MAGNUM_HIGH          0xAA

#define GFORCE_HEADER_LEN     0x04
//max byte of receive message
#define MSG_MAX_SIZE          50

//msg type definde
#define GFORCE_GESTURE        0x0F
#define GFORCE_QUATERNION     0x02

/**
 * @brief if use Arduino Mega,change the GforceSerial define to com number which you select
 */
#define GforceSerial          Serial

/////////////////////////////////////////////////////
///////////enum defines
typedef enum Gesture
{
    Release,
    Fist,
    Spread,
    WaveIn,
    WaveOut,
    Pinch,
    Shoot,
    Unknown,
}Gesture_t;

typedef enum {
    Gforce_Quaternion = 0x02,
    Gforce_gesture = 0x0F,
    Gforce_Unknown         // have none available data
} GforceMsg_t;

/**
 * @brief define quaternion struct
 */
typedef struct Quat
{
   float W;
   float X;
   float Y;
   float Z;
} quaternion_t;

/**
 * @brief define euler struct
 */
typedef struct Euler
{
    float Pitch;
    float Roll;
    float Yaw;
} euler_t;

/**
 * @breif define avaliable gforce package struct
 */
typedef struct GforceData{
  GforceMsg_t   msgType;
  union
  {
    quaternion_t quaternion;
    Gesture_t      gesture;
  } value;
}GforceData_t;

#define GFORCE_PI	3.14159265358979
//It is only used in single thread
class gForceAdapter
{
  public:
      gForceAdapter();
      bool updateData(void);
      GforceMsg_t  GetMsgType(void);
      bool ConvertQuatToEuler(quaternion_t *quat,euler_t *euler);
      bool avaliable(void);
      bool GetAvaliableData(GforceData_t *gForcepkg);
  private:
      bool GetQuaternion(quaternion_t *quat);
      bool GetGesture(Gesture_t *gesture);
      unsigned char  mGforceData[MSG_MAX_SIZE];
      bool mDataAvailable;
      int  mReceiveDataIndex;    
      /**
       * @brief These function only used in converting quaternion to euler
       *  parameter: q0[in], data[out]
       */
      long  ConvertFloatToLong(float q0);
      long  MultiplyThenShift29(long a, long b);
};
#endif
