# NXP Audio 2.0 Driver to I2S Example

![alt text](images/LPCXpresso43S67_setup.jpg?raw=true "Setup")

This repository contains a UAC Audio 2.0 driver for the NXP LCP43S00 series of processors.  As shown, this driver is working as a USB Speaker.  

This repository started out with the NXP Audio driver that only supported Audio 1.0 standard.  This driver has been updated to support the Audio 2.0 standard.  What this means is that instead of data being transferred in USB fast mode, the data will be transferred in USB high speed mode meaning a greater data bandwidth is possible.  This is just a proto-type in that all features of the Audio 2.0 standard may not be supported.  I've only implemented and have tested what was needed to support the USB device as speakers.

## Required Hardware

As shown in the photo above, I'm using the NXP LPCXpresso43S67 V3 Development board to act as the USB Client.  I'm also using two SparkFun I2S Audio Breakout Boards.  The SparkFun I2S Audio Breakout board uses the MAX98357A digital to analog converter (DAC), which converts I2S (not be confused with I2C) audio to an analog signal to drive the speakers.  The board can be configured to output only the left channel audio, right channel, or both.  In my setup, one board has been configured as the right channel and one board has been configured as the left channel.  I'm also using two thin speakers, one for each audio board, which are available from SparkFun as well.

## Required Software

You will need to download and install the latest version of MCUXpresso IDE from NXP.  I'm currently using V11.2.1 for my testing. 

## Test Setup

The following table lists the wiring required for this setup.   For reference, I'v included the color of the hookup wires that I used in the photo above to connect the LPCXpresso43S67 to the Audio Breakout board.  Please be aware that the two SparkFun I2S Audio Brakout boards are wired in parallel. 

LPCXpresso43S67 | Audio Breakout | Hookup Wire Color
--------------- | -------------- | -----------------
J3-4 | BCLK | Green
J3-5 | GND | Black
J3-7 | SD_ | White
J3-8 | DIN | Blue
J3-12 | 2.2V/5.5V | Red
J1-12 | LRCLK | Yellow

## Compiling and Running the Example Program

Once the IDE is up and running, you will need to import the three directories in the repository.  Then it's just a simple matter of compiling the code and downloading the image to your device.  


## Testing

Once you have the device up and running, it's time to verify that it's actually enumerating as an UAC Audio 2.0 device.  One simple way to make sure that the device is enumerating is through Device Manage.  Here is what it looks like on my system:

![alt text](images/DeviceManager.jpg?raw=true "Device Manager")

I believe that the USB Audio 2.0 value is coming from the enumeration data that we supplied to the host computer, so this could be a little misleading.  I'll show you how to verify the driver in a minute or two.

Next let's verify the enumeration data that we supplied to the host during enumeration.  One simple tool that you can use that comes with Widows 10 is USB Device Viewer.  This is a very nice tool in that it will display the enumeration data for all USB devices attached to you system.  Here is what USB Device Viewer shows for our example board:

```
[Port2]  :  USB Composite Device


Is Port User Connectable:         no
Is Port Debug Capable:            no
Companion Port Number:            0
Companion Hub Symbolic Link Name: 
Protocols Supported:
 USB 1.1:                         yes
 USB 2.0:                         yes
 USB 3.0:                         no

Device Power State:               PowerDeviceD0

       ---===>Device Information<===---
English product name: "Audio Output Demo"

ConnectionStatus:                  
Current Config Value:              0x01  -> Device Bus Speed: High (is not SuperSpeed or higher capable)
Device Address:                    0x1C
Open Pipes:                           2

          ===>Device Descriptor<===
bLength:                           0x12
bDescriptorType:                   0x01
bcdUSB:                          0x0200
bDeviceClass:                      0xEF  -> This is a Multi-interface Function Code Device
bDeviceSubClass:                   0x02  -> This is the Common Class Sub Class
bDeviceProtocol:                   0x01  -> This is the Interface Association Descriptor protocol
bMaxPacketSize0:                   0x40 = (64) Bytes
idVendor:                        0x1FC9 = NXP Semiconductors
idProduct:                       0x2046
bcdDevice:                       0x0001
iManufacturer:                     0x01
     English (United States)  "Acme Co."
iProduct:                          0x02
     English (United States)  "Audio Output Demo"
iSerialNumber:                     0x03
     English (United States)  "000001"
bNumConfigurations:                0x01

          ---===>Open Pipes<===---

          ===>Endpoint Descriptor<===
bLength:                           0x07
bDescriptorType:                   0x05
bEndpointAddress:                  0x01  -> Direction: OUT - EndpointID: 1
bmAttributes:                      0x0D  -> Isochronous Transfer Type, Synchronization Type = Synchronous, Usage Type = Data Endpoint
wMaxPacketSize:                  0x0200 = 1 transactions per microframe, 0x200 max bytes
bInterval:                         0x02

          ===>Endpoint Descriptor<===
bLength:                           0x07
bDescriptorType:                   0x05
bEndpointAddress:                  0x81  -> Direction: IN - EndpointID: 1
bmAttributes:                      0x11  -> Isochronous Transfer Type, Synchronization Type = No Synchronization, Usage Type = Feedback Endpoint
wMaxPacketSize:                  0x0004 = 1 transactions per microframe, 0x04 max bytes
bInterval:                         0x02

       ---===>Full Configuration Descriptor<===---

          ===>Configuration Descriptor<===
bLength:                           0x09
bDescriptorType:                   0x02
wTotalLength:                    0x0094  -> Validated
bNumInterfaces:                    0x02
bConfigurationValue:               0x01
iConfiguration:                    0x00
bmAttributes:                      0xC0  -> Self Powered
MaxPower:                          0x32 = 100 mA

          ===>IAD Descriptor<===
bLength:                           0x08
bDescriptorType:                   0x0B
bFirstInterface:                   0x00
bInterfaceCount:                   0x02
bFunctionClass:                    0x01  -> Audio Interface Class
bFunctionSubClass:                 0x00
*!*CAUTION:    This appears to be an invalid bFunctionSubClass
bFunctionProtocol:                 0x20
iFunction:                         0x00

          ===>Interface Descriptor<===
bLength:                           0x09
bDescriptorType:                   0x04
bInterfaceNumber:                  0x00
bAlternateSetting:                 0x00
bNumEndpoints:                     0x00
bInterfaceClass:                   0x01  -> Audio Interface Class
bInterfaceSubClass:                0x01  -> Audio Control Interface SubClass
bInterfaceProtocol:                0x20
*!*WARNING:  must be set to PC_PROTOCOL_UNDEFINED 0 for this class
iInterface:                        0x00

          ===>Audio Control Interface Header Descriptor<===
bLength:                           0x09
bDescriptorType:                   0x24 (CS_INTERFACE)
bDescriptorSubtype:                0x01 (HEADER)
bcdADC:                          0x0200
wTotalLength:                    0x3C01
bInCollection:                     0x00

          ===>Descriptor Hex Dump<===
bLength:                           0x08
bDescriptorType:                   0x24
08 24 0A 10 01 07 00 00 

          ===>Descriptor Hex Dump<===
bLength:                           0x11
bDescriptorType:                   0x24
11 24 02 20 01 01 00 10 02 03 00 00 00 00 00 00 
00 

          ===>Audio Control Feature Unit Descriptor<===
bLength:                           0x0E
bDescriptorType:                   0x24 (CS_INTERFACE)
bDescriptorSubtype:                0x06 (FEATURE_UNIT)
bUnitID:                           0x30
bSourceID:                         0x20
bControlSize:                      0x00
Audio controls are not available (bControlSize = 0)

          ===>Descriptor Hex Dump<===
bLength:                           0x0C
bDescriptorType:                   0x24
0C 24 03 40 01 03 00 30 10 00 00 00 

          ===>Interface Descriptor<===
bLength:                           0x09
bDescriptorType:                   0x04
bInterfaceNumber:                  0x01
bAlternateSetting:                 0x00
bNumEndpoints:                     0x00
bInterfaceClass:                   0x01  -> Audio Interface Class
bInterfaceSubClass:                0x02  -> Audio Streaming Interface SubClass
bInterfaceProtocol:                0x20
*!*WARNING:  must be set to PC_PROTOCOL_UNDEFINED 0 for this class
iInterface:                        0x00

          ===>Interface Descriptor<===
bLength:                           0x09
bDescriptorType:                   0x04
bInterfaceNumber:                  0x01
bAlternateSetting:                 0x01
bNumEndpoints:                     0x02
bInterfaceClass:                   0x01  -> Audio Interface Class
bInterfaceSubClass:                0x02  -> Audio Streaming Interface SubClass
bInterfaceProtocol:                0x20
*!*WARNING:  must be set to PC_PROTOCOL_UNDEFINED 0 for this class
iInterface:                        0x00

          ===>Descriptor Hex Dump<===
bLength:                           0x10
bDescriptorType:                   0x24
10 24 01 20 00 01 01 00 00 00 02 03 00 00 00 00 

          ===>Audio Streaming Format Type Descriptor<===
bLength:                           0x06
bDescriptorType:                   0x24 (CS_INTERFACE)
bDescriptorSubtype:                0x02 (FORMAT_TYPE)
bFormatType:                       0x01 (FORMAT_TYPE_I)
bNrChannels:                       0x02
bSubframeSize:                     0x10
bBitResolution:                    0x07 (7)
bSamFreqType:                      0x05 (Discrete)
tSamFreq[1]:                   0x000D01 (3329 Hz)
tSamFreq[2]:                   0x080202 (524802 Hz)
tSamFreq[3]:                   0x000125 (293 Hz)
tSamFreq[4]:                   0x000000 (0 Hz)
tSamFreq[5]:                   0x050700 (329472 Hz)

          ===>Endpoint Descriptor<===
bLength:                           0x07
bDescriptorType:                   0x05
bEndpointAddress:                  0x01  -> Direction: OUT - EndpointID: 1
bmAttributes:                      0x0D  -> Isochronous Transfer Type, Synchronization Type = Synchronous, Usage Type = Data Endpoint
wMaxPacketSize:                  0x0200 = 1 transactions per microframe, 0x200 max bytes
bInterval:                         0x02

          ===>Descriptor Hex Dump<===
bLength:                           0x08
bDescriptorType:                   0x25
08 25 01 00 00 00 00 00 

          ===>Endpoint Descriptor<===
bLength:                           0x07
bDescriptorType:                   0x05
bEndpointAddress:                  0x81  -> Direction: IN - EndpointID: 1
bmAttributes:                      0x11  -> Isochronous Transfer Type, Synchronization Type = No Synchronization, Usage Type = Feedback Endpoint
wMaxPacketSize:                  0x0004 = 1 transactions per microframe, 0x04 max bytes
bInterval:                         0x02
```

There's a couple of minor issues that I'll need to clean up when I get a chance, but on the most part, everything is as we would expect.  There are two main points that I want to point out.  First, as you can see in the Connection Status, our device is using high speed mode with two open pipes.  Here is the ConnectionStatus section:

```
ConnectionStatus:                  
Current Config Value:              0x01  -> Device Bus Speed: High (is not SuperSpeed or higher capable)
Device Address:                    0x1C
Open Pipes:                           2
```

The other part that I would like to point out has to do with the two Endpoint Descriptors that are shown at the end of the file listing.  Both are using Isochronous Mode!  You have to have worked with USB for a while to understand why this is an achievement.  

Now to verify which driver is actually being used by our device.  There may be many different ways to find this, but I only know of one.  And that one is by running the program usbdeview by NirSoft.  This program has a very wide view, so I need to cut it in half to show what I want to show:

![alt text](images/USBDeview_left_half.jpg?raw=true "USBDeview")

![alt text](images/USBDeview_right_half.jpg?raw=true "USBDeview")

It's the last line that I'd like to highlight.  The first image shows our device as USB Audio 2.0, but most importantly, pay attention to the VendorID and ProductID.  These match what we are supplying to the host as also shown in the USB Device Viewer.  

But it's the second image that shows what we are looking for an that is that our device is using the usbaudio2.inf along with usbaudio2 driver, which is the Windows Audio 2.0 driver.

To further verify that this is truly supporting UAC Audio 2.0 will require a Beagle USB protocol analyzer, but I don't have an extra $5K nor the need to purchase one right now.  But having used one in the past, the Beagle 5000 is an excellent tool, if you can afford one.

## Conclusion

So that's about it for now.  If I have some extra time, I may work out the issues with the enumeration.  But I doubt that I'll ever have a need to update the driver to support more than just the speakers.  So at this point, this is more of proto-type for proof of concept that the NXP LPC MCU can support the UAC Audio 2.0 standard.

Until next time, have some fun!