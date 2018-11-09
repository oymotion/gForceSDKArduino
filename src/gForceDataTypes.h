

#ifndef _GFORCE_DATA_TYPES_H_
#define _GFORCE_DATA_TYPES_H_


#define Quaternion 0x02
#define Gesture    0x0F
#define Emgraw     0x12



#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)

////////////////////////////////////////////////////
// Function returns
enum GF_Ret { OK = 0, ERR_PARAM = -1, TIME_OUT = -2, ERR_DATA = -3 };

// Type and constants for gestures
enum GF_Gesture {
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
struct GF_Quaternion {
    float w;
    float x;
    float y;
    float z;
};

struct GF_Euler {
    float pitch;
    float roll;
    float yaw;
};

struct GF_Emgraw{
   unsigned char raw[16]; 
};

// Definition of gForce package data
struct GF_Data {
    // message type
    enum Type {
        QUATERNION = 0x02,
        GESTURE    = 0x0F,
        EMGRAW     = 0x12
    } type;

    union {
        struct GF_Quaternion quaternion;
        enum   GF_Gesture    gesture;   
        struct GF_Emgraw     emgrawData;
    } value;
};


typedef int (*FUNC_GET_SERIAL_DATA)(unsigned char *data);
typedef unsigned long (*FUNC_GET_MILLIS)(void);

#endif /*_GFORCE_DATA_TYPES_H_*/
