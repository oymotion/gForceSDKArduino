
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


#include "gForceAdapterC.h"

#include <math.h>
#include <stdio.h>

#include "gForceAdapter.h"


static GForceAdapter *_adapter = (GForceAdapter*)NULL;


static GForceAdapter* getGForceAdapter()
{
  if (_adapter == NULL)
    _adapter = new GForceAdapter(SYS_GetChar, SYS_GetTick);

  return _adapter;
}


/*  parsing gforcejoint data  */
enum GF_Ret GFC_GetgForcedata(struct GF_Data *gForceData, unsigned long timeout)
{
  return getGForceAdapter()->GetGForceData(gForceData, timeout);
}


/*  Convert quaternions to euler */
enum GF_Ret GFC_QuaternionToEuler(const struct GF_Quaternion *quat, struct GF_Euler *euler)
{
  return getGForceAdapter()->QuaternionToEuler(quat, euler);
}


/*  get gesture */
BOOL GFC_GetGesture(enum GF_Gesture gesture, unsigned long timeout)
{
  return getGForceAdapter()->GotGesture(gesture, timeout);
}



