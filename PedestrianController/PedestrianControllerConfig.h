#ifndef PEDESTRIAN_CONTROLLER_CONFIG_H
#define PEDESTRIAN_CONTROLLER_CONFIG_H

#define WALK 12   // ESP8266 (ESP-WROOM-02) IO12
#define STOP 13   // ESP8266 (ESP-WROOM-02) IO13
#define I2CSCL 5  // ESP8266 (ESP-WROOM-02) SCL
#define I2CSDA 4  // ESP8266 (ESP-WROOM-02) SDA

#define WALK_TIME 10000   // 10sec
#define STOP_TIME 10000   // 10sec
#define TRANSITION_COUNT 14
#define TRANSITION_TIME 500   // 500msec (must fixed)

#define TIME_SCHEDULE_ON 21
#define TIME_SCHEDULE_OFF 2

//  your network SSID (name)
#define WIFI_SSID "******"
// your network password
#define WIFI_PASSWORD "******"

#define SNTP_SERVER_FQDN "time.nist.gov"
#define SNTP_SERVER_PORT 2390

#define LOCAL_TIMEZONE_FROM_UTC 9

#endif
