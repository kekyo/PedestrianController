#include <Wire.h>
#include <Ticker.h>
#include <user_interface.h>

#include "PedestrianControllerConfig.h"

// https://github.com/NorthernWidget/DS3231
#include <DS3231.h>

bool getNtpTimeValue(DateTime& time);
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

static uint32_t calculateTimeDifferent(const uint32_t start, const uint32_t end)
{
    if (end >= start)
    {
        return end - start;
    }
    else
    {
        return UINT32_MAX - start + end;
    }
}

static void ChangeBlinkState()
{
    static uint8_t currentState = 3;

    switch (currentState)
    {
        case 0:
        case 1:
        case 2:
            digitalWrite(STATUS, LOW);
            currentState++;
            break;
        default:
            digitalWrite(STATUS, HIGH);
            currentState = 0;
            break;
    }
}

void BlinkStatus(const uint16_t msec)
{
    static Ticker ticker;

    if (msec == 0)
    {
        ticker.detach();
        digitalWrite(STATUS, LOW);
    }
    else if (msec == UINT16_MAX)
    {
        ticker.detach();
        digitalWrite(STATUS, HIGH);
    }
    else
    {
        digitalWrite(STATUS, HIGH);
        ticker.attach_ms(msec / 4, ChangeBlinkState);
    }
}

////////////////////////////////////////////////

static uint8_t lastUpdatedDay = 0;

static uint32_t getSleepingSecond(const uint32_t timeoutMillisecond)
{
    const uint32_t start = millis();
    DateTime currentTime = getRtcTimeValue();

    String currentTimeString = formatTime(currentTime);

    Serial.print("Current time from RTC: ");
    Serial.println(currentTimeString);

    if (currentTime.day() != lastUpdatedDay)
    {
        Serial.println();
        Serial.println("======================");
        Serial.println("Time update start:");

        DateTime ntpTime;
        if (getNtpTimeValue(ntpTime))
        {
            setRtcTimeValue(ntpTime);

            currentTime = ntpTime;
            currentTimeString = formatTime(currentTime);

            Serial.print("Got and updated time from NTP: ");
            Serial.println(currentTimeString);
        }

        lastUpdatedDay = currentTime.day();

        Serial.println("======================");
        Serial.println();
    }

    const uint32_t end = millis();
    const uint32_t differ = calculateTimeDifferent(start, end);
    if (differ < timeoutMillisecond)
    {
        delay(timeoutMillisecond - differ);
    }

    const uint8_t hour = currentTime.hour();

#if (TIME_SCHEDULE_ON >= TIME_SCHEDULE_OFF)
    if ((hour >= TIME_SCHEDULE_ON) || (hour < TIME_SCHEDULE_OFF))
    {
        return 0;
    }
#else
    if ((hour >= TIME_SCHEDULE_ON) && (hour < TIME_SCHEDULE_OFF))
    {
        return 0;
    }
#endif

    const DateTime next = DateTime(
        currentTime.year(),
        currentTime.month(),
        ((hour > TIME_SCHEDULE_ON) ? 1 : 0) + currentTime.day(),
        TIME_SCHEDULE_ON,
        0,
        0);

    return (next.unixtime() - currentTime.unixtime()) * 1000;
}

////////////////////////////////////////////////

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(500);

    digitalWrite(WALK, LOW);
    digitalWrite(STOP, LOW);

    BlinkStatus(UINT16_MAX);

    delay(100);

    pinMode(WALK, OUTPUT);
    pinMode(STOP, OUTPUT);
    pinMode(STATUS, OUTPUT);

    Serial.println("    ");

    wifi_set_sleep_type(LIGHT_SLEEP_T);

    delay(200);
}

////////////////////////////////////////////////

void loop()
{
    const uint32_t requireMillisecond = getSleepingSecond(STOP_TIME);
    if (requireMillisecond == 0)
    {
        BlinkStatus(0);

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
        const uint32_t sleepMillisecond =
            (requireMillisecond < (10 * 60 * 1000))
                ? requireMillisecond
                : (10 * 60 * 1000);

        Serial.print("Sleeping ");
        Serial.println(sleepMillisecond / (60 * 1000), DEC);

        digitalWrite(WALK, LOW);
        digitalWrite(STOP, LOW);

        BlinkStatus(3000);

        delay(sleepMillisecond);
    }
}
