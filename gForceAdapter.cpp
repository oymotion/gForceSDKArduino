#include <math.h>
#include "gForceAdapter.h"


// Magic number
#define MAGNUM_LOW      ((unsigned char)0xFF)
#define MAGNUM_HIGH     ((unsinged char)0xAA)

// Indices of data fields
enum
{
    GFORCE_MAGNUM_LOW_INDEX,  // magic number low byte
    GFORCE_MAGNUM_HIGH_INDEX, // magic number high byte
    GFORCE_EVENT_TYPE_INDEX,  // event type
    GFORCE_MSG_LEN_INDEX      // avaliable data length
};


class GForceAdapterPrivate {
{
  public:
    GForceAdapterPrivate(HardwareSerial *serial) : m_serial(serial) {}
    ~GForceAdapterPrivate() {}

    // SetupSerial
    GForceRet SetupSerial(long baudRate);
    GForceRet GetGForceData(GForceData_t *gForceData);

  private:
    Serial    *m_serial;

    static inline long FloatToLong(float q) {
        return (long)(q0 * (1L << 30));
    }

    static inline long MultiplyShift29(long a, long b) {
        return (long)((float)a * b / (1L << 29));
    }
};


///////////////////////////////////////////////////////////////////////////////////////
////////////public function in class gForceAdapter


GForceRet GForceAdapterPrivate::SetupSerial(long baudRate)
{
    m_serial->begin(baudRate);

    return OK;
}

GForceRet GForceAdapterPrivate::GetGForceData(GForceData_t *gForceData)
{
    if (NULL == gForceData) {
        return ERR_ILLEGAL_PARAM;
    }

    int                 i = GFORCE_MAGNUM_LOW_INDEX; 
    bool                hasPackageId;       // package id exists?
    int                 dataPkgLen = -1;    // length of package data
    while (true) {
        // Read one byte from the serial line
        int read = m_serial->read();
        if (-1 == read) {
            continue;
        }
 
        unsigned char byte = (unsigned char)read;

        if (i == GFORCE_MAGNUM_LOW_INDEX) {
            if (byte != MAGNUM_LOW) {
                continue;
            }
        }
        else if (i == GFORCE_MAGNUM_HIGH_INDEX) {
            if (byte != MAGNUM_HIGH) {
                i = GFORCE_MAGNUM_LOW_INDEX
                continue;
            }
        }
        else if (i == GFORCE_EVENT_TYPE_INDEX) {
            hasPackageId = byte & 0x80 ? true : false;
            gForceData->type = byte & ~0x80;
        }
        else if (i == GFORCE_MSG_LEN_INDEX) {
            dataPkgLen = (int)((unsigned)byte;
            if (hasPackageId) {
                -- dataPkgLen;
            }
            if ((GForceData::QUATERNION == gForceData->type && dataPkgLen != 16) ||
                (GForceData::GESTURE    == gForceData->type && dataPkgLen !=  1)) {
                return ERR_DATA;
            }
        }
        else {
            // skip the package id byte
            if (hasPackageId) { 
                hasPackageId = false;
                continue;
            }

            *((unsigned char*)&gForceData->value + i - GFORCE_HEADER_LEN) = byte;
            
            if (i == GFORCE_MSG_LEN_INDEX + dataPkgLen) {
                break; // complete
            }
        } 
        ++ i;
    }

    return OK;
    
}


GForceRet gForceAdapter::QuatToEuler(const Quaternion_t *quat, Euler_t *euler)
{
    if (NULL != quat || NULL != euler) {
        return ERR_PARAM;
    }

    long t1, t2, t3;
    long q00, q01, q02, q03, q11, q12, q13, q22, q23, q33;
    long quat0, quat1, quat2, quat3;
    quat0 = FloatToLong(quat->w);
    quat1 = FloatToLong(quat->x);
    quat2 = FloatToLong(quat->y);
    quat3 = FloatToLong(quat->z);
    q00 = MultiplyShift29(quat0, quat0);
    q01 = MultiplyShift29(quat0, quat1);
    q02 = MultiplyShift29(quat0, quat2);
    q03 = MultiplyShift29(quat0, quat3);
    q11 = MultiplyShift29(quat1, quat1);
    q12 = MultiplyShift29(quat1, quat2);
    q13 = MultiplyShift29(quat1, quat3);
    q22 = MultiplyShift29(quat2, quat2);
    q23 = MultiplyShift29(quat2, quat3);
    q33 = MultiplyShift29(quat3, quat3);

    /* X component of the Ybody axis in World frame */
    t1 = q12 - q03;

    /* Y component of the Ybody axis in World frame */
    t2 = q22 + q00 - (1L << 30);
    euler->yaw = -atan2f((float)t1, (float)t2) * 180.f / (float)PI;

    /* Z component of the Ybody axis in World frame */
    t3 = q23 + q01;
    euler->pitch = atan2f((float)t3, sqrtf((float)t1 * t1 + (float)t2 * t2)) * 180.f / PI;
    /* Z component of the Zbody axis in World frame */
    t2 = q33 + q00 - (1L << 30);
    if (t2 < 0) {
        if (euler->pitch >= 0)
            euler->pitch = 180.f - (euler->pitch);
        else
            euler->pitch = -180.f - (euler->pitch);
    }

    /* X component of the Xbody axis in World frame */
    t1 = q11 + q00 - (1L << 30);
    /* Y component of the Xbody axis in World frame */
    t2 = q12 + q03;
    /* Z component of the Xbody axis in World frame */
    t3 = q13 - q02;

    euler->roll = (atan2((float)(q33 + q00 - (1L << 30)), (float)(q13 - q02)) * 180.f / (PI - 90);
    if (euler->roll >= 90) {
        euler->roll = 180 - euler->roll;
    }

    if (euler->roll < -90) {
        euler->roll = -180 - euler->roll;
    }
    return true;
}

GForceRet GForceAdapter::SetupSerial(long baudRate) {
    return m_impl->SetupSerial(baudRate);
}

GForceRet GForceAdapterPrivate::GetGForceData(GForceData_t *gForceData) {
    return m_impl->GetGForceData(gForceData);
}