

#ifndef _GFORCE_DATA_TYPES_H_
#define _GFORCE_DATA_TYPES_H_


#define Quaternion 0x02
#define Gesture    0x0F
#define Emgraw     0x12


////////////////////////////////////////////////////
// Function returns
typedef enum { GF_RET_OK = 0, GF_RET_ERR_PARAM = -1, GF_RET_TIME_OUT = -2, GF_RET_ERR_DATA = -3 } GF_Ret;

// Type and constants for gestures
typedef enum
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
} GF_Gesture;

// Definitions of Quaternion and Euler Angle
typedef struct
{
  float w;
  float x;
  float y;
  float z;
} GF_Quaternion;

typedef struct
{
  float pitch;
  float roll;
  float yaw;
} GF_Euler;

typedef struct
{
  unsigned char raw[16];
} GF_Emgraw;

// message type
typedef enum
{
  GF_QUATERNION = 0x02,
  GF_GESTURE = 0x0F,
  GF_EMGRAW = 0x12
} GF_Data_Type;

// Definition of gForce package data
typedef struct
{
  GF_Data_Type type;

  union
  {
    GF_Quaternion quaternion;
    GF_Gesture gesture;
    GF_Emgraw emgrawData;
  } value;
} GF_Data;


typedef int (*FUNC_GET_SERIAL_DATA)(unsigned char *data);
typedef unsigned long (*FUNC_GET_MILLIS)(void);

#endif /*_GFORCE_DATA_TYPES_H_*/
