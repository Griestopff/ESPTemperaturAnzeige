#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
//#include "sd_diskio.h"
//#include "SDS011.h" 
//#include "SdsDustSensor.h"

#include <Arduino.h>
//#include <TFT_eSPI.h>


#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_I2CDevice.h> //WICHTIG

#define DHTPIN 5     // Digital pin connected to the DHT sensor -> Data von Sensor an GPOI 5  //nicht 2 benutzen!!
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321 -> Welcher Sensor Typ

#define Threshold 40 /* Greater the value, more the sensitivity */

// For the Adafruit shield, these are the default.
#define TFT_DC 2
#define TFT_CS 17


//#define TFT_GREY 0x5AEB

//TFT_eSPI tft = TFT_eSPI(); 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


RTC_DATA_ATTR int bootCount = 0;
touch_pad_t touchPin;

DHT dht(DHTPIN, DHTTYPE); //DHT Objekt (dht), dem Pin und Typ zugeordnet werden
int LED_BUILTIN = 2;
const int ledPin = 15;

void print_wakeup_reason(){
    
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
/*
Method to print the touchpad by which ESP32
has been awaken from sleep
*/
void print_wakeup_touchpad(){

  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}

void callback(){
  //placeholder callback function
}

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode (LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  dht.begin();
  tft.begin();

  Serial.println("-----------------------------------------");
  print_wakeup_reason();
  print_wakeup_touchpad();
  touchAttachInterrupt(T0, callback, Threshold);
  Serial.println("-----------------------------------------");

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Temp:");

}

void loop() {

  digitalWrite(ledPin,HIGH);
  digitalWrite(LED_BUILTIN, HIGH);

  float t = dht.readTemperature();
    
  if(isnan(t)){
      Serial.println("Jo Temperatur konnte nicht gelesen werden...jo");
      tft.println("///");
      Serial.println("-----------------------------------------");
      delay(1000);
  }else
  {
    if(touchRead(T0) < 20){
      Serial.print("Temperatur: ");
      Serial.print(t);
      tft.print(t);
      Serial.println("");
      Serial.print("TouchRead: ");
      Serial.print(touchRead(T0));
      Serial.println("");
      Serial.println("-----------------------------------------");
      delay(1000);
    }
  }

  Serial.println("Gehe jetzt schlafen");
  esp_deep_sleep_start();
  Serial.println("Ich schlafe..also wird das nicht angezeigt..");

  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000);
}