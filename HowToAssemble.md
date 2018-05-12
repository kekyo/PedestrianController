# How to assemble PedestrianController.

* This is point for assemble parts from naked plain board.

## Assemble ESP-WROOM-02

![PlainBoard](images/PlainBoard.jpg)

![SolderStep0](images/SolderStep0.jpg)

* Light stick ESP-WROOM-02 to board.

![FixShifted](images/FixShifted.png)

* We have to fix shifted position for ESP-WROOM-02 and do solder.

![SolderStep1](images/SolderStep1.jpg)

* Mount switches.

![SolderStep2](images/SolderStep2.jpg)

![SolderStep3](images/SolderStep3.jpg)

* Finished partial mounting because we have to test before assemble all parts.

## Write PedestrianController firmware.

![SetupSerial](images/SetupSerial.jpg)

* Connect header "CON1" to USB-serial converter.
  * Caution: We must not mistake power line between 3.3V and GND.
  * I use this converter: [High power USB-Serial converter (In japanese)](https://www.switch-science.com/catalog/3422/)

* Before write, we do power up (Connect serial converter to USB) and reset for firmware-write mode by mounted switches.
  * Swithces-on-off sequence is bit complex.
  * See also the video: https://youtu.be/IjTIz9GXHyo 

![SetupArduino](images/SetupArduino.png)

* Select "Generic ESP8266 Module" at Arduino IDE.
  * And select these menu items (include your USB port "COM?")

![Writing](images/Writing.png)

* This is writing log.
  * After wrote firmware, We can see LED's grow sequence. See also the video: https://youtu.be/FbDNfWrt7zE

![DebugLog1](images/DebugLog1.png)

* And outout debug log. It contains initial wifi sequence.
  * Log contains invalid "Current time from RTC", you can ignore this logs.

## Extract power module from AC power supply unit.

![OpenPowerSupply](images/OpenPowerSupply.jpg)

![ExtractPowerSupply](images/ExtractPowerSupply.jpg)

## Final assemble steps.

* We have to assemble all parts.
  * Include power terminals (CON3, CON4), SSRs and RTC module header (CON2).

![Intermediate1](images/Intermediate1.jpg)

![Intermediate2](images/Intermediate2.jpg)

* If RTC module's header pin angled, force straight and insert to header.

![ConnectPowerSupply](images/ConnectPowerSupply.jpg)

* Connect power module onto board.

![RigidBase1](images/RigidBase1.jpg)

* Cut longer rigid bases.

![RigidBase2](images/RigidBase2.jpg)

* Pre-mount to board.

![Finished](images/Finished.jpg)

* All items into the box "TAKACHI SW-100."
