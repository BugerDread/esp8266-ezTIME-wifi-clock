#include <ESP8266WiFi.h>
#include <esp8266-hw-spi-max7219-7seg.h>
#include <ezTime.h>

// configuration
static const char ourtimezone[] PROGMEM = "Europe/Prague";  //official timezone names https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define wifi_ssid             "network-name"                //ssid of your WiFi
#define wifi_pwd              "password"                    //password for your WiFi
#define SPI_SPEED             8000000                       //display SPI@8MHZ
#define SPI_CSPIN             5                             //display SPI CS=GPIO5
#define DISP_BRGTH            8                             //brightness of the display (0-15)
#define DISP_AMOUNT           2                             //number of display modules connected

BgrMax7seg disp = BgrMax7seg(SPI_SPEED, SPI_CSPIN, DISP_AMOUNT); //init display
Timezone myTZ;

void setup() {
  Serial.begin(115200);
  Serial.println(F("\n* * * ESP BOOT * * *"));
  setDebug(INFO);
  disp.init();
  disp.setBright(DISP_BRGTH, ALL_MODULES);
  disp.print(F("NTPclock"), 1);
  disp.print(F("bugerdrd"), 2);
  
  Serial.println(F("Connecting to WiFi"));
  WiFi.begin( wifi_ssid, wifi_pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("\nWiFi connected"));
  
  waitForSync();

  if (myTZ.setLocation(FPSTR(ourtimezone))) {
    Serial.println(F("Timezone lookup OK"));
  } else {
    Serial.println(F("\nTimezone lookup failed, will use UTC!"));
    myTZ = UTC;
  }
}

void loop() {
  events();
  if (secondChanged()) {
    disp.print(myTZ.dateTime("H-i-s"), 1);
    disp.print(myTZ.dateTime("d.m.Y"), 2);
  }
}
