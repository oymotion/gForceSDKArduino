# gForce SDK for Arduino

## Overview
gForce SDK for Arduino is for using [gForce Embedded Suite][gForceEmbeddedSuite] 
in Arduino and Arduino-compatible devices. It offers an easy way for 
Arduino-alike devices to connect to [gForce Armband][gForceArmband] and read 
data such as gestures and quaternions from it.

A gForce Embedded Suite includes a gForce Armband and a gForceJoint. 
The gForceJoint is a hardware module that bridges the gForce armband over BLE 
to the Arduino board over UART.

## Usage  
1. Download method  
    If you have git:  

    First you need to download the files from the library:  

    `$ git clone git@github.com:oymotion/gForceSDKArduino.git`  

    Then download all the submodule files:  

    `$ git submodule update --init`  

    Complete these two steps to obtain the complete project documentation,The submodule should be located 
    `gForceSDKArduino -> src ` ,including ` gForceSDKEmbedded.git ` all files,if no,
    Please download and add ` gForceSDKEmbedded.git ` to the above location  by yourself.  

    If you don't have git :  

    First enter the oymotion/gForceSDKArduino.git , Click the ` Clone or download ` to download the  gForceSDKArduino.zip;
    Then enter the oymotion/gForceSDKEmbedded.git , Click the ` Clone or download ` to download the  gForceSDKEmbedded.zip;
    Depress gForceSDKEmbedded.zip and add all the files decompressed into `gForceSDKArduino -> src `.  

2. Importing the gForce SDK for Arduino

    `zip` all files of this source code into a package file, say `gForceSDKArduino.zip`.
    Open the Arduino IDE, select `Sketch -> Include Library -> Add .ZIP library`,
    and then select `gForceSDKArduino.zip`.

3. Opening and running the example code

    The example code maps six Arduino GPIO pins to six gestures. A GPIO pin
    will output high when its corresponding gesture from gForce armband is
    received.

    The following steps will instruct you to running the example code:

    * Wire the gForceJoint to the Arduino board, and then turn on the Arduino 
      board after connecting it to your development PC with Arduino IDE.

    * In the Arduino IDE, select `File -> Example -> gForce -> SimpleControlByGestureC`
      to open the the example code. Modify the code if needed, then build,
      download and run it.

    * Turn on the gForce Armband, and appoach it to the gForceJoint as close as 
      possible to make them connect with each other via BLE. Make sure the 
      connection is established by checking if the green light LED on the 
      gForce Armband flashes fast. And then wear the gForce Armband on your 
      right forearm.

    * [Perform gestures][GuideToPerformingGestures] and check if the corresponding 
      Arduino pins output correctly.

    **Note:**
    > To make sure gForce armband can recognize your gestures, please refer to
    > [Guide to Performing Gestures][GuideToPerformingGestures]
    > and spend several minutes learning and training yourself. The recognition
    > rate can reach 95% and even higher after you get familiar with performing 
    > the gestures properly.

## Trouble shooting
1. Some Arduino boards may use GPIO to simulate UART, alleged software UART.
   This may consume most of the CPU cycles and cause problems with processing
   quaternion data. Quaternion data packages may be dropped in this situation.
   If you meet such a problem,  you may have to use a hardware UART instead.

[gForceEmbeddedSuite]: https://oymotion.github.io/doc/gForce100EmbeddedSuiteUserGuide/
[gForceArmband]: https://oymotion.github.io/doc/gForce100UserGuide/
[GuideToPerformingGestures]: https://www.youtube.com/watch?v=wBsYJf0wrkk  
