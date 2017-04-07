# gForce SDK for Arduino

## Brief
The gForce SDK for Arduino is for using the gForce Embedded Suite in Arduino and
Arduino-compatible devices. It offers an easy way of connecting to the gForce
armband and reading gForce armband data such as gestures and quaternions/Euler
angles.

## Usage
1. Wire the hardware

    The gForceJoint, which is included in the gForce Embedded Suite, is a BLE
    module that connects the gForce armband to the Arduino board. You need to
    wire the BLE to the UART port of the Arduino

2. Import the gForce SDK for Arduino

    Open the Arduino IDE, select `Sketch -> Include Library -> Add .ZIP library`,
    and then select the SDK zip file `gForce.zip`.

3. Open the example code

    Select `File -> Example -> gForce -> SimpleControlByGesture`.
