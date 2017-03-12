#ifndef GFORCEADAPTER_H
#define GFORCEADAPTER_H

#include <Arduino.h>

////////////////////////////////////////////////////

// Function returns
enum GForceRet {
    OK              = 0,
    ERR_PARAM       = -1,
    ERR_SERIAL      = -2,
    ERR_DATA        = -3
};

// Type and constants for gestures
enum GF_Gesture
{
    RELEASE,    // 0
    FIST,
    SPREAD,
    WAVEIN,
    WAVEOUT,
    PINCH,
    SHOOT,
    UNKNOWN,
};


// Definitions of Quaternion and Euler Angle
struct GF_Quaternion
{
    float w;
    float x;
    float y;
    float z;
};

struct GF_Euler
{
    float pitch;
    float roll;
    float yaw;
};

// Definition of gForce package data
struct GF_GForceData
{
    // message type
    enum Type
    {
        QUATERNION  = 0x02,
        GESTURE     = 0x0F,
    };

    Type type;
    union {
        GF_Quaternion   quaternion;
        GF_Gesture       gesture;
    } value;
};

//It is only used in single thread
class GForceAdapterPrivate;
class GForceAdapter
{
  public:
    GForceAdapter(HardwareSerial *serial) ;
    ~GForceAdapter() {}

    // Sets up the serial line connection. This function shall be called prior to GetForceData.
    GForceRet Init(void);

    // Reads one gForce package data from the serial line and outputs to gForceData.
    GForceRet GetGForceData(GF_GForceData *gForceData);

    // Helper function for converting a quaternion to a Euler angle
    static GForceRet QuaternionToEuler(const GF_Quaternion *quat, GF_Euler *euler);

  private:
    GForceAdapterPrivate *m_impl;

};

#endif
