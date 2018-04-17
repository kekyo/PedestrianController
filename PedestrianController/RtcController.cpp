#include <Wire.h>

// https://github.com/NorthernWidget/DS3231
#include <DS3231.h>

////////////////////////////////////////////////

DateTime getRtcTimeValue()
{
    return RTClib::now();
}

////////////////////////////////////////////////

void setRtcTimeValue(const DateTime& time)
{
    DS3231 ds3231;

    ds3231.setClockMode(false); // set to 24h

    ds3231.setYear(time.year() % 100);
    ds3231.setMonth(time.month());
    ds3231.setDate(time.day());
    ds3231.setHour(time.hour());
    ds3231.setMinute(time.minute());
    ds3231.setSecond(time.second());
}
