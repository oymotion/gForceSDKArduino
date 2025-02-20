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


#ifndef GFORCEADAPTER_H
#define GFORCEADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gForceDataTypes.h"

#include <stdbool.h>


///
/// Brief Sets up the serial line connection. This function shall be called
/// prior to GetForceData.
///
void GF_Init(FUNC_GET_SERIAL_DATA getCharFunc, FUNC_GET_MILLIS getTimerFunc);


///
/// Reads one gForce package data from the serial line and outputs to
/// gForceData.
///
/// \param[out] gForceData The GF_Data structure to store gForceData.
/// \return
GF_Ret GF_GetGForceData(GF_Data *gForceData, unsigned long timeout);


///
/// Checks if a specified gesture is received.
/// This function is used by the Scratch plugin.
///
/// \param[in] gesture The specified gesture to check
/// \return true if the specified gesture is received; otherwise false
bool GF_GotGesture(GF_Gesture gesture, unsigned long timeout);


// Helper function for converting a quaternion to a Euler angle
GF_Ret GF_QuaternionToEuler(const GF_Quaternion *quat, GF_Euler *euler);


#ifdef __cplusplus
}
#endif

#endif
