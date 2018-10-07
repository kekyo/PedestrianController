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

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>    // https://github.com/DFRobot/DFRobotDFPlayerMini
#include <EasyButton.h>             // https://github.com/evert-arias/EasyButton

#include <functional>

#include "Config.h"

////////////////////////////////////////////////

class PedestrianSignalButton
{
private:
    DFRobotDFPlayerMini* pPlayer;
    EasyButton requestButton;

    uint32_t lastTickCount;
    uint32_t delayCount;
    uint32_t cuckooCount;
    uint32_t demoCount;

    enum States
    {
        Waiting1,
        Waiting2,
        WillWalk,
        Walking1,
        Walking2,
        WillWait,
        Waiting0
    } currentState;

    enum RoadSignalStates
    {
        Unknown_Road,
        Stopped_Road,
        Going,
        WillStop
    };

    enum PedestrianSignalStates
    {
        Unknown_Pedestrian,
        Stopped_Pedestrian,
        Walking_Pedestrian,
        Blinking
    };

    bool sendTo(const char* pHost, int port, bool isPost, const char* pResourcePath, String& result)
    {
        String url("http://");
        url += pHost;
        url += ":";
        url += port;
        url += pResourcePath;

        HTTPClient client;
        if (!client.begin(url))
        {
            Serial.println(" failed.");
            return false;
        }

        const auto statusCode = isPost ? client.POST("") : client.GET();
        result = client.getString();

        if (statusCode >= 400)
        {
            Serial.print(" failed:");
        }
        else
        {
            Serial.print(" success:");
        }

        Serial.print(statusCode);
        Serial.print(" [");
        Serial.print(result.c_str());
        Serial.println("].");

        return statusCode < 400;
    }

    bool sendStopToRoadSignal()
    {
        Serial.print("Send 'Stop' to RoadSignal ...");

        String result;
        return sendTo(WIFI_ROAD_SIGNAL_NAME, 80, false, "/api/stop", result);
    }

    bool sendGoToRoadSignal()
    {
        Serial.print("Send 'Go' to RoadSignal ...");

        String result;
        return sendTo(WIFI_ROAD_SIGNAL_NAME, 80, false, "/api/go", result);
    }

    RoadSignalStates getRoadSignal()
    {
        Serial.print("Getting RoadSignal status ...");

        String result;
        if (!sendTo(WIFI_ROAD_SIGNAL_NAME, 80, false, "/api/status", result))
        {
            return RoadSignalStates::Unknown_Road;
        }

        if (result == "Stopped")
        {
            return RoadSignalStates::Stopped_Road;
        }
        if (result == "Going")
        {
            return RoadSignalStates::Going;
        }
        if (result.startsWith("WillStop "))
        {
            return RoadSignalStates::WillStop;
        }

        return RoadSignalStates::Unknown_Road;
    }

    bool sendWalkToPedestrianSignal()
    {
        Serial.print("Send 'Walk' to PedestrianSignal ...");

        String result;
        return sendTo(WIFI_PEDESTRIAN_SIGNAL_NAME, 80, false, "/api/walk", result);
    }

    bool sendStopToPedestrianSignal()
    {
        Serial.print("Send 'Stop' to PedestrianSignal ...");

        String result;
        return sendTo(WIFI_PEDESTRIAN_SIGNAL_NAME, 80, false, "/api/stop", result);
    }

    PedestrianSignalStates getPedestrianSignal()
    {
        Serial.print("Getting PedestrianSignal status ...");

        String result;
        if (!sendTo(WIFI_PEDESTRIAN_SIGNAL_NAME, 80, false, "/api/status", result))
        {
            return PedestrianSignalStates::Unknown_Pedestrian;
        }

        if (result == "Stopped")
        {
            return PedestrianSignalStates::Stopped_Pedestrian;
        }
        if (result == "Going")
        {
            return PedestrianSignalStates::Walking_Pedestrian;
        }
        if (result.startsWith("Blinking "))
        {
            return PedestrianSignalStates::Blinking;
        }

        return PedestrianSignalStates::Unknown_Pedestrian;
    }

    void requested()
    {
        switch (currentState)
        {
            case States::Waiting1:
                digitalWrite(PUMPED, HIGH);
                delayCount = millis();
                currentState = States::Waiting2;
                pPlayer->play(WAIT_SOUND);
                delay(300);
                break;

            // Fall through
            case States::Waiting2:
                pPlayer->play(WAIT_SOUND);
                delay(300);
                break;
        }
    }

    void tick()
    {
        switch (currentState)
        {
            case States::Waiting1:
                demoCount--;
                if (demoCount == 0)
                {
                    digitalWrite(PUMPED, HIGH);
                    delayCount = millis();
                    currentState = States::Waiting2;
                    pPlayer->play(WAIT_SOUND);
                    delay(300);
                }
                break;

            case States::Waiting2:
                {
                    const auto now = millis();
                    if ((now - delayCount) >= (TRANSITION_WAITING2 * 1000))
                    {
                        sendStopToRoadSignal();
                        currentState = States::WillWalk;
                    }
                }
                break;

            case States::WillWalk:
                if (getRoadSignal() == RoadSignalStates::Stopped_Road)
                {
                    digitalWrite(PUMPED, LOW);
                    sendWalkToPedestrianSignal();
                    delayCount = millis();
                    currentState = States::Walking1;
                }
                break;

            case States::Walking1:
                {
                    const auto now = millis();
                    if ((now - delayCount) >= (TRANSITION_WALKING * 1000))
                    {
                        sendStopToPedestrianSignal();
                        currentState = States::WillWait;
                    }
                    else
                    {
                        cuckooCount = millis();
                        currentState = States::Walking2;
                    }
                }
                break;

            case States::Walking2:
                {
                    const auto now = millis();
                    if ((now - cuckooCount) >= 1000)
                    {
                        pPlayer->play(CUCKOO_SOUND);
                        currentState = States::Walking1;
                    }
                }
                break;

            case States::WillWait:
                if (getPedestrianSignal() == PedestrianSignalStates::Stopped_Pedestrian)
                {
                    delayCount = millis();
                    currentState = States::Waiting0;
                }
                break;

            case States::Waiting0:
                {
                    const auto now = millis();
                    if ((now - delayCount) >= (TRANSITION_WAITING0 * 1000))
                    {
                        sendGoToRoadSignal();
                        demoCount = TRANSITION_DEMO;
                        currentState = States::Waiting1;
                    }
                }
                break;
        }
    }

public:
    PedestrianSignalButton()
        : pPlayer(nullptr), currentState(States::Waiting1), requestButton(REQUEST)
        , lastTickCount(0), delayCount(0), cuckooCount(0), demoCount(TRANSITION_DEMO)
    {
    }

    void Init(DFRobotDFPlayerMini* pPlayer)
    {
        this->pPlayer = pPlayer;

        while (!sendGoToRoadSignal());
        while (!sendStopToPedestrianSignal());

        requestButton.onPressed([&]() { requested(); });
        requestButton.begin();
    }

    void handle()
    {
        requestButton.read();

        const auto now = millis();
        if ((now - lastTickCount) >= 1000)
        {
            lastTickCount = now;
            tick();
        }
    }
};

////////////////////////////////////////////////

SoftwareSerial softwareSerial(DFPLAYER_RX, DFPLAYER_TX);
DFRobotDFPlayerMini player;

PedestrianSignalButton button;

void setup(void)
{
    Serial.begin(115200);
    softwareSerial.begin(9600);
    Wire.begin();

    pinMode(PUMPED, OUTPUT);

    delay(100);

    digitalWrite(PUMPED, LOW);

    delay(500);

    Serial.println("    ");
    Serial.println("PedestrianSignalButton start.");

    Serial.print("Initializing DFPlayer ");
    while (true)
    {
        Serial.print(".");
        if (!player.begin(softwareSerial))
        {
            delay(500);
        }
        else
        {
            player.volume(30);
            break;
        }
    }
    Serial.println(" done.");

    const auto localIP = IPAddress(192, 168, 4, 1);
    const auto gateway = IPAddress(192, 168, 4, 1);
    const auto netmask = IPAddress(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(localIP, gateway, netmask);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Served WiFi [");
    Serial.print(WIFI_SSID);
    Serial.print("] ");

    Serial.println("");
    Serial.print("Connected [");
    Serial.print(WiFi.softAPIP());
    Serial.println("]");

    button.Init(&player);
}

void loop(void)
{
    button.handle();
}
