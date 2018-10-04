/////////////////////////////////////////////////////////////////////////////////////////////////
//
// PedestrianController - American pedestrian signal controller on ESP8266
// Copyright (c) 2017-2018 Kouji Matsui (@kozy_kekyo)
//
// RoadSignal - Matrix signal controller demonstration at NT NAGOYA 2018.
// This is part of PedestrianController.
// It's from the ExtremeFeedbackDevice project: https://github.com/kekyo/ExtremeFeedbackDevice
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

#ifndef ROAD_CONTROLLER_CONFIG_H
#define ROAD_CONTROLLER_CONFIG_H

#define GO 12         // ESP8266 (ESP-WROOM-02) IO12
#define WILLSTOP 13   // ESP8266 (ESP-WROOM-02) IO13
#define STOP 14       // ESP8266 (ESP-WROOM-02) IO14
#define STATUS 4      // ESP8266 (ESP-WROOM-02) IO4

#define TRANSITION_WILLSTOP 4      // second

// your network SSID (name)
#define WIFI_SSID "MatrixSignalDemo"
// your network password
#define WIFI_PASSWORD "li2u3yr9fbi2uh4"

#endif
