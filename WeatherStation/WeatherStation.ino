#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "DHTesp.h"

// Display class
SSD1306Wire display(0x3c, D3, D4);

// DHT class
DHTesp dht;

// Message Buffers
char buf_time[32];
char buf_temp[32];
char buf_humidity[32];

void setup() {
  Serial.begin(115200);

  // Configure OLED
  display.init();
  display.clear();
  display.display();

  // Configure DHT
  dht.setup(D6, DHTesp::DHT11);
}

void loop() {

  // Blocking delay to wait for new data
  delay(1000);

  // Pull latest data 
  sprintf(buf_time, "   Time sec: %d ", millis()/1000);
  sprintf(buf_temp, "   Temperature C: %.1f ", dht.getTemperature());
  sprintf(buf_humidity, "   Humidity %: %.1f ", dht.getHumidity());
  
  // Reset the display before writing new information
  display.clear();

  // Update screen buffer with the new values for time, temp, humidity
  display.drawString(0, 0, buf_time);
  display.drawString(0, 12, buf_temp); 
  display.drawString(0, 24, buf_humidity);

  // Transmit the screen buffer to the OLED to be displayed
  display.display();

  // Also write the values to the serial monitor
  Serial.print(buf_time);
  Serial.print(buf_temp);
  Serial.println(buf_humidity);
}
