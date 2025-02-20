
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


#include "gForceAdapterC.h"

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
  GFORCE_MAGNUM_LOW_INDEX, // magic number low byte
  GFORCE_MAGNUM_HIGH_INDEX, // magic number high byte
  GFORCE_EVENT_TYPE_INDEX, // event type
  GFORCE_MSG_LEN_INDEX // avaliable data length
};


static GF_Gesture m_gestureBuf;
static bool m_exitGestureFlag;
static FUNC_GET_MILLIS m_getTimerFunc;
static FUNC_GET_SERIAL_DATA m_getCharFunc;



void GF_Init(FUNC_GET_SERIAL_DATA getCharFunc, FUNC_GET_MILLIS getTimerFunc)
{
  m_exitGestureFlag = false;

  m_getTimerFunc = getTimerFunc;
  m_getCharFunc = getCharFunc;
}


bool GF_GotGesture(GF_Gesture gesture, unsigned long timeout)
{
  GF_Data recData;

  if ((GF_RET_OK == GF_GetGForceData((&recData), timeout)) && (recData.type == GF_GESTURE))
  {
    if (recData.value.gesture == GF_RELEASE)
    {
      return false;
    }

    m_exitGestureFlag = true;
    m_gestureBuf = recData.value.gesture;

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


GF_Ret GF_GetGForceData(GF_Data *gForceData, unsigned long timeout)
{
  if (NULL == gForceData)
  {
    return GF_RET_ERR_PARAM;
  }

  int i = GFORCE_MAGNUM_LOW_INDEX;
  bool hasPackageId; // package id exists?
  int dataPkgLen = -1; // length of package data
  unsigned char readData;
  unsigned long startTime = m_getTimerFunc();

  while (true)
  {
    if (!m_getCharFunc)
      return GF_RET_ERR_DATA;

    int readLen;

    // Read one byte from the serial line
    while ((readLen = m_getCharFunc(&readData)) == 0)
    {
      if ((unsigned long)(m_getTimerFunc() - startTime) > timeout)
      {
        return GF_RET_TIME_OUT;
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
      hasPackageId = ((tempByte & 0x80) != 0);
      gForceData->type = (GF_Data_Type)(tempByte & ~0x80);

      if ((GF_QUATERNION != gForceData->type) &&
          (GF_GESTURE != gForceData->type) &&
          (GF_EMGRAW != gForceData->type))
      {
        return GF_RET_ERR_DATA;
      }
    }
    else if (i == GFORCE_MSG_LEN_INDEX)
    {
      dataPkgLen = (int)tempByte;

      if (hasPackageId)
      {
        --dataPkgLen;
      }

      if ((GF_QUATERNION == gForceData->type && dataPkgLen != 16) ||
          (GF_GESTURE == gForceData->type && dataPkgLen != 1 && dataPkgLen != 5) ||
          (GF_EMGRAW == gForceData->type && dataPkgLen != 16))
      {
        return GF_RET_ERR_DATA;
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

      if (gForceData->type == GF_GESTURE)
      {
        static GF_Gesture prev_gesture = GF_RELEASE;

        if (prev_gesture == (GF_Gesture)tempByte)
        {
          return GF_RET_TIME_OUT;
        }

        prev_gesture = (GF_Gesture)tempByte;
        gForceData->value.gesture = (GF_Gesture)tempByte;
        break; /* ignore other bytes */
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

  return GF_RET_OK;
}


GF_Ret GF_QuaternionToEuler(const GF_Quaternion *quat, GF_Euler *euler)
{
  if ((NULL == quat) || (NULL == euler))
  {
    return GF_RET_ERR_PARAM;
  }

  double test = quat->y * quat->z + quat->x * quat->w;

  if (fabs(test) > 0.4999f)
  {
    int symbol = (test > 0.4999f) ? 1 : -1;
    euler->yaw = symbol * 2 * atan2f(quat->y, quat->w) * 180 / M_PI;
    euler->pitch = symbol * 90.f;
    euler->roll = 0.f;
    return GF_RET_OK;
  }

  euler->yaw = atan2f((2 * quat->z * quat->w - 2 * quat->x * quat->y),
                      (1 - 2 * quat->x * quat->x - 2 * quat->z * quat->z)) *
               180 / M_PI;
  euler->pitch = (float)asin(2 * test) * 180 / M_PI;
  euler->roll = atan2f((2 * quat->y * quat->w - 2 * quat->x * quat->z),
                       (1 - 2 * quat->x * quat->x - 2 * quat->y * quat->y)) *
                180 / M_PI;

  return GF_RET_OK;
}
