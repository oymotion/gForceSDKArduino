

#ifndef _GFORCE_DATA_TYPES_H_
#define _GFORCE_DATA_TYPES_H_


#define Quaternion 0x02
#define Gesture    0x0F
#define Emgraw     0x12


////////////////////////////////////////////////////
// Function returns
enum GF_Ret { OK = 0, ERR_PARAM = -1, TIME_OUT = -2, ERR_DATA = -3 };

// Type and constants for gestures
enum GF_Gesture
{
  GF_RELEASE,
  GF_FIST,
  GF_SPREAD,
  GF_WAVEIN,
  GF_WAVEOUT,
  GF_PINCH,
  GF_SHOOT,
  GF_SEFDEF1,
  GF_SEFDEF2,
  GF_UNKNOWN = -1
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

struct GF_Emgraw
{
  unsigned char raw[16];
};

// Definition of gForce package data
struct GF_Data
{
  // message type
  enum Type
  {
    QUATERNION = 0x02,
    GESTURE    = 0x0F,
    EMGRAW     = 0x12
  } type;

  union
  {
    struct GF_Quaternion quaternion;
    enum   GF_Gesture    gesture;
    struct GF_Emgraw     emgrawData;
  } value;
};


typedef int (*FUNC_GET_SERIAL_DATA)(unsigned char *data);
typedef unsigned long (*FUNC_GET_MILLIS)(void);

#endif /*_GFORCE_DATA_TYPES_H_*/
