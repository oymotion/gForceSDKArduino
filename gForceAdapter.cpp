/*
 * Copyright 2017, OYMotion Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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

#include "gForceAdapter.h"
#include <math.h>

// Magic number
#define MAGNUM_LOW ((unsigned char)0xFF)
#define MAGNUM_HIGH ((unsigned char)0xAA)

// header length
#define GFORCE_HEADER_LEN ((unsigned char)0x04)

// Indices of data fields
enum {
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
    bool GotGesture(GF_Gesture gesture);
    static inline long FloatToLong(float q) { return (long)(q * (1L << 30)); }

    static inline long MultiplyShift29(long a, long b) {
        return (long)((float)a * b / (1L << 29));
    }

  private:
    const long      m_baudrate = 115200;
    HardwareSerial *m_serial;
    GF_Gesture      m_gestureBuf;
    bool            m_exitGestureFlag;
    bool            m_releaseFlag;
};

GF_Ret GForceAdapterPrivate::Init(void) {
    m_serial->begin(m_baudrate);
    m_serial->setTimeout(5);
    m_exitGestureFlag = false;
    return OK;
}

bool GForceAdapterPrivate::GotGesture(GF_Gesture gesture) {
    GF_Data recData;
    if ((OK == GetGForceData(&recData)) && (recData.type == GF_Data::GESTURE)) {
        if (recData.value.gesture == GF_RELEASE) {
            return false;
        }
        m_exitGestureFlag = true;
        m_gestureBuf      = recData.value.gesture;
        if (gesture == m_gestureBuf) {
            m_exitGestureFlag = false;
            return true;
        }
        return false;
    } else {
        if (m_exitGestureFlag) {
            if (gesture == m_gestureBuf) {
                m_exitGestureFlag = false;
                return true;
            }
        }
        return false;
    }
}

GF_Ret GForceAdapterPrivate::GetGForceData(GF_Data *gForceData) {
    if (NULL == gForceData) {
        return ERR_PARAM;
    }

    int  i = GFORCE_MAGNUM_LOW_INDEX;
    bool hasPackageId;    // package id exists?
    int  dataPkgLen = -1; // length of package data
    char readData;
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
        } else if (i == GFORCE_MAGNUM_HIGH_INDEX) {
            if (tempByte != MAGNUM_HIGH) {
                i = GFORCE_MAGNUM_LOW_INDEX;
                continue;
            }
        } else if (i == GFORCE_EVENT_TYPE_INDEX) {
            hasPackageId     = tempByte & 0x80 ? true : false;
            gForceData->type = (GF_Data::Type)(tempByte & ~0x80);
        } else if (i == GFORCE_MSG_LEN_INDEX) {
            dataPkgLen = (int)tempByte;
            if (hasPackageId) {
                --dataPkgLen;
            }
            if ((GF_Data::QUATERNION == gForceData->type && dataPkgLen != 16) ||
                (GF_Data::GESTURE == gForceData->type && dataPkgLen != 1)) {
                return ERR_DATA;
            }
        } else {
            // skip the package id byte
            if (hasPackageId) {
                hasPackageId = false;
                continue;
            }

            if (gForceData->type == GF_Data::GESTURE) {
                gForceData->value.gesture = (GF_Gesture)tempByte;
            } else {
                *((unsigned char *)&gForceData->value + i - GFORCE_HEADER_LEN) =
                    tempByte;
            }

            if (i == GFORCE_MSG_LEN_INDEX + dataPkgLen) {
                break; // complete
            }
        }
        ++i;
    }

    return OK;
}

///////////////////////////////////////////////////////////////////////////////////////
////////////public function in class gForceAdapter
GF_Ret GForceAdapter::Init(void) { return m_impl->Init(); }

GF_Ret GForceAdapter::GetGForceData(GF_Data *gForceData) {
    return m_impl->GetGForceData(gForceData);
}

bool GForceAdapter::GotGesture(GF_Gesture gesture) {
    return m_impl->GotGesture(gesture);
}

GForceAdapter::GForceAdapter(int comNum) {
    HardwareSerial *serial[4];
#if defined(HAVE_HWSERIAL0)
    serial[0] = &Serial;
#endif
#if defined(HAVE_HWSERIAL1)
    serial[1] = &Serial1;
#else
    serial[1] = &Serial;
#endif
#if defined(HAVE_HWSERIAL2)
    serial[2] = &Serial2;
#else
    serial[2] = &Serial;
#endif
#if defined(HAVE_HWSERIAL3)
    serial[3] = &Serial3;
#else
    serial[3] = &Serial;
#endif
    m_impl = new GForceAdapterPrivate(serial[comNum]);
}

GForceAdapter::GForceAdapter(HardwareSerial *serial) {
    m_impl = new GForceAdapterPrivate(serial);
}

GF_Ret GForceAdapter::QuaternionToEuler(const GF_Quaternion *quat,
                                        GF_Euler *           euler) {
    if ((NULL == quat) || (NULL == euler)) {
        return ERR_PARAM;
    }
    double test = quat->y * quat->z + quat->x * quat->w;
    if (abs(test) > 0.4999f) {
        int symbol   = (test > 0.4999f) ? 1 : -1;
        euler->yaw   = symbol * 2 * atan2f(quat->y, quat->w) * 180 / M_PI;
        euler->pitch = symbol * 90.f;
        euler->roll  = 0.f;
        return OK;
    }
    euler->yaw = atan2f((2 * quat->z * quat->w - 2 * quat->x * quat->y),
                        (1 - 2 * quat->x * quat->x - 2 * quat->z * quat->z)) *
                 180 / M_PI;
    euler->pitch = (float)asin(2 * test) * 180 / M_PI;
    euler->roll  = atan2f((2 * quat->y * quat->w - 2 * quat->x * quat->z),
                         (1 - 2 * quat->x * quat->x - 2 * quat->y * quat->y)) *
                  180 / M_PI;
    return OK;
}
