// This code is derived from the HelloServer Example
// in the (ESP32) WebServer library .
//
// It hosts a webpage which has one temperature reading to display.
// The webpage is always the same apart from the reading which would change.
// The getTemp() function simulates getting a temperature reading.
// homePage.h contains 2 constant string literals which is the two parts of the
// webpage that never change.
// handleRoot() builds up the webpage by adding as a C++ String:
// homePagePart1 + getTemp() +homePagePart2
// It then serves the webpage with the command:
// server.send(200, "text/html", message);
// Note the text is served as html.
//
// Replace the code in the homepage.h file with your own website HTML code.
//
// This example requires only an ESP32 and download cable. No other hardware is reuired.
// A wifi SSID and password is required.
// Written by: Natasha Rohan  12/3/23
//
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include "homepage.h"


#include <Adafruit_PN532.h>
#define SERVO_PIN 18

Servo myservo;
rgb_lcd lcd;
int pir = 32;
const char* ssid = "";
const char* password = "";

WebServer server(80);


const uint8_t AUTHORIZED_UID[4] = {0xEB ,0x94 ,0x0E, 0x21}; // Example UID
#define PN532_I2C_ADDRESS 0x24  // Update with your device's address if necessary
Adafruit_PN532 nfc(21, 22);


// Variable to track the servo's position
int servoPosition = 0;
int pirState = LOW;
int ledState = LOW;
bool PirIS = false;//false//true
bool RFIDis= false;

String getservo() {
  if (servoPosition == 90) {
    return "On";  // Servo is at 90 degrees (on)
  } else {
    return "Off";  // Servo is at 0 degrees (off)
  }
}

void pirON (){
  PirIS = true;
 server.send(200, "text/plain", "PIR ON"); // Return the updated webpage
}
  
void pirOFF(){
  PirIS = false;
 server.send(200, "text/plain", "PIR OFF"); // Return the updated webpage

}
  

void handleRoot() {
  String message = homePagePart1;
  // Replace the %s placeholder with the servo's status (On or Off)
  message.replace("%s", getservo());
  message += homePagePart2;
  server.send(200, "text/html", message);  // Send the webpage with updated servo status
}

void handleTurnOn() {
  myservo.write(90);   // Move the servo to 90 degrees
  servoPosition = 90;  // Update the servo state
  lcd.clear();
  lcd.print("Servo On");
  lcd.setCursor(0, 1);
  lcd.print("Welcome!");
  digitalWrite(2, HIGH);  // Turn on LED
  ledState = HIGH;
  delay(5000);

  lcd.clear();
  myservo.write(0);   // Move the servo to 90 degrees
  servoPosition = 0;  // Update the servo state
  String message = homePagePart1;
  message.replace("%s", getservo());  // Update the servo status on the page
  message += homePagePart2;
 server.send(200, "text/plain", "Servo triggered!"); // Return the updated webpage
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {



  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/turnon", handleTurnOn);  // Correctly define the route
server.on("/piroff", pirOFF); 
server.on("/piron", pirON); 
  server.onNotFound(handleNotFound);
 pinMode(pir, INPUT);
 pinMode(2, OUTPUT);
  server.begin();


  myservo.attach(18);  // Attach the servo to pin 18
  lcd.begin(16, 2);
  lcd.print("Access Needed");

   bool nfc_status = false;
  
  while (!Serial) delay(10); // for Leonardo/Micro/Zero
  
  Serial.println("Setting up NFC reader and servo...");
  
  // Initialize the servo motor
 
  myservo.write(0); // Set initial position to 0 degrees

  // Initialize I2C bus
  Wire.begin();

  // Initialize the NFC reader
  nfc_status = nfc.begin();
  Serial.println(nfc_status);
  uint32_t versiondata = nfc.getFirmwareVersion();
  Serial.println(versiondata);
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1); // halt
    }

  // Print firmware version information
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware version: "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig(); // Configure the NFC reader for reading cards

  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop() {
  server.handleClient();
  delay(2);
    // Allow CPU to switch to other tasks

    if(PirIS==true){
      pirState = digitalRead(pir);
    if (pirState == HIGH) {
          lcd.setCursor(0, 0);
            lcd.print("Access Needed!!!");
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Welcome!");

            myservo.write(90);  // Move servo to 90 degrees
            digitalWrite(2, HIGH);  // Turn on LED
            ledState = HIGH;
            
            
        } else {
            myservo.write(0);  // Move servo to 0 degrees
            digitalWrite(2, LOW);  // Turn off LED
            ledState = LOW;

            lcd.setCursor(0, 0);
            lcd.print("Access Needed!!!");
          
          
        }
        
        delay(2000); // Adjust delay as needed
        lcd.clear(); 
    }
          if(RFIDis==true){
            
            
            
            uint8_t success;
            uint8_t uid[7];  // Buffer to store the returned UID
            uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on card type)

            // Check if a card is in range
            success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("access needed!");
            if (success) {
              // Card found
              Serial.println("Card detected!");

              // Check if the UID matches the authorized UID
              if (uidLength == 4 && checkUID(uid)) {
                Serial.println("Authorized card detected. Activating servo...");
                lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Welcome!");
                myservo.write(90);  // Turn servo to 90 degrees (open/activate)
                delay(2000);        // Keep it open for 2 seconds
                lcd.clear();
              } else {
                Serial.println("Unauthorized card or unknown card.");
                  lcd.clear();
                 lcd.setCursor(0, 0);
            lcd.print("Access Denied!!!");
            lcd.setCursor(0, 1);
            lcd.print("invalid card!!!");
            delay(2000);
            lcd.clear();
              }
              
              // Return the servo to the default position (closed)
              myservo.write(0);  
              delay(1000);  // Add delay to avoid multiple readings of the same card
            }
            }
          
}
bool checkUID(uint8_t *uid) {
  for (int i = 0; i < 4; i++) {
    if (uid[i] != AUTHORIZED_UID[i]) {
      return false;
    }
  }
  return true;
}
