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

// Constants for gestures
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
} GForceData_t;

//It is only used in single thread
class GForceAdapterPrivate;
class GForceAdapter
{
  public:
    GForceAdapter(HardwareSerial *serial) : m_impl(new GForceAdapterPrivate(serial)) {}
    ~GForceAdapter() {}

    // SetupSerial
    GForceRet SetupSerial(long baudRate);
    GForceRet GetGForceData(GForceData_t *gForceData);
    static GForceRet QuaternionToEuler(const Quaternion_t *quat, Euler_t *euler);

  private:
    std::unique_ptr<GForceAdapterPrivate> m_impl;

};




#endif
