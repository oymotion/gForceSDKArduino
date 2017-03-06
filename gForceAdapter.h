#ifndef GFORCEADAPTER_H
#define GFORCEADAPTER_H

#include <Arduino.h>

////////////////////////////////////////////////////
////marcos gforce data format
enum
{
    MAGNUM_LOW_INDEX,  // 0xFF
    MAGNUM_HIGH_INDEX, // 0xAA
    EVENT_TYPE_INDEX,  // event type
    MSG_LEN_INDEX,     // avaliable data length
    GFORCE_HEADER_LEN
};

// Magic number
#define MAGNUM_LOW      0xFF
#define MAGNUM_HIGH     0xAA

//max byte of receive message
#define MSG_MAX_SIZE 50

//msg type definde
//#define GFORCE_GESTURE      0x0F
//#define GFORCE_QUATERNION   0x02
typedef enum 
{
    GFORCE_QUATERNION = 0x02,
    GFORCE_GESTURE = 0x0F,
    GFORCE_UNKNOWN // have none available data
} GforceMsg_t;


/**
 * @brief if use Arduino Mega,change the GforceSerial define to com number which you select
 */
#define GforceSerial Serial

/////////////////////////////////////////////////////
///////////enum defines
typedef enum Gesture
{
    RELEASE,
    FIST,
    SPREAD,
    WAVEIN,
    WAVEOUT,
    PINCH,
    SHOOT,
    UNKNOWN,
} Gesture_t;

/**
 * @brief define quaternion struct
 */
typedef struct Quaternion
{
    float w;
    float x;
    float y;
    float z;
} Quaternion_t;

/**
 * @brief define euler struct
 */
typedef struct Euler
{
    float pitch;
    float roll;
    float yaw;
} Euler_t;

/**
 * @breif define avaliable gforce package struct
 */
typedef struct GForceData
{
    GForceMsg_t msgType;
    union {
        Quaternion_t    quaternion;
        Gesture_t       gesture;
    } value;
} GForceData_t;

//#define GFORCE_PI 3.14159265358979
//It is only used in single thread
class GForceAdapter
{
  public:
    GForceAdapter();
    bool UpdateData(void);
    GforceMsg_t GetMsgType(void);
    bool ConvertQuatToEuler(Quaternion_t *quat, Euler_t *euler);
    bool Avaliable(void);
    bool GetAvaliableData(GforceData_t *gForcepkg);

  private:
    bool GetQuaternion(Quaternion_t *quat);
    bool GetGesture(Gesture_t *gesture);
    unsigned char mGforceData[MSG_MAX_SIZE];
    bool mDataAvailable;
    int mReceiveDataIndex;
    /**
       * @brief These function only used in converting quaternion to euler
       *  parameter: q0[in]
       */
    static long FloatToLong(float q);
    static long MultiplyShift29(long a, long b);
};
#endif
