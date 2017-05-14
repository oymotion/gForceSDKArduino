# gForce SDK for Arduino

## Brief
The gForce SDK for Arduino is for using the gForce Embedded Suite in Arduino and
Arduino-compatible devices. It offers an easy way for Arduino-alike devices to
connect to gForce armband and read gForce armband data such as gestures and
quaternions.

A gForce Embedded Suite includes a gForce armband and a gForceJoint. 
The gForceJoint is a hardware module that bridges the gForce armband over BLE 
to the Arduino board over UART.

## Usage
1. Import the gForce SDK for Arduino

    `zip` all files of this source code into a package file, say `gForce.zip`.
    Open the Arduino IDE, select `Sketch -> Include Library -> Add .ZIP library`,
    and then select `gForce.zip`.

2. Open and run the example code

    The example code maps six Arduino GPIO pins to six gestures. A GPIO pin
    will output high when its corresponding gesture from gForce armband is
    received.

    The following steps will instruct you to run the example code:

    * Wire the gForceJoint to the Arduino board. And then turn on your Arduino 
      board and connect it to your development PC with Arduino IDE.

    * In the Arduino IDE, select `File -> Example -> gForce -> SimpleControlByGesture`
      to open the the example code. Modify the code if needed, then build,
      download and run it.

    * Turn on the gForce armband, and appoach it to the gForceJoint to connect
      to the Arduino board over BLE. Make sure the connection is established by
      checking if the LED on the gForce armband flashes fast. And then put
      it on your right forearm.

    * Do the gestures and check if the corresponding Arduino pins output
      correctly.

    **Note:**
    > To make sure gForce armband can recognize your gestures, please refer to
    > [Guide to Making Gestures](http://www.tudou.com/programs/view/7ETsgGTRw2k/)
    > and spend several minutes learning and training yourself. The recognition
    > rate can reach 95% and even higher after you get familiar with doing the
    > gestures.

## Trouble shooting
1. Some Arduino boards may use GPIO to stimulate UART, alleged software UART.
   This may consume most of the CPU cycles and cause problems with processing
   quaternion data. Quaternion data packages may be dropped in this situation.
   If you meet such a problem,  you may have to use a hardware UART instead.
