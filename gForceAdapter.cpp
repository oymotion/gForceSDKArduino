/*
 * Copyright 2017, OYMotion Inc.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are me * t:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in  
 *    the documentation and/or other materials provided with the 
 *    distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE.
 *
 */

#include <math.h>
#include "gForceAdapter.h"


// Magic number
#define MAGNUM_LOW            ((unsigned char)0xFF)
#define MAGNUM_HIGH           ((unsigned char)0xAA)

//header length
#define GFORCE_HEADER_LEN     ((unsigned char)0x04)

// Indices of data fields
enum
{
    GFORCE_MAGNUM_LOW_INDEX,  // magic number low byte
    GFORCE_MAGNUM_HIGH_INDEX, // magic number high byte
    GFORCE_EVENT_TYPE_INDEX,  // event type
    GFORCE_MSG_LEN_INDEX      // avaliable data length
};


class GForceAdapterPrivate {
  public:
    GForceAdapterPrivate(HardwareSerial *serial) : m_serial(serial) {}
    ~GForceAdapterPrivate() {}
    
    GF_Ret Init(void);
    GF_Ret GetGForceData(GF_Data *gForceData);

    static inline long FloatToLong(float q) {
        return (long)(q * (1L << 30));
    }

    static inline long MultiplyShift29(long a, long b) {
        return (long)((float)a * b / (1L << 29));
    }
  private:
    const int         m_baudrate = 115200;
    HardwareSerial    *m_serial;
};

GF_Ret GForceAdapterPrivate::Init(void)
{
    m_serial->begin(m_baudrate);
    m_serial->setTimeout(1);        
    return OK;
}

GF_Ret GForceAdapterPrivate::GetGForceData(GF_Data *gForceData)
{
    if (NULL == gForceData) {
        return ERR_PARAM;
    }

    int                 i = GFORCE_MAGNUM_LOW_INDEX; 
    bool                hasPackageId;       // package id exists?
    int                 dataPkgLen = -1;    // length of package data
    char		readData;
    while (true) {
        // Read one byte from the serial line
        int readLen = m_serial->readBytes(&readData, 1);
        if (1 != readLen) {
            return ERR_DATA;
        }
 
        unsigned char tempByte = (unsigned char)readData;

        if (i == GFORCE_MAGNUM_LOW_INDEX) {
            if (tempByte != MAGNUM_LOW) {
                continue;
            }
        }
        else if (i == GFORCE_MAGNUM_HIGH_INDEX) {
            if (tempByte != MAGNUM_HIGH) {
                i = GFORCE_MAGNUM_LOW_INDEX;
                continue;
            }
        }
        else if (i == GFORCE_EVENT_TYPE_INDEX) {
            hasPackageId = tempByte & 0x80 ? true : false;
            gForceData->type = (GF_Data::Type)(tempByte & ~0x80);
        }
        else if (i == GFORCE_MSG_LEN_INDEX) {
            dataPkgLen = (int)tempByte;
            if (hasPackageId) {
                -- dataPkgLen;
            }
            if ((GF_Data::QUATERNION == gForceData->type && dataPkgLen != 16) ||
                (GF_Data::GESTURE    == gForceData->type && dataPkgLen !=  1)) {
                return ERR_DATA;
            }
        }
        else {
            // skip the package id byte
            if (hasPackageId) { 
                hasPackageId = false;
                continue;
            }

            *((unsigned char*)&gForceData->value + i - GFORCE_HEADER_LEN) = tempByte;
            
            if (i == GFORCE_MSG_LEN_INDEX + dataPkgLen) {
                break; // complete
            }
        } 
        ++ i;
    }

    return OK;
    
}

///////////////////////////////////////////////////////////////////////////////////////
////////////public function in class gForceAdapter
GF_Ret GForceAdapter::Init(void) {
    return m_impl->Init();
}

GF_Ret GForceAdapter::GetGForceData(GF_Data *gForceData) {
    return m_impl->GetGForceData(gForceData);
} 

GForceAdapter::GForceAdapter (HardwareSerial *serial)
{
    m_impl = new GForceAdapterPrivate(serial) ;
}

GF_Ret GForceAdapter::QuaternionToEuler(const GF_Quaternion *quat, GF_Euler *euler)
{
    if (NULL != quat || NULL != euler) {
        return ERR_PARAM;
    }

    long t1, t2, t3;
    long q00, q01, q02, q03, q11, q12, q13, q22, q23, q33;
    long quat0, quat1, quat2, quat3;
    quat0 = GForceAdapterPrivate::FloatToLong(quat->w);
    quat1 = GForceAdapterPrivate::FloatToLong(quat->x);
    quat2 = GForceAdapterPrivate::FloatToLong(quat->y);
    quat3 = GForceAdapterPrivate::FloatToLong(quat->z);
    q00 = GForceAdapterPrivate::MultiplyShift29(quat0, quat0);
    q01 = GForceAdapterPrivate::MultiplyShift29(quat0, quat1);
    q02 = GForceAdapterPrivate::MultiplyShift29(quat0, quat2);
    q03 = GForceAdapterPrivate::MultiplyShift29(quat0, quat3);
    q11 = GForceAdapterPrivate::MultiplyShift29(quat1, quat1);
    q12 = GForceAdapterPrivate::MultiplyShift29(quat1, quat2);
    q13 = GForceAdapterPrivate::MultiplyShift29(quat1, quat3);
    q22 = GForceAdapterPrivate::MultiplyShift29(quat2, quat2);
    q23 = GForceAdapterPrivate::MultiplyShift29(quat2, quat3);
    q33 = GForceAdapterPrivate::MultiplyShift29(quat3, quat3);

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

    euler->roll = atan2f(((float)(q33 + q00 - (1L << 30))), (float)(q13 - q02) * 180.f / (PI - 90));
    if (euler->roll >= 90) {
        euler->roll = 180 - euler->roll;
    }

    if (euler->roll < -90) {
        euler->roll = -180 - euler->roll;
    }
    return OK;
}
