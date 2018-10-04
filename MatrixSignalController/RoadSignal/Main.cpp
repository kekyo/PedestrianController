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

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>

#include <functional>

#include "Config.h"

////////////////////////////////////////////////

class RoadSignalController
{
private:
    ESP8266WebServer* pServer;
    HardwareSerial* pSerial;

    Ticker ticker;

    volatile bool tickStatus;
    volatile uint8_t stopRemains;

    enum States
    {
        Stopped,
        Going,
        WillStop
    } volatile currentState;

    enum RequestStates
    {
        None,
        Stop,
        Go
    } volatile requestState;

    void requestStatus()
    {
        String result;
        switch (currentState)
        {
            case States::Stopped:
                result = "Stopped";
                break;
            case States::Going:
                result = "Going";
                break;
            case States::WillStop:
                result = "WillStop";
                break;
        }

        pServer->send(200, "text/plain", result.c_str());
    }

    void requestGo()
    {
        requestState = RequestStates::Go;
        pServer->send(200, "text/plain", "Go requested.");
        pSerial->println("Go requested.");
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
                    case RequestStates::Go:
                        digitalWrite(STOP, LOW);
                        digitalWrite(GO, HIGH);
                        currentState = States::Going;
                        requestState = None;
                        break;
                    default:
                        break;
                }
                break;
            case States::Going:
                switch (requestState)
                {
                    case RequestStates::Stop:
                        digitalWrite(GO, LOW);
                        digitalWrite(WILLSTOP, HIGH);
                        stopRemains = TRANSITION_WILLSTOP;
                        currentState = States::WillStop;
                        requestState = None;
                       break;
                    case RequestStates::Go:
                        requestState = None;
                        break;
                    default:
                        break;
                }
                break;
            case States::WillStop:
                stopRemains--;
                if (stopRemains == 0)
                {
                    digitalWrite(WILLSTOP, LOW);
                    digitalWrite(STOP, HIGH);
                    currentState = States::Stopped;
                }
                break;
        }

        digitalWrite(STATUS, tickStatus ? HIGH : LOW);
        tickStatus = !tickStatus;
    }

    static void tickHandler(RoadSignalController* pThis)
    {
        pThis->tick();
    }

public:
    RoadSignalController()
        : pServer(nullptr), pSerial(nullptr), tickStatus(false)
        , currentState(States::Stopped), stopRemains(0), requestState(RequestStates::None)
    {
    }

    void Init(ESP8266WebServer* pServer, HardwareSerial* pSerial)
    {
        this->pServer = pServer;
        this->pSerial = pSerial;

        pinMode(GO, OUTPUT);
        pinMode(WILLSTOP, OUTPUT);
        pinMode(STOP, OUTPUT);
        pinMode(STATUS, OUTPUT);

        delay(100);

        digitalWrite(GO, LOW);
        digitalWrite(WILLSTOP, LOW);
        digitalWrite(STOP, HIGH);
        digitalWrite(STATUS, LOW);

        pServer->on("/api/status", HTTP_GET, [&]() { requestStatus(); });
        pServer->on("/api/go", HTTP_GET, [&]() { requestGo(); });
        pServer->on("/api/stop", HTTP_GET, [&]() { requestStop(); });
        pServer->onNotFound([&]() { requestNotFound(); });

        ticker.attach_ms(1000, tickHandler, this);
    }
};

////////////////////////////////////////////////

ESP8266WebServer server(80);

RoadSignalController controller;

void setup(void)
{
    Serial.begin(115200);
    Wire.begin();

    delay(500);

    Serial.println("    ");
    Serial.println("RoadSignal start.");

    const auto localIP = IPAddress(192, 168, 4, 2);
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
