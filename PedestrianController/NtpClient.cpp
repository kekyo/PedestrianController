#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// https://github.com/NorthernWidget/DS3231
#include <DS3231.h>

////////////////////////////////////////////////

#include "PedestrianControllerConfig.h"

static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
static byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

void BlinkStatus(const uint16_t msec);

////////////////////////////////////////////////

// send an NTP request to the time server at the given address
static void sendNtpPacket(WiFiUDP &udp, const IPAddress &address)
{
    Serial.println("sending NTP packet...");

    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

////////////////////////////////////////////////

bool getNtpTimeValue(DateTime& time, const uint16_t timeoutSecond)
{
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    BlinkStatus(600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // 30sec
    auto connected = false;
    for (int i = 0; i < (timeoutSecond * 2); i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            connected = true;
            break;
        }
        delay(500);
        Serial.print(".");
    }

    if (!connected)
    {
        BlinkStatus(300);

        //WiFi.mode(WIFI_OFF);
        //WiFi.forceSleepBegin();

        Serial.println(" timeout");

        delay(3000);

        BlinkStatus(UINT16_MAX);

        return false;
    }

    Serial.println(" connected");

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Starting UDP");

    WiFiUDP udp;
    udp.begin(SNTP_SERVER_PORT);

    Serial.print("Local port: ");
    Serial.println(udp.localPort());

    //get a random server from the pool
    IPAddress timeServerIP;
    WiFi.hostByName(SNTP_SERVER_FQDN, timeServerIP);

    sendNtpPacket(udp, timeServerIP); // send an NTP packet to a time server

    delay(1000);

    const int cb = udp.parsePacket();
    if (!cb)
    {
        BlinkStatus(100);

        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        WiFi.forceSleepBegin();

        Serial.println("no packet yet");

        delay(3000);
        
        BlinkStatus(UINT16_MAX);

        return false;
    }

    Serial.print("packet received, length=");
    Serial.println(cb);

    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();

    BlinkStatus(UINT16_MAX);

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    const uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);
    const uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);

    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    const uint32_t secsSince1900 = highWord << 16 | lowWord;

    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const uint32_t seventyYears = 2208988800UL;

    // subtract seventy years:
    const uint32_t epoch = secsSince1900 - seventyYears;
    
    const uint32_t localTime = epoch + LOCAL_TIMEZONE_FROM_UTC * 3600;

    time = DateTime(localTime);
    return true;
}
