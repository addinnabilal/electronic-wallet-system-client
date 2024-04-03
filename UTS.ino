
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;


const char* ssid = "";
const char* password = "";
int BUTTON_PIN = 0;
int LED_PIN = 2;

int buttonState = 0; 
int lastButtonState = HIGH; 

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  

  Serial.begin(9600); // Start the Serial communication to send messages to the computer
  WiFi.begin(ssid, password); // Connect to the network

  while (WiFi.status() != WL_CONNECTED) { // Wait for the connection to establish
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW) {
    if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
      HTTPClient http;

      Serial.println("Sending POST request");
      http.begin("https://card-payment-iot.vercel.app/api/pay"); // Specify request destination
      http.addHeader("Content-Type", "application/json"); 
      
      // Prepare your JSON payload as a String
      String payload = "{\"client_id\":\"13520045\"}";
      
      int httpCode = http.POST(payload); // Send the POST request with payload
      
      if (httpCode > 0) { //Check for the returning code
        String response = http.getString(); //Get the request response payload
        Serial.println(response); //Print the response payload
        
        if (httpCode == 200) {
          // LED on for 5 seconds to indicate success
          digitalWrite(LED_PIN, HIGH); 
          delay(5000); // Wait for 5 seconds
          digitalWrite(LED_PIN, LOW); // Turn off the LED
        } else if (httpCode == 400) {
          // LED blink for 5 seconds to indicate not found or other error
          for (int i = 0; i < 5; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(500);
            digitalWrite(LED_PIN, LOW);
            delay(500);
          }
        }
      } else {
        Serial.println("Error on HTTP request");
      }

      http.end(); //Close connection
    }
    delay(1000); // Simple debounce 
  }
  lastButtonState = buttonState;
}