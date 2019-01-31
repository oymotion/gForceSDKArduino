
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

/*
 * see "https://oymotion.github.io/gForceEmbeddedSuit/gForce100EmbeddedSuiteUserGuide" for protocol
 */


#include "gForceAdapter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NULL
#define NULL ((void*)0)
#endif


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// Magic number
#define MAGNUM_LOW ((unsigned char)0xFF)
#define MAGNUM_HIGH ((unsigned char)0xAA)

// header length
#define GFORCE_HEADER_LEN ((unsigned char)0x04)


// Indices of data fields
enum
{
  GFORCE_MAGNUM_LOW_INDEX,  // magic number low byte
  GFORCE_MAGNUM_HIGH_INDEX, // magic number high byte
  GFORCE_EVENT_TYPE_INDEX,  // event type
  GFORCE_MSG_LEN_INDEX      // avaliable data length
};


class GForceAdapterPrivate
{
public:
  //GForceAdapterPrivate(HardwareSerial *serial) : m_serial(serial) {}
  GForceAdapterPrivate(FUNC_GET_SERIAL_DATA getCharFunc, FUNC_GET_MILLIS getTimerFunc)
  {
    m_getCharFunc = getCharFunc;
    m_getTimerFunc = getTimerFunc;
  }
  ~GForceAdapterPrivate() {}

  GF_Ret Init(void);
  GF_Ret GetGForceData(GF_Data *gForceData, unsigned long timeout);
  bool GotGesture(GF_Gesture gesture, unsigned long timeout);

private:
  GF_Gesture      m_gestureBuf;
  bool            m_exitGestureFlag;
  FUNC_GET_MILLIS     m_getTimerFunc;
  FUNC_GET_SERIAL_DATA m_getCharFunc;
};


GF_Ret GForceAdapterPrivate::Init(void)
{
  m_exitGestureFlag = false;
  return OK;
}


bool GForceAdapterPrivate::GotGesture(GF_Gesture gesture, unsigned long timeout)
{
  GF_Data recData;

  if ((OK == GetGForceData((&recData), timeout)) && (recData.type == GF_Data::GESTURE))
  {
    if (recData.value.gesture == GF_RELEASE)
    {
      return false;
    }

    m_exitGestureFlag = true;
    m_gestureBuf      = recData.value.gesture;

    if (gesture == m_gestureBuf)
    {
      m_exitGestureFlag = false;
      return true;
    }

    return false;
  }
  else
  {
    if (m_exitGestureFlag)
    {
      if (gesture == m_gestureBuf)
      {
        m_exitGestureFlag = false;
        return true;
      }
    }

    return false;
  }
}


GF_Ret GForceAdapterPrivate::GetGForceData(GF_Data *gForceData, unsigned long timeout)
{
  if (NULL == gForceData)
  {
    return ERR_PARAM;
  }

  int  i = GFORCE_MAGNUM_LOW_INDEX;
  bool hasPackageId;    // package id exists?
  int  dataPkgLen = -1; // length of package data
  unsigned char readData;
  unsigned long startTime = m_getTimerFunc();

  while (true)
  {
    if (!m_getCharFunc)
      return ERR_DATA;

    int readLen;

    // Read one byte from the serial line
    while ((readLen = m_getCharFunc(&readData)) == 0)
    {
      if((unsigned long)(m_getTimerFunc() - startTime) > timeout)
      {
        return TIME_OUT;
      }
    }

    unsigned char tempByte = (unsigned char)readData;

    if (i == GFORCE_MAGNUM_LOW_INDEX)
    {
      if (tempByte != MAGNUM_LOW)
      {
        continue;
      }
    }
    else if (i == GFORCE_MAGNUM_HIGH_INDEX)
    {
      if (tempByte != MAGNUM_HIGH)
      {
        i = GFORCE_MAGNUM_LOW_INDEX;
        continue;
      }
    }
    else if (i == GFORCE_EVENT_TYPE_INDEX)
    {
      hasPackageId     = ((tempByte & 0x80) != 0);
      gForceData->type = (GF_Data::Type)(tempByte & ~0x80);

      if ((GF_Data::QUATERNION != gForceData->type) &&
          (GF_Data::GESTURE != gForceData->type) &&
          (GF_Data::EMGRAW != gForceData->type))
      {
        return ERR_DATA;
      }
    }
    else if (i == GFORCE_MSG_LEN_INDEX)
    {
      dataPkgLen = (int)tempByte;

      if (hasPackageId)
      {
        --dataPkgLen;
      }

      if ((GF_Data::QUATERNION == gForceData->type && dataPkgLen != 16) ||
          (GF_Data::GESTURE == gForceData->type && dataPkgLen != 1) ||
          (GF_Data::EMGRAW == gForceData->type && dataPkgLen != 16))
      {
        return ERR_DATA;
      }
    }
    else
    {
      // skip the package id byte
      if (hasPackageId)
      {
        hasPackageId = false;
        continue;
      }

      if (gForceData->type == GF_Data::GESTURE)
      {
        gForceData->value.gesture = (GF_Gesture)tempByte;
      }
      else
      {
        *((unsigned char *)&gForceData->value + i - GFORCE_HEADER_LEN) = tempByte;
      }

      dataPkgLen--;

      if (dataPkgLen == 0)
        break;
    }

    i++;
  }

  return OK;
}


///////////////////////////////////////////////////////////////////////////////////////
////////////public function in class gForceAdapter
GF_Ret GForceAdapter::Init(void)
{
  return m_impl->Init();
}


GF_Ret GForceAdapter::GetGForceData(GF_Data *gForceData, unsigned long timeout)
{
  return m_impl->GetGForceData(gForceData, timeout);
}


bool GForceAdapter::GotGesture(GF_Gesture gesture, unsigned long timeout)
{
  return m_impl->GotGesture(gesture, timeout);
}


GForceAdapter::GForceAdapter(FUNC_GET_SERIAL_DATA getCharFunc, FUNC_GET_MILLIS getTimerFunc)
{
  m_impl = new GForceAdapterPrivate(getCharFunc, getTimerFunc);
}


GF_Ret GForceAdapter::QuaternionToEuler(const GF_Quaternion *quat,
                                        GF_Euler            *euler)
{
  if ((NULL == quat) || (NULL == euler))
  {
    return ERR_PARAM;
  }

  double test = quat->y * quat->z + quat->x * quat->w;

  if (abs(test) > 0.4999f)
  {
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
