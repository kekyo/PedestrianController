#include <Wire.h>

#define WALK 0    // ESP8266 (ESP-WROOM-02) IO0
#define STOP 16   // ESP8266 (ESP-WROOM-02) IO16
#define I2CSCL 5  // ESP8266 (ESP-WROOM-02) SCL
#define I2CSDA 4  // ESP8266 (ESP-WROOM-02) SDA

#define WALK_TIME 10000   // 10sec
#define STOP_TIME 10000   // 10sec
#define TRANSITION_COUNT 14
#define TRANSITION_TIME 500   // 500msec (must fixed)

// https://github.com/NorthernWidget/DS3231
#include <DS3231.h>

DateTime getNtpTimeValue();
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

    delay(200);

    Serial.println("    ");
    Serial.println("======================");
    Serial.println("Start up:");

    auto currentUTCTime = getNtpTimeValue();
    auto currentUTCTimeString = formatTime(currentUTCTime);

    Serial.print(currentUTCTimeString);
    Serial.println(": Ready.");
    Serial.println("======================");
    Serial.println();

    delay(1000);
}

////////////////////////////////////////////////

void loop()
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
    Serial.print("Stopping ...");

    digitalWrite(STOP, HIGH);
    delay(STOP_TIME);

    Serial.println(" Done");
}
