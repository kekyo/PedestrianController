/////////////////////////////////////////////////////////////////////////////////////////////////
//
// PedestrianController - American pedestrian signal controller on ESP8266
// Copyright (c) 2017-2018 Kouji Matsui (@kozy_kekyo)
//
// PedestrianSignal - Matrix signal controller demonstration at NT NAGOYA 2018.
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
#include <Ticker.h>

#include <functional>

#include "Config.h"

////////////////////////////////////////////////

class PedestrianSignalController
{
private:
    ESP8266WebServer* pServer;
    HardwareSerial* pSerial;

    Ticker ticker;

    volatile bool tickStatus;
    volatile uint8_t blinkingRemains;

    enum States
    {
        Stopped,
        Walking,
        BlinkOn,
        BlinkOff
    } volatile currentState;

    enum RequestStates
    {
        None,
        Stop,
        Walk
    } volatile requestState;

    void requestStatus()
    {
        String result;
        switch (currentState)
        {
            case States::Stopped:
                result = "Stopped";
                break;
            case States::Walking:
                result = "Walking";
                break;
            case States::BlinkOn:
            case States::BlinkOff:
                result = "Blinking ";
                result += blinkingRemains;
                break;
        }

        pServer->send(200, "text/plain", result.c_str());
    }

    void requestWalk()
    {
        requestState = RequestStates::Walk;
        pServer->send(200, "text/plain", "Walk requested.");
        pSerial->println("Walk requested.");
    }

    void requestStop()
    {
        requestState = RequestStates::Stop;
        pServer->send(200, "text/plain", "Stop requested.");
        pSerial->println("Stop requested.");
    }

    void requestNotFound()
    {
        pServer->send(404, "text/plain", "Invalid resource path.");
    }

    void tick()
    {
        switch (currentState)
        {
            case States::Stopped:
                switch (requestState)
                {
                    case RequestStates::Stop:
                        requestState = None;
                        break;
                    case RequestStates::Walk:
                        digitalWrite(STOP, LOW);
                        digitalWrite(WALK, HIGH);
                        currentState = States::Walking;
                        requestState = None;
                        break;
                    default:
                        break;
                }
                break;
            case States::Walking:
                switch (requestState)
                {
                    case RequestStates::Stop:
                        digitalWrite(WALK, LOW);
                        digitalWrite(STOP, HIGH);
                        blinkingRemains = TRANSITION_COUNT;
                        currentState = States::BlinkOff;
                        requestState = None;
                       break;
                    case RequestStates::Walk:
                        requestState = None;
                        break;
                    default:
                        break;
                }
                break;
            case States::BlinkOff:
                digitalWrite(STOP, LOW);
                currentState = States::BlinkOn;
                break;
            case States::BlinkOn:
                digitalWrite(STOP, HIGH);
                blinkingRemains--;
                if (blinkingRemains == 0)
                {
                    currentState = States::Stopped;
                }
                else
                {
                    currentState = States::BlinkOff;
                }
                break;
        }

        digitalWrite(STATUS, tickStatus ? HIGH : LOW);
        tickStatus = !tickStatus;
    }

    static void tickHandler(PedestrianSignalController* pThis)
    {
        pThis->tick();
    }

public:
    PedestrianSignalController()
        : pServer(nullptr), pSerial(nullptr), tickStatus(false)
        , currentState(States::Stopped), blinkingRemains(0), requestState(RequestStates::None)
    {
    }

    void Init(ESP8266WebServer* pServer, HardwareSerial* pSerial)
    {
        this->pServer = pServer;
        this->pSerial = pSerial;

        pinMode(WALK, OUTPUT);
        pinMode(STOP, OUTPUT);
        pinMode(STATUS, OUTPUT);

        delay(100);

        digitalWrite(WALK, LOW);
        digitalWrite(STOP, HIGH);
        digitalWrite(STATUS, LOW);

        pServer->on("/api/status", HTTP_GET, [&]() { requestStatus(); });
        pServer->on("/api/walk", HTTP_GET, [&]() { requestWalk(); });
        pServer->on("/api/stop", HTTP_GET, [&]() { requestStop(); });
        pServer->onNotFound([&]() { requestNotFound(); });

        ticker.attach_ms(TRANSITION_TIME, tickHandler, this);
    }
};

////////////////////////////////////////////////

ESP8266WebServer server(80);

PedestrianSignalController controller;

void setup(void)
{
    Serial.begin(115200);
    Wire.begin();

    delay(500);

    Serial.println("    ");
    Serial.println("PedestrianSignal start.");

    const auto localIP = IPAddress(192, 168, 4, 3);
    const auto gatewayIP = IPAddress(192, 168, 4, 1);
    const auto netmask = IPAddress(255, 255, 255, 0);

    WiFi.mode(WIFI_STA);
    WiFi.config(localIP, gatewayIP, netmask);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting WiFi [");
    Serial.print(WIFI_SSID);
    Serial.print("] ");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected [");
    Serial.print(WiFi.localIP());
    Serial.println("]");

    controller.Init(&server, &Serial);
    server.begin();

    Serial.println("HTTP server started.");
}

void loop(void)
{
    server.handleClient();
}
