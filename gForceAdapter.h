#ifndef GFORCEADAPTER_H
#define GFORCEADAPTER_H

#include <memory>
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
enum Gesture
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
struct Quaternion
{
    float w;
    float x;
    float y;
    float z;
} Quaternion_t;

struct Euler
{
    float pitch;
    float roll;
    float yaw;
};

// Definition of gForce package data
typedef struct GForceData
{
    // types
    enum Type
    {
        QUATERNION  = 0x02,
        GESTURE     = 0x0F,
    };

    Type type;
    union {
        Quaternion_t    quaternion;
        Gesture_t       gesture;
    } value;
};

//It is only used in single thread
class GForceAdapterPrivate;
class GForceAdapter
{
  public:
    GForceAdapter(HardwareSerial *serial) : m_impl(new GForceAdapterPrivate(serial)) {}
    ~GForceAdapter() {}

    // Sets up the serial line connection. This function shall be called prior to GetForceData.
    GForceRet SetupSerial(long baudRate);

    // Reads one gForce package data from the serial line and outputs to gForceData.
    GForceRet GetGForceData(GForceData *gForceData);

    // Helper function for converting a quaternion to a Euler angle
    static GForceRet QuaternionToEuler(const Quaternion *quat, Euler *euler);

  private:
    std::unique_ptr<GForceAdapterPrivate> m_impl;

};




#endif
