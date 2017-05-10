# gForce SDK for Arduino

## Brief
The gForce SDK for Arduino is for using the gForce Embedded Suite in Arduino and
Arduino-compatible devices. It offers an easy way for Arduino-alike devices to
connect to the gForce armband and read gForce armband data such as gestures and
quaternions over UART.

## Usage
1. Import the gForce SDK for Arduino

    `zip` all files of this source code into a package file, say `gForce.zip`.
    Open the Arduino IDE, select `Sketch -> Include Library -> Add .ZIP library`,
    and then select `gForce.zip`.

2. Open and run the example code

    The example code maps six Arduino GPIO pins to six gestures. The GPIO pin
    will output high when the corresponding gesture from the gForce armband is
    received.

    Follow the following step to run the example code:

    * Wire the gForceJoint, which is included in the gForce Embedded Suite, to
      the Arduino board. The gForceJoint is BLE module used for connecting
      the gForce armband to Arduino. Turn on your Arduino board and connect it
      to your development PC.

    * On your development PC, select `File -> Example -> gForce -> SimpleControlByGesture`
      to open the the example code. Modify the code if needed, then build,
      download and run it.

    * Turn on the gForce armband, and appoach it to the gForceJoint to connect
      to the Arduino over BLE. Make sure the connection is done by checking the
      light on the gForce armband flashes fast. Put it on your right forearm.

    * Do the gestures and check if the corresponding Arduino pins output
      correctly.

    **Note:**
    > To make sure gForce armband can recognize your gestures, please refer to
    > [Guide to Making Gestures](http://www.tudou.com/programs/view/7ETsgGTRw2k/)
    > and spend several minutes learning and training yourself. The recognition
    > rate can reach 95% and higher after you get familiar with doing the
    > gestures.

## Trouble shooting
1. Some Arduino boards may use GPIO to stimulate UART, alleged software UART.
   This may consume most of the CPU cycles and cause problems with processing
   quaternion data. Quaternion data packages may be dropped in this situation.
   If you meet this problem,  you will have to use hardware UART instead.
