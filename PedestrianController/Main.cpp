#include <Wire.h>
#include <user_interface.h>

#include "PedestrianControllerConfig.h"

// https://github.com/NorthernWidget/DS3231
#include <DS3231.h>

bool getNtpTimeValue(DateTime& time, const uint16_t timeoutSecond);
DateTime getRtcTimeValue();
void setRtcTimeValue(const DateTime& time);

////////////////////////////////////////////////

static String formatTime(const DateTime& time)
{
    auto timeString = String(time.month(), DEC);
    timeString += "/";
    timeString += String(time.day(), DEC);
    timeString += "/";
    timeString += String(time.year(), DEC);
    timeString += " ";
    timeString += String(time.hour(), DEC);
    timeString += ":";
    timeString += String(time.minute(), DEC);
    timeString += ":";
    timeString += String(time.second(), DEC);

    return timeString;
}

////////////////////////////////////////////////

static bool isDisplaying(const bool force, const uint32_t timeout)
{
    const uint32_t start = millis();
    DateTime currentTime = getRtcTimeValue();

    String currentTimeString = formatTime(currentTime);

    Serial.print("Current time from RTC: ");
    Serial.println(currentTimeString);

    if (force
        || ((currentTime.hour() == 0) && (currentTime.minute() == 0) && (currentTime.second() == 0)))
    {
        Serial.println();
        Serial.println("======================");
        Serial.println("Time update start:");

        if (getNtpTimeValue(currentTime, 30))
        {
            setRtcTimeValue(currentTime);

            currentTimeString = formatTime(currentTime);

            Serial.print("Got and updated time from NTP: ");
            Serial.println(currentTimeString);
        }

        Serial.println("======================");
        Serial.println();
    }

    const uint32_t end = millis();
    if (end >= start)
    {
        auto differ = end - start;
        if (differ < timeout)
        {
            delay(timeout - differ);
        }
    }
    else
    {
        auto differ = UINT32_MAX - start + end;
        if (differ < timeout)
        {
            delay(timeout - differ);
        }
    }

    return (currentTime.hour() < TIME_SCHEDULE_OFF)
        || (currentTime.hour() >= TIME_SCHEDULE_ON);
}

////////////////////////////////////////////////

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(200);

    digitalWrite(WALK, LOW);
    digitalWrite(STOP, LOW);

    delay(100);

    pinMode(WALK, OUTPUT);
    pinMode(STOP, OUTPUT);

    Serial.println("    ");

    wifi_set_sleep_type(LIGHT_SLEEP_T);
}

////////////////////////////////////////////////

static bool isFirst = true;

void loop()
{
    if (isDisplaying(isFirst, STOP_TIME))
    {
        Serial.print("Walking ...");

        digitalWrite(STOP, LOW);
        digitalWrite(WALK, HIGH);
        delay(WALK_TIME);

        Serial.println(" Done");
        Serial.print("Transition ...");

        digitalWrite(WALK, LOW);

        for (int i = 0; i < TRANSITION_COUNT; i++)
        {
            Serial.print(" ");
            Serial.print(TRANSITION_COUNT - i, DEC);
        
            digitalWrite(STOP, HIGH);
            delay(TRANSITION_TIME);
            digitalWrite(STOP, LOW);
            delay(TRANSITION_TIME);
        }

        Serial.println(" Done");

        digitalWrite(STOP, HIGH);
    }
    else
    {
        Serial.println("Sleeping 10 minutes");

        digitalWrite(WALK, LOW);
        digitalWrite(STOP, LOW);

        delay(10 * 60 * 1000);
    }

    isFirst = false;
}
