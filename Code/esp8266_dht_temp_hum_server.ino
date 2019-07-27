/* DHTServer - ESP8266 Webserver with a DHT sensor as an input
  Based on ESP8266Webserver, DHTexample, and BlinkWithoutDelay (thank you)
  Version 1.0 5/3/2014 Version 1.0 Mike Barela for Adafruit Industries
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#define DHTTYPE DHT11
#define DHTPIN 2

const char* ssid = "***";
const char* password = "***";
ESP8266WebServer server(80);

// Initialize DHT sensor
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold. It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value. The default for a 16mhz AVR is a value of 6. For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01© Adafruit Industries https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver Page 9 of 18DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

float humidity, temp_c; // Values read from sensor
String webString = ""; // String to display

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0; // will store last temp was read
const long interval = 2000; // interval at which to read sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);


void handle_root() {
  server.send(200, "text/plain", "Hello from the weather esp8266, read from /temp or /humidity");
  delay(100);
} 

void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200); // Serial connection from ESP-01 via 3.3v console cable
  dht.begin(); // initialize temperature sensor
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handle_root);
  server.on("/temp", []() { // if you add this subdirectory to your webserver call, you get text below :)
    gettemperature(); // read sensor
    webString = "Temperature: " + String((int)temp_c) + " C"; // Arduino has a hard time with float to string
    server.send(200, "text/plain", webString); // send to someones browser when asked
  });
  server.on("/humidity", []() { // if you add this subdirectory to your webserver call, you get text below :)
    gettemperature(); // read sensor
    webString = "Humidity: " + String((int)humidity) + "%";
    server.send(200, "text/plain", webString); // send to someones browser when asked
  });
  server.on("/sensors", []() { // if you add this subdirectory to your webserver call, you get text below :)
    gettemperature(); // read sensor
    webString = "Temperature: " + String((int)temp_c) + " C" + "\nHumidity: " + String((int)humidity) + "%";
    server.send(200, "text/plain", webString); // send to someones browser when asked
  });
  server.begin();
  Serial.println("HTTP server started");
} 

void loop(void)
{
  server.handleClient();
} 

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.© Adafruit Industries https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver Page 10 of 18
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;
    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity(); // Read humidity (percent)
    temp_c = dht.readTemperature(); // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}
