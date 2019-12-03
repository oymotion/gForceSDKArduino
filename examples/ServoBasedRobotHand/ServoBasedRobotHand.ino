/*
   Copyright 2019, OYMotion Inc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
   DAMAGE.

*/

#include <Servo.h>
#include <gForceAdapter.h>

// Define USE_GFORCE to 1 to control hand using gForce.
// Define USE_GFORCE to 0 to show gestures one by one.
#define USE_GFORCE 1

#define gForceSerial Serial

#define thumb         0
#define indexfinger   1
#define middlefinger  2
#define ringfinger    3
#define littlefinger  4
#define armtwist      5

#define SERVOS_CNT    6

enum GestureType
{
  FistIdx = 0,
  ReleaseIdx,
  WaveInIdx,
  WaveOutIdx,
  TapIdx,
  ShootIdx,
  RelaxIdx,
  GestureCnt // Count
};

typedef struct
{
  int pos_min;
  int pos_max;
  int pos_offset;
} SERVO_POS_OFFSET;

const SERVO_POS_OFFSET SERVO_OFFSET[SERVOS_CNT] = {
    {5, 175, 0},
    {5, 175, 0},
    {5, 175, 0},
    {5, 175, 0},
    {5, 175, 0},
    {5, 175, 0}};

// Pos for thumb, finger, middle, ring, little, wrist
const int GESTURE[GestureCnt][SERVOS_CNT] = {
    {170, 170, 170, 170, 170,  90}, // fist
    { 10,  10,  10,  10,  10,  90}, // spread
    { 10,  10,  10,  10,  10, 170}, // wave in
    { 10,  10,  10,  10,  10,  10}, // wave out
    {170, 170,  10,  10,  10,  90}, // pinch
    { 10,  10, 170, 170, 170,  90}, // shoot
    { 40,  40,  40,  40,  40,  90}  // Relax
};

unsigned long SYS_GetTick(void);
int SYS_GetChar(unsigned char *data);

GForceAdapter gforce(SYS_GetChar, SYS_GetTick);

//unsigned long timeStamp;

Servo servos[SERVOS_CNT];

/* returns char count */
int SYS_GetChar(unsigned char *data)
{
  int ret = gForceSerial.read();

  if (ret == -1)
    return 0;

  *data = (unsigned char)ret;

  return 1;
}

/* returns System time */
unsigned long SYS_GetTick(void)
{
  return millis();
}

int servo_pos(GestureType gesture, int servo_id)
{
  int pos = GESTURE[gesture][servo_id];

  pos += SERVO_OFFSET[servo_id].pos_offset;
  pos = min(pos, SERVO_OFFSET[servo_id].pos_max);
  pos = max(pos, SERVO_OFFSET[servo_id].pos_min);

  return pos;
}

void perform_gesture(GestureType gestureIdx)
{
  static GestureType last_gesIdx = GestureCnt;

  Serial.println();
  Serial.print("Gesture ");
  Serial.print(gestureIdx);
  Serial.print(": ");

  if (last_gesIdx != gestureIdx)
  {
    for (int i = 0; i < SERVOS_CNT; i++)
    {
      int pos = servo_pos(gestureIdx, i);
      Serial.print(pos);
      Serial.print(",");

      servos[i].write(pos);
    }

    last_gesIdx = gestureIdx;
  }
}

void setup()
{
  servos[0].attach(6);
  servos[1].attach(5);
  servos[2].attach(4);
  servos[3].attach(3);
  servos[4].attach(2);
  servos[5].attach(7);

  //  Serial.begin(115200);
  gForceSerial.begin(115200);
  delay(500);
  //  timeStamp = millis();
}

void loop()
{
  //put your main code here, to run repeatedly:
  
  //  if((millis() - timeStamp) > 5000)
  //  {
  //    timeStamp = millis();
  //  }

#if USE_GFORCE
  GF_Data gForceData;

  if (OK == gforce.GetGForceData(&gForceData, 10))
  {
    GF_Gesture gesture;

    switch (gForceData.type)
    {
      case GF_Data::QUATERNION:
        break;

      case GF_Data::GESTURE:
        gesture = gForceData.value.gesture;

        if (gesture == GF_FIST)
        {
          perform_gesture(FistIdx);
        }
        else if (gesture == GF_SPREAD)
        {
          perform_gesture(ReleaseIdx);
        }
        else if (gesture == GF_WAVEIN)
        {
          perform_gesture(WaveInIdx);
        }
        else if (gesture == GF_WAVEOUT)
        {
          perform_gesture(WaveOutIdx);
        }
        else if (gesture == GF_PINCH)
        {
          perform_gesture(TapIdx);
        }
        else if (gesture == GF_SHOOT)
        {
          perform_gesture(ShootIdx);
        }
        else if (gesture == GF_RELEASE)
        {
          perform_gesture(RelaxIdx);
        }
        else if (gesture == GF_UNKNOWN)
        {
          perform_gesture(RelaxIdx);
        }

        break;

      default:
        break;
    }
  }
#else
  static int i = 0;

  perform_gesture(i);
  delay(2000);

  if (++i >= GestureCnt)
  {
    i = 0;
  }

#endif
}
