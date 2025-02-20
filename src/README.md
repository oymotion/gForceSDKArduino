# gForce SDK for Embedded

## Overview

gForce SDK for Embedded is for using [gForce Embedded Suite] [gForceEmbeddedSuite]  
in Embedded devices. It offers an easy way for AVR/ARM to connect to [gForce Armband][gForceArmband] and read data such as gestures, quaternions and emgrawdata from it.

A gForce Embedded Suite includes a gForce Armband and a gForceJoint. The gForceJoint is a hardware module that bridges the gForce armband over BLE to the AVR/ARM board over UART.

## Usage

We provide some C API interfaces in the library to help you analyze the data of gForce armband.
You must add those files into your project, and call the interfaces:

```C
///
/// Brief Sets up the serial line connection. This function shall be called
/// prior to GetForceData.
///
void GF_Init(FUNC_GET_SERIAL_DATA getCharFunc, FUNC_GET_MILLIS getTimerFunc);
```

```C
//Parse the data received by the serial port;  
//*gForceData : Points to the structure that stores the parsed data;  
//timeout : Set data acceptance judgment time;

GF_Ret GF_GetGForceData(GF_Data *gForceData, unsigned long timeout);
```

```C
//Converts the received quaternions into euler angles;  
//*quat :  Points to a structure that stores quaternions;  
//*euler : Points to the structure that stores the euler Angle;  

GF_Ret GF_QuaternionToEuler(const GF_Quaternion *quat, GF_Euler *euler);
```

```C
//Determine if the hand gesture is obtained;  
//gesture : The variable that stores the gesture;
//timeout : Set data acceptance judgment time;  

bool GF_GotGesture(GF_Gesture gesture, unsigned long timeout);
```

The following two functions require users to populate themselves based on the platform they use:

```C
// Gets the data received by the serial port
// *data : Point to the received data
// return : Returns 1 if data got, otherwise 0 
int SYS_GetChar(unsigned char *data)  
{  
  int ret = Your_USART_RxData;

  if(ret == -1)  
    return 0;  

  *data = (unsigned char)ret;  

  return 1;
}
```

```C
// returns System time in milliseconds
unsigned long SYS_GetTick(void)
{  
  return System_RunTime();  
}  
```

Then in main() or proper funtion：

```C
// Init gForce
GF_Init(SYS_GetChar, SYS_GetTick);

while (true)
{
  GF_Data gForceData;
  GF_Euler Euler;

  // timeout is 100ms
  unsigned long timeout = 100;

  // Get gForce data
  GF_Ret ret = GF_GetGForceData((&gForceData), timeout);

  if (GF_RET_OK == ret)
  {
    GF_Gesture gesture;
    switch (gForceData.type)
    {
    case GF_QUATERNION:
      GF_QuaternionToEuler(&(gForceData.value.quaternion), &Euler);
      // Do something?

      break;
    case GF_GESTURE:
      gesture = gForceData.value.gesture;
      // Do something?
      // printf("gesture: %d\n", gesture);

      break;
    default:
      break;
    }
  }
  else
  {
    // Error occured
    // printf("GF_GetGForceData() returned: %d\n", ret);
  }
}
```

**Note:**
    > Use equivalent functions of class gForceAdaptor for C++, or use C API in C++.
    > To make sure gForce armband can recognize your gestures, please refer to
    > [Guide to Performing Gestures][GuideToPerformingGestures]
    > and spend several minutes learning and training yourself. The recognition
    > rate can reach 95% and even higher after you get familiar with performing
    > the gestures properly.

## Trouble shooting

1. Some Arduino boards may use GPIO to simulate UART, alleged software UART. This may consume most of the CPU cycles and cause problems with processing quaternion data. Quaternion data packages may be dropped in this situation. If you meet such a problem,  you may have to use a hardware UART instead.  

[gForceEmbeddedSuite]: https://oymotion.github.io/doc/gForce100EmbeddedSuiteUserGuide/
[gForceArmband]: https://oymotion.github.io/doc/gForce100UserGuide/
[GuideToPerformingGestures]: https://www.youtube.com/watch?v=wBsYJf0wrkk  
