/////////////////////////////////////////////////////////////////////////////////////////////////
//
// PedestrianController - American pedestrian signal controller on ESP8266
// Copyright (c) 2017-2018 Kouji Matsui (@kozy_kekyo)
//
// PedestrianSignalButton - Matrix signal controller demonstration at NT NAGOYA 2018.
// This is part of PedestrianController.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PEDESTRIAN_CONTROLLER_CONFIG_H
#define PEDESTRIAN_CONTROLLER_CONFIG_H

#define REQUEST 4        // ESP8266 (ESP-WROOM-02) IO4
#define DFPLAYER_RX 12   // ESP8266 (ESP-WROOM-02) IO12
#define DFPLAYER_TX 13   // ESP8266 (ESP-WROOM-02) IO13
#define PUMPED 14        // ESP8266 (ESP-WROOM-02) IO14

#define TRANSITION_WAITING2 10   // second
#define TRANSITION_WALKING 20    // second
#define TRANSITION_WAITING0 3    // second
#define TRANSITION_DEMO 30       // second

// your network SSID (name)
#define WIFI_SSID "MatrixSignalDemo"
// your network password
#define WIFI_PASSWORD "li2u3yr9fbi2uh4"

#define WIFI_ROAD_SIGNAL_NAME "192.168.4.2"
#define WIFI_PEDESTRIAN_SIGNAL_NAME "192.168.4.3"

#define CHIRP_SOUND 1
#define CUCKOO_SOUND 2
#define WAIT_SOUND 3
#define WALK_SOUND 4

#endif
