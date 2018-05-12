# American pedestrian signal controller on ESP8266

![Prototyping](images/Prototyping1.jpg)

## What's this?

* It's pedestrian signal controller running on ESP8266 (aka ESP-WROOM-02).
* I made custom schematic and artwork from scratch.
  * Use EAGLE 8
* I wrote sequencer code by C++ on ESP8266-Arduino IDE.

![Prototyping](images/Prototyping2.jpg)

## Features

* Basic sequence for american pedestrian signal standard (3 wired control).
* Scheduled time control between signal-on hour and signal-off hour.
* Battery-backuped RTC using scheduler.
* Auto synchronize NTP server with your WiFi AP.

## Schematic and artwork

* I ordered PCB to [Fusion PCB](https://www.seeedstudio.com/fusion_pcb.html).
  * TODO: Now in production ;)

![Schematic](images/Schematic.png)

![Artwork](images/Artwork.png)

## How to assemble PedestrianController

![Finished](images/Finished.jpg)

* See also: [How to assemble PedestrianController](HowToAssemble.md)

## License

* Under Apache v2
