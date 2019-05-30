/*
   Ported to ESP32 by LeRoy Miller May 2019
    - Mostly working port
    - telnet server doesn't work (not sure how to fix it)

   Change example.com to your needs.
   You will also have to create your own backend for updates.


   C64-WiFiModem - (c) 2017 my2bits <markus at borgelin.org>
   changes here
   Built hardware based on ESP-12, 3.3V-reg and level shifters.
   Tidy up help, and categorizing
   Added automatic firmware check at startup
   Corrected connectWifi() link check
   Changed MAX_CMD_LENGTH to actual FIFO buffer size
   Fixed help-bug in 9600+ baud
   Corrected setBaudRate(), bauds[] array and sizeof, to error out correct
   Adjusted pinouts for my needs
   Adjusted default servers
   Added modem restart command
   Added OLED display with information
   Added OTA Update
   Added scan for wifi
   Some fixes to Pin polarity, does not work as expected though. Fixes welcome
   Probably added more stuff I forgot to write here.
   Thanks to Paul Richards and Jussi Salin for source code.

   based on
   WiFi SIXFOUR - A virtual WiFi modem based on the ESP 8266 chipset
   Copyright (C) 2016 Paul Rickards <rickards@gmail.com>
   Added EEPROM read/write, status/help pages, busy answering of incoming calls
   uses the readily available Sparkfun ESP8266 WiFi Shield which has 5v level
   shifters and 3.3v voltage regulation present-- easily connect to a C64
   https://www.sparkfun.com/products/13287

   based on
   ESP8266 based virtual modem
   Copyright (C) 2016 Jussi Salin <salinjus@gmail.com>

   https://github.com/jsalin/esp8266_modem

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>

// OTA stuff
//#include <ESP8266WiFiMulti.h>
#include <HTTPClient.h>
//#include <HTTPUpdate.h>
//ESP8266WiFiMulti WiFiMulti;
// OTA stuff end

// OLED stuff
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"
//#include "SSD1306Brzo.h"

 #include "SH1106.h"
//#define OLED_RESET LED_BUILTIN // Changed reset pin to LED_BUILTIN to be able to use D4,D5
//SSD1306 display(0x3c, D4, D5); // Initialize the OLED display using Wire library. ADDR,SDA,SCL
//SSD1306Brzo display(0x3c, D5, D7); //TODO: Free up a pin. D5,D7...
SH1106 display(0x3c, 21, 22);

// OLED stuff end
/*
// These translation tables are not used yet.
static unsigned char petToAscTable[256] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x14,0x09,0x0d,0x11,0x93,0x0a,0x0e,0x0f,
0x10,0x0b,0x12,0x13,0x08,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x5b,0x5c,0x5d,0x5e,0x5f,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x0c,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf
};

static unsigned char ascToPetTable[256] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x14,0x09,0x0d,0x11,0x93,0x0a,0x0e,0x0f,
0x10,0x0b,0x12,0x13,0x08,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0x5b,0x5c,0x5d,0x5e,0x5f,
0xc0,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0xdb,0xdc,0xdd,0xde,0xdf,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x0c,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};
*/
#define VERSIONA 0
#define VERSIONB 1
#define VERSION_ADDRESS 0    // EEPROM address
#define VERSION_LEN     2    // Length in bytes
#define SSID_ADDRESS    2
#define SSID_LEN        32
#define PASS_ADDRESS    34
#define PASS_LEN        63
#define IP_TYPE_ADDRESS 97   // for future use
#define STATIC_IP_ADDRESS 98 // length 4, for future use
#define STATIC_GW       102  // length 4, for future use
#define STATIC_DNS      106  // length 4, for future use
#define STATIC_MASK     110  // length 4, for future use
#define BAUD_ADDRESS    111
#define ECHO_ADDRESS    112
#define SERVER_PORT_ADDRESS 113 // 2 bytes
#define AUTO_ANSWER_ADDRESS 115 // 1 byte
#define TELNET_ADDRESS  116     // 1 byte
#define VERBOSE_ADDRESS 117
#define PET_TRANSLATE_ADDRESS 118
#define FLOW_CONTROL_ADDRESS 119
#define PIN_POLARITY_ADDRESS 120
#define DIAL0_ADDRESS   200
#define DIAL1_ADDRESS   250
#define DIAL2_ADDRESS   300
#define DIAL3_ADDRESS   350
#define DIAL4_ADDRESS   400
#define DIAL5_ADDRESS   450
#define DIAL6_ADDRESS   500
#define DIAL7_ADDRESS   550
#define DIAL8_ADDRESS   600
#define DIAL9_ADDRESS   650
#define BUSY_MSG_ADDRESS 700
#define BUSY_MSG_LEN    80
#define LAST_ADDRESS    780

//#define SWITCH_PIN 0        // GPIO0 (programming mode pin)
//#define LED_PIN 12          // Status LED
//#define DCD_PIN 2           // DCD Carrier Status
//#define RTS_PIN 4           // RTS Request to Send, connect to host's CTS pin
//#define CTS_PIN 5           // CTS Clear to Send, connect to host's RTS pin
//#define DISP_SDA 14         // Display SDA pin
//#define DISP_SCL 13         //Display SCL pin

// Global variables
String build = "9";
String firmwareVersion =  "C64-wifimodem-" +  String(VERSIONA) + "." + String(VERSIONB) + "." + build;
String cmd = "";           // Gather a new AT command to this string from serial
bool cmdMode = true;       // Are we in AT command mode or connected mode
bool callConnected = false;// Are we currently in a call
bool telnet = false;       // Is telnet control code handling enabled
bool verboseResults = false;
//#define DEBUG 1          // Print additional debug information to serial channel
#undef DEBUG
#define LISTEN_PORT 6400   // Listen to this if not connected. Set to zero to disable.
int tcpServerPort = LISTEN_PORT;
#define RING_INTERVAL 3000 // How often to print RING when having a new incoming connection (ms)
unsigned long lastRingMs = 0; // Time of last "RING" message (millis())
//long myBps;                // What is the current BPS setting
#define MAX_CMD_LENGTH 128 // Maximum length for AT command
char plusCount = 0;        // Go to AT mode at "+++" sequence, that has to be counted
unsigned long plusTime = 0;// When did we last receive a "+++" sequence
#define LED_TIME 15         // How many ms to keep LED on at activity
unsigned long ledTime = 0;
#define TX_BUF_SIZE 256    // Buffer where to read from serial before writing to TCP
// (that direction is very blocking by the ESP TCP stack,
// so we can't do one byte a time.)
uint8_t txBuf[TX_BUF_SIZE];
const int speedDialAddresses[] = { DIAL0_ADDRESS, DIAL1_ADDRESS, DIAL2_ADDRESS, DIAL3_ADDRESS, DIAL4_ADDRESS, DIAL5_ADDRESS, DIAL6_ADDRESS, DIAL7_ADDRESS, DIAL8_ADDRESS, DIAL9_ADDRESS };
String speedDials[10];
const long bauds[] = { 300, 1200, 2400, 4800, 9600, 19200, 38400 }; // not needed yet  57600, 115200
byte serialspeed;
bool echo = true;
bool autoAnswer = false;
String ssid, password, busyMsg;
byte ringCount = 0;
String resultCodes[] = { "OK", "CONNECT", "RING", "NO CARRIER", "ERROR", "", "NO DIALTONE", "BUSY", "NO ANSWER" };
enum resultCodes_t { R1_OK, R1_CONNECT, R1_RING, R1_NOCARRIER, R1_ERROR, R1_NONE, R1_NODIALTONE, R1_BUSY, R1_NOANSWER };
unsigned long connectTime = 0;
bool petTranslate = false; // Fix PET MCTerm 1.26C Pet->ASCII encoding to actual ASCII
bool hex = false;
enum flowControl_t { F_NONE, F_HARDWARE, F_SOFTWARE };
byte flowControl = F_NONE;      // Use flow control
bool txPaused = false;          // Has flow control asked us to pause?
enum pinPolarity_t { P_CBM, P_NORMAL }; // Is LOW (0) or HIGH (1) active?
byte pinPolarity = P_CBM;
//bool ota_flag = false;  // Start OTA in main loop

// Telnet codes
#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define DONT 0xfe



WiFiClient tcpClient;
WiFiServer tcpServer(tcpServerPort);
WebServer webServer(80);
MDNSResponder mdns;

/*---------------------------------------------------------------------- */

String connectTimeString() {
    unsigned long now = millis();
    int secs = (now - connectTime) / 1000;
    int mins = secs / 60;
    int hours = mins / 60;
    String out = "";
    if (hours < 10) out.concat("0");
    out.concat(String(hours));
    out.concat(":");
    if (mins % 60 < 10) out.concat("0");
    out.concat(String(mins % 60));
    out.concat(":");
    if (secs % 60 < 10) out.concat("0");
    out.concat(String(secs % 60));
    return out;
}

void writeSettings() {
    setEEPROM(ssid, SSID_ADDRESS, SSID_LEN);
    setEEPROM(password, PASS_ADDRESS, PASS_LEN);
    setEEPROM(busyMsg, BUSY_MSG_ADDRESS, BUSY_MSG_LEN);

    EEPROM.write(BAUD_ADDRESS, serialspeed);
    EEPROM.write(ECHO_ADDRESS, byte(echo));
    EEPROM.write(AUTO_ANSWER_ADDRESS, byte(autoAnswer));
    EEPROM.write(SERVER_PORT_ADDRESS, highByte(tcpServerPort));
    EEPROM.write(SERVER_PORT_ADDRESS + 1, lowByte(tcpServerPort));
    EEPROM.write(TELNET_ADDRESS, byte(telnet));
    EEPROM.write(VERBOSE_ADDRESS, byte(verboseResults));
    EEPROM.write(PET_TRANSLATE_ADDRESS, byte(petTranslate));
    EEPROM.write(FLOW_CONTROL_ADDRESS, byte(flowControl));
    EEPROM.write(PIN_POLARITY_ADDRESS, byte(pinPolarity));

    for (int i = 0; i < 10; i++) {
        setEEPROM(speedDials[i], speedDialAddresses[i], 50);
    }
    EEPROM.commit();
}

void readSettings() {
    echo = EEPROM.read(ECHO_ADDRESS);
    autoAnswer = EEPROM.read(AUTO_ANSWER_ADDRESS);
    // serialspeed = EEPROM.read(BAUD_ADDRESS);

    ssid = getEEPROM(SSID_ADDRESS, SSID_LEN);
    password = getEEPROM(PASS_ADDRESS, PASS_LEN);
    busyMsg = getEEPROM(BUSY_MSG_ADDRESS, BUSY_MSG_LEN);
    tcpServerPort = word(EEPROM.read(SERVER_PORT_ADDRESS), EEPROM.read(SERVER_PORT_ADDRESS + 1));
    telnet = EEPROM.read(TELNET_ADDRESS);
    verboseResults = EEPROM.read(VERBOSE_ADDRESS);
    petTranslate = EEPROM.read(PET_TRANSLATE_ADDRESS);
    flowControl = EEPROM.read(FLOW_CONTROL_ADDRESS);
    pinPolarity = EEPROM.read(PIN_POLARITY_ADDRESS);

    for (int i = 0; i < 10; i++) {
        speedDials[i] = getEEPROM(speedDialAddresses[i], 50);
    }
}

void defaultEEPROM() {
    EEPROM.write(VERSION_ADDRESS, VERSIONA);
    EEPROM.write(VERSION_ADDRESS + 1, VERSIONB);

    setEEPROM("", SSID_ADDRESS, SSID_LEN);
    setEEPROM("", PASS_ADDRESS, PASS_LEN);
    setEEPROM("d", IP_TYPE_ADDRESS, 1);
    EEPROM.write(SERVER_PORT_ADDRESS, highByte(LISTEN_PORT));
    EEPROM.write(SERVER_PORT_ADDRESS + 1, lowByte(LISTEN_PORT));

    EEPROM.write(BAUD_ADDRESS, 0x00);
    EEPROM.write(ECHO_ADDRESS, 0x01);
    EEPROM.write(AUTO_ANSWER_ADDRESS, 0x01);
    EEPROM.write(TELNET_ADDRESS, 0x00);
    EEPROM.write(VERBOSE_ADDRESS, 0x01);
    EEPROM.write(PET_TRANSLATE_ADDRESS, 0x00);
    EEPROM.write(FLOW_CONTROL_ADDRESS, 0x00);
    EEPROM.write(PIN_POLARITY_ADDRESS, 0x00);

    setEEPROM("bbs.retrohack.se:6464", speedDialAddresses[0], 50);
    setEEPROM("deltacity.se:2323", speedDialAddresses[1], 50);
    setEEPROM("bbs.retrohack.se:8484", speedDialAddresses[2], 50);
    setEEPROM("borderlinebbs.dyndns.org:6400", speedDialAddresses[3], 50);
    setEEPROM("cottonwoodbbs.dyndns.org:6502", speedDialAddresses[4], 50);
    setEEPROM("eaglebird.ddns.net:2300", speedDialAddresses[5], 50);
    setEEPROM("bbs.jammingsignal.com:23", speedDialAddresses[6], 50);
    for (int i = 7; i < 10; i++) {
      setEEPROM("", speedDialAddresses[i], 50);
    }

    setEEPROM("SORRY, THE BBS IS CURRENTLY BUSY. PLEASE TRY AGAIN LATER.", BUSY_MSG_ADDRESS, BUSY_MSG_LEN);
    EEPROM.commit();
}

String getEEPROM(int startAddress, int len) {
    String myString;

    for (int i = startAddress; i < startAddress + len; i++) {
        if (EEPROM.read(i) == 0x00) {
            break;
        }
        myString += char(EEPROM.read(i));
      //Serial.print(char(EEPROM.read(i)));
    }
    //Serial.println();
    return myString;
}

void setEEPROM(String inString, int startAddress, int maxLen) {
    for (int i = startAddress; i < inString.length() + startAddress; i++) {
        EEPROM.write(i, inString[i - startAddress]);
        //Serial.print(i, DEC); Serial.print(": "); Serial.println(inString[i - startAddress]);
        //if (EEPROM.read(i) != inString[i - startAddress]) { Serial.print(" (!)"); }
        //Serial.println();
    }
    // null pad the remainder of the memory space
    for (int i = inString.length() + startAddress; i < maxLen + startAddress; i++) {
        EEPROM.write(i, 0x00);
        //Serial.print(i, DEC); Serial.println(": 0x00");
    }
}

void sendResult(int resultCode) {
    Serial.print("\r\n");
    if (verboseResults == 0) {
        Serial.println(resultCode);
        return;
    }
    if (resultCode == R1_CONNECT) {
        Serial.print(String(resultCodes[R1_CONNECT]) + " " + String(bauds[serialspeed]));
    } else if (resultCode == R1_NOCARRIER) {
        Serial.print(String(resultCodes[R1_NOCARRIER]) + " (" + connectTimeString() + ")");
    } else {
        Serial.print(String(resultCodes[resultCode]));
    }
    Serial.print("\r\n");
}

void sendString(String msg) {
    Serial.print("\r\n");
    Serial.print(msg);
    Serial.print("\r\n");
}

// Hold for 5 seconds to switch to 300 baud
// Slow flash: keep holding
// Fast flash: let go
int checkButton() {
    /*
    long time = millis();
    while (digitalRead(SWITCH_PIN) == LOW && millis() - time < 5000) {
        delay(250);
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        yield();
    }
    if (millis() - time > 5000) {
        Serial.flush();
        Serial.end();
        serialspeed = 0;
        delay(100);
        Serial.begin(bauds[serialspeed]);
        sendResult(R1_OK);
        while (digitalRead(SWITCH_PIN) == LOW) {
            delay(50);
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            yield();
        }
        return 1;
    } else {
        return 0;
    }
    */
}

void connectWiFi() {
    if (ssid == "" || password == "") {
        Serial.println("CONFIGURE YOUR WIFI SETTINGS.\r\nAT? FOR HELP.");
        return;
    }
    if (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid.c_str(), password.c_str()); yield(); delay(100);
        Serial.print("\nCONNECTING TO SSID "); Serial.print(ssid);
        uint8_t i = 0;
        while (WiFi.status() != WL_CONNECTED && i++ < 20) {
//            digitalWrite(LED_PIN, LOW);
            delay(250);
//            digitalWrite(LED_PIN, HIGH);
            delay(250);
            Serial.print(".");
        }
    Serial.println();
    }
    if (WiFi.status() == WL_CONNECTED) { //TODO Fix to WL_CONNECTED instead of if (i == 21)
        Serial.print("WIFI CONNECTED TO: "); Serial.println(WiFi.SSID());
        Serial.print("YOUR IP: "); Serial.println(WiFi.localIP());
        updateLed();
        // Check if there is a new firmware
        HTTPClient httpFWcheck;
        httpFWcheck.useHTTP10(true);
        httpFWcheck.setTimeout(8000);
        httpFWcheck.begin("http://example.com/update-c64-wifimodem.php?doupdate=false");
        httpFWcheck.setUserAgent("ESP8266-http-Update");
        httpFWcheck.addHeader("x-ESP8266-STA-MAC", WiFi.macAddress());
        // httpFWcheck.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
        // httpFWcheck.addHeader(F("x-ESP8266-free-space"), String(ESP.getFreeSketchSpace()));
        // httpFWcheck.addHeader(F("x-ESP8266-sketch-size"), String(ESP.getSketchSize()));
        // httpFWcheck.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
        // httpFWcheck.addHeader(F("x-ESP8266-chip-size"), String(ESP.getFlashChipRealSize()));
        // httpFWcheck.addHeader(F("x-ESP8266-sdk-version"), ESP.getSdkVersion());
        // httpFWcheck.addHeader(F("x-ESP8266-mode"), F("sketch"));
        httpFWcheck.addHeader("x-ESP8266-version", firmwareVersion);
            int httpCode = httpFWcheck.GET();
            // Serial.println("DEBUG:" + String(httpCode));
            switch(httpCode) {
                case 200:
                    Serial.println("UPDATE AVAILABLE");
                    break;
                case 304:
                    Serial.println("FIRMWARE IS UP TO DATE");
                    break;
                default:
                    //  Serial.println("COULD NOT CHECK FOR UPDATE");
                    break;
            }
        httpFWcheck.end();
    } else {
        Serial.print("COULD NOT CONNECT TO "); Serial.println(ssid);
        WiFi.disconnect();
        updateLed();
    }
}

void updateLed() {
    if (WiFi.status() == WL_CONNECTED) {
//        digitalWrite(LED_PIN, LOW);  // on
    } else {
//        digitalWrite(LED_PIN, HIGH); //off
    }
}

void disconnectWiFi() {
    WiFi.disconnect();
    updateLed();
}

void setBaudRate(long inSpeed) { //TODO: change from int as its only 32767
    // Serial.println("DEBUG:" + String(inSpeed));
    if (inSpeed == 0) {
        sendResult(R1_ERROR);
        return;
    }
    int foundBaud = -1;
    for (int i = 0; i < (sizeof(bauds)/sizeof(long)); i++) {
        if (inSpeed == bauds[i]) {
            foundBaud = i;
            break;
        }
    }
   // Serial.println("DEBUG:" + String(foundBaud));
    // requested baud rate not found, return error
    if (foundBaud == -1) {
        sendResult(R1_ERROR);
        return;
    }
    if (foundBaud == serialspeed) {
        sendResult(R1_OK);
        return;
    }
    Serial.print("SWITCHING SERIAL PORT TO ");
    Serial.print(inSpeed);
    Serial.println(" IN 5 SECONDS");
    delay(5000);
    Serial.end();
    delay(200);
    Serial.begin(bauds[foundBaud]);
    serialspeed = foundBaud;
    delay(200);
    sendResult(R1_OK);
}

void setCarrier(byte carrier) {
/*    if (pinPolarity == P_CBM) carrier = !carrier;
    digitalWrite(DCD_PIN, carrier);
*/
}

void displayNetworkStatus() {
    Serial.print("WIFI STATUS: ");
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("CONNECTED");
    }
    if (WiFi.status() == WL_IDLE_STATUS) {
        Serial.println("OFFLINE");
    }
    if (WiFi.status() == WL_CONNECT_FAILED) {
        Serial.println("CONNECT FAILED");
    }
    if (WiFi.status() == WL_NO_SSID_AVAIL) {
        Serial.println("SSID UNAVAILABLE");
    }
    if (WiFi.status() == WL_CONNECTION_LOST) {
        Serial.println("CONNECTION LOST");
    }
    if (WiFi.status() == WL_DISCONNECTED) {
        Serial.println("DISCONNECTED");
    }
    if (WiFi.status() == WL_SCAN_COMPLETED) {
        Serial.println("SCAN COMPLETED");
    }
    yield();

    Serial.print("SSID.......: "); Serial.println(WiFi.SSID());

    //  Serial.print("ENCRYPTION: ");
    //  switch(WiFi.encryptionType()) {
    //    case 2:
    //      Serial.println("TKIP (WPA)");
    //      break;
    //    case 5:
    //      Serial.println("WEP");
    //      break;
    //    case 4:
    //      Serial.println("CCMP (WPA)");
    //      break;
    //    case 7:
    //      Serial.println("NONE");
    //      break;
    //    case 8:
    //      Serial.println("AUTO");
    //      break;
    //    default:
    //      Serial.println("UNKNOWN");
    //      break;
    //  }

    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC ADDRESS: ");
    Serial.print(mac[0], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.println(mac[5], HEX);
    yield(); delay(100);

    Serial.print("IP ADDRESS.: "); Serial.println(WiFi.localIP()); yield(); delay(50);
    Serial.print("GATEWAY....: "); Serial.println(WiFi.gatewayIP()); yield(); delay(50);
    Serial.print("SUBNET MASK: "); Serial.println(WiFi.subnetMask()); yield(); delay(50);
    Serial.print("SERVER PORT: "); Serial.println(tcpServerPort); yield(); delay(50);
    Serial.print("WEB CONFIG.: HTTP://"); Serial.println(WiFi.localIP()); yield(); delay(50);
    Serial.print("CALL STATUS: "); yield(); delay(50);
    if (callConnected) {
        Serial.print("CONNECTED TO "); Serial.println(ipToString(tcpClient.remoteIP())); yield(); delay(50);
        Serial.print("CALL LENGTH: "); Serial.println(connectTimeString()); yield(); delay(50);
    } else {
        Serial.println("NOT CONNECTED"); delay(50);
    }
}

void displayCurrentSettings() {
    Serial.println("ACTIVE PROFILE:"); yield(); delay(50);
    Serial.print("BAUD: "); Serial.println(bauds[serialspeed]); yield(); delay(50);
    Serial.print("SSID: "); Serial.println(ssid); yield(); delay(50);
    Serial.print("PASS: "); Serial.println(password); yield(); delay(50);
    //  Serial.print("SERVER TCP PORT: "); Serial.println(tcpServerPort); yield(); delay(50);
    Serial.print("BUSY MSG: "); Serial.println(busyMsg); yield(); delay(50);
    Serial.print("E"); Serial.print(echo); Serial.print(" "); yield(); delay(10);
    Serial.print("V"); Serial.print(verboseResults); Serial.print(" "); yield(); delay(10);
    Serial.print("&K"); Serial.print(flowControl); Serial.print(" "); yield(); delay(10);
    Serial.print("&P"); Serial.print(pinPolarity); Serial.print(" "); yield(); delay(10);
    //  Serial.print("NET"); Serial.print(telnet); Serial.print(" "); yield(); delay(10);
    //  Serial.print("PET"); Serial.print(petTranslate); Serial.print(" "); yield(); delay(10);
    Serial.print("S0:"); Serial.print(autoAnswer); Serial.print(" "); yield(); delay(10);
    Serial.println(); yield(); delay(50);

    Serial.println("SPEED DIAL HOSTS:");
    for (int i = 0; i < 10; i++) {
        Serial.print(i); Serial.print(": "); Serial.println(speedDials[i]);
        yield(); delay(50);
    }
    Serial.println();
}

void displayStoredSettings() {
    Serial.println("STORED PROFILE:"); yield(); delay(50);
    Serial.print("BAUD: "); Serial.println(bauds[EEPROM.read(BAUD_ADDRESS)]); yield(); delay(50);
    Serial.print("SSID: "); Serial.println(getEEPROM(SSID_ADDRESS, SSID_LEN)); yield(); delay(50);
    Serial.print("PASS: "); Serial.println(getEEPROM(PASS_ADDRESS, PASS_LEN)); yield(); delay(50);
    //  Serial.print("SERVER TCP PORT: "); Serial.println(word(EEPROM.read(SERVER_PORT_ADDRESS), EEPROM.read(SERVER_PORT_ADDRESS+1))); yield(); delay(50);
    Serial.print("BUSY MSG: "); Serial.println(getEEPROM(BUSY_MSG_ADDRESS, BUSY_MSG_LEN)); yield(); delay(50);
    Serial.print("E"); Serial.print(EEPROM.read(ECHO_ADDRESS)); Serial.print(" "); yield(); delay(50);
    Serial.print("V"); Serial.print(EEPROM.read(VERBOSE_ADDRESS)); Serial.print(" "); yield(); delay(50);
    Serial.print("&K"); Serial.print(EEPROM.read(FLOW_CONTROL_ADDRESS)); Serial.print(" "); yield(); delay(50);
    Serial.print("&P"); Serial.print(EEPROM.read(PIN_POLARITY_ADDRESS)); Serial.print(" "); yield(); delay(50);
    //  Serial.print("NET"); Serial.print(EEPROM.read(TELNET_ADDRESS)); Serial.print(" "); yield(); delay(50);
    //  Serial.print("PET"); Serial.print(EEPROM.read(PET_TRANSLATE_ADDRESS)); Serial.print(" "); yield(); delay(50);
    Serial.print("S0:"); Serial.print(EEPROM.read(AUTO_ANSWER_ADDRESS)); Serial.print(" "); yield(); delay(50);
    Serial.println(); yield(); delay(50);

    Serial.println("STORED SPEED DIAL HOSTS:");
    for (int i = 0; i < 10; i++) {
        Serial.print(i); Serial.print(": "); Serial.println(getEEPROM(speedDialAddresses[i], 50));
        yield(); delay(50);
    }
    Serial.println();
}

void waitForSpace() {
    Serial.print("PRESS SPACE");
    char c = 0;
    while (c != 0x20) {
        if (Serial.available() > 0) {
            c = Serial.read();
            if (petTranslate == true){
                if (c > 127) c-= 128;
            }
        }
    }
    Serial.print("\r");
}

void displayHelp() {
    welcome(); yield(); delay(100);
    Serial.println("AT COMMANDS <N=NUMBER>"); yield(); delay(100);
    Serial.println("---------------------------------------"); yield(); delay(100);
    Serial.println("-< DIALING COMMANDS >-"); yield(); delay(100);
    Serial.println("ATDTHOST:PORT.......:DIAL HOST"); yield(); delay(100);
    Serial.println("ATDSN (0-9).........:DIAL STORED HOST"); yield(); delay(100);
    Serial.println("AT&ZN=HOST:PORT.....:STORE HOST (0-9)"); yield(); delay(100);
    //Serial.println("ATDL................:DIAL LAST HOST"); yield(); delay(100);
    Serial.println("ATH.................:HANGUP"); yield(); delay(100);
    Serial.println("+++.................:COMMAND MODE"); yield(); delay(100);
    Serial.println("ATO.................:ONLINE MODE"); yield(); delay(100);
    //ATGET<URL>..........:HTTP GET

    Serial.println();
    Serial.println("-< SERVER COMMANDS >-"); yield(); delay(100);
    Serial.println("ATA.................:ANSWER CALL"); yield(); delay(100);
    Serial.println("AT$SP=N.............:SET SERVER PORT"); yield(); delay(100);
    Serial.println("ATS0/ATS1...........:AUTO ANSWER OFF/ON"); yield(); delay(100);
    Serial.println("AT$BM=MESSAGE.......:SET BUSY MESSAGE"); yield(); delay(100);

    Serial.println();
    Serial.println("-< INFO >-"); yield(); delay(100);
    Serial.println("ATI.................:MODEM INFO"); yield(); delay(100);
    waitForSpace();
    Serial.println();
    Serial.println("-< GENERAL SETTINGS >-"); yield(); delay(100);
    Serial.println("AT$SB=N (300-38400).:SET BAUD RATE"); yield(); delay(100);
    Serial.println("AT&PN (0=CBM,1=NORM):PIN POLARITY"); yield(); delay(100);
    Serial.println("AT&KN (0=NONE,1=HW).:FLOW CONTROL"); yield(); delay(100);              //2=SW
    Serial.println("ATE0/ATE1...........:ECHO OFF/ON"); yield(); delay(100);
    Serial.println("ATV0/ATV1...........:VERBOSE OFF/ON"); yield(); delay(100);
    //ATNETN (0,1)........:TELNET ON/OFF
    //ATPETN (0,1)........:PET TRANSL ON/OFF
    Serial.println("ATZ.................:LOAD SETTINGS"); yield(); delay(100);
    Serial.println("AT&W................:WRITE SETTINGS"); yield(); delay(100);
    Serial.println("AT&V................:VIEW SETINGS"); yield(); delay(100);
    Serial.println("AT&F................:FACTORY DEFAULT"); yield(); delay(100);

    Serial.println();
    Serial.println("-< WIFI >-"); yield(); delay(100);
    Serial.println("AT$C0/AT$C1.........:DIS/CONNECT WIFI"); yield(); delay(100);
    Serial.println("AT$SSID=SSID........:SET WIFI SSID"); yield(); delay(100);
    Serial.println("AT$PASS=PASS........:SET WIFI PASSWORD"); yield(); delay(100);
    Serial.println("AT$SCAN.............:SCAN WIFI SSID"); yield(); delay(100);

    Serial.println();
    Serial.println("-< OTHER >-"); yield; delay(100);
    Serial.println("AT$UPDATE...........:UPDATE FIRMW."); yield(); delay(100);
    Serial.println("AT$RESTART..........:RESTART MODEM"); yield(); delay(100);
    Serial.println("ATCOMMAND?..........:QUERY SETTING"); yield(); delay(100);
}

void storeSpeedDial(byte num, String location) {
    //if (num < 0 || num > 9) { return; }
    speedDials[num] = location;
    //  Serial.print("STORED "); Serial.print(num); Serial.print(": "); Serial.println(location);
}

void welcome() {
    Serial.println();
    Serial.println(firmwareVersion + " BY MY2BITS");
}

void restart() {
    Serial.println("RESTARTING MODEM");
    display.clear();  // approx 20 chars per row
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 24, "Restarting modem");
    display.display();
    ESP.restart();
}

/*------------------------------------------------------------------------- */
/**
   Arduino main init function
*/
void setup() {
    //pinMode(LED_PIN, OUTPUT);
    //digitalWrite(LED_PIN, HIGH); // off
    //pinMode(SWITCH_PIN, INPUT);
    //digitalWrite(SWITCH_PIN, HIGH);
    //pinMode(DCD_PIN, OUTPUT);
    //pinMode(RTS_PIN, OUTPUT);
    //digitalWrite(RTS_PIN, HIGH); // ready to receive data
    //pinMode(CTS_PIN, INPUT);
    //digitalWrite(CTS_PIN, HIGH); // pull up
    setCarrier(false);
    //ESPhttpUpdate.rebootOnUpdate(false);
    EEPROM.begin(LAST_ADDRESS + 1);
    delay(10);
    if (EEPROM.read(VERSION_ADDRESS) != VERSIONA || EEPROM.read(VERSION_ADDRESS + 1) != VERSIONB) {
        defaultEEPROM();
    }
    readSettings();
    // Fetch baud rate from EEPROM
    serialspeed = EEPROM.read(BAUD_ADDRESS);
    // Check if it's out of bounds-- we have to be able to talk
    if (serialspeed < 0 || serialspeed > (sizeof(bauds)/sizeof(long))) {
        serialspeed = 0;
    }
    Serial.begin(bauds[serialspeed]);
    // OLED: Initialising the UI will init the display too.
    display.init();
    //display.flipScreenVertically();
    display.setContrast(129);
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    //display.drawString(64, 0, "C64-WiFiModem v" + String(VERSIONA) + "." + String(VERSIONB) + "." + build);
    display.drawString(64, 0, firmwareVersion);
    display.drawString(64, 12, "by my2bits");
    display.drawString(64, 48, "Press <RETURN> to begin");
    display.display();
    // Wait loop until correct keypress sent to modem
    char c;
    //unsigned long startMillis = millis();
    //while (c != 8 && c != 127 && c!= 20) { // Check for the backspace key to begin
    //while (c != 32) { // Check for space to begin
    while (c != 0x0a && c != 0x0d) { // Check for RETURN to begin
        if (Serial.available() > 0) {
            c = Serial.read();
            if (petTranslate == true){
                if (c > 127) c-= 128;
            }
        }
        if (checkButton() == 1) {
            break; // button pressed, we're setting to 300 baud and moving on
        }
        //if (millis() - startMillis > 2000) {
        //  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        //  startMillis = millis();
        //}
        yield();
    }
    welcome();
    /*
    if (tcpServerPort > 0) tcpServer.begin();
    WiFi.mode(WIFI_STA);
    connectWiFi();
    sendResult(R1_OK);
    //tcpServer(tcpServerPort); // can't start tcpServer inside a function-- must live outside
    digitalWrite(LED_PIN, LOW); // on
    webServer.on("/", handleRoot);
    webServer.on("/ath", handleWebHangUp);
    webServer.begin();
    //mdns.begin("C64-WiFiModem", WiFi.localIP());
    MDNS.begin("C64-WiFiModem");
    */
}

/*------------------------------------------------------------------------- */
String ipToString(IPAddress ip) {
    char s[16];
    sprintf(s, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return s;
}

void hangUp() {
    tcpClient.stop();
    callConnected = false;
    setCarrier(callConnected);
    sendResult(R1_NOCARRIER);
    connectTime = 0;
}

void handleWebHangUp() {
    String t = "NO CARRIER (" + connectTimeString() + ")";
    hangUp();
    webServer.send(200, "text/plain", t);
}

void handleRoot() {
    String page = "WIFI STATUS: ";
    if (WiFi.status() == WL_CONNECTED) {
        page.concat("CONNECTED");
    }
    if (WiFi.status() == WL_IDLE_STATUS) {
        page.concat("OFFLINE");
    }
    if (WiFi.status() == WL_CONNECT_FAILED) {
        page.concat("CONNECT FAILED");
    }
    if (WiFi.status() == WL_NO_SSID_AVAIL) {
        page.concat("SSID UNAVAILABLE");
    }
    if (WiFi.status() == WL_CONNECTION_LOST) {
        page.concat("CONNECTION LOST");
    }
    if (WiFi.status() == WL_DISCONNECTED) {
        page.concat("DISCONNECTED");
    }
    if (WiFi.status() == WL_SCAN_COMPLETED) {
        page.concat("SCAN COMPLETED");
    }
    yield();
    page.concat("\nSSID.......: " + WiFi.SSID());

    byte mac[6];
    WiFi.macAddress(mac);
    page.concat("\nMAC ADDRESS: ");
    page.concat(String(mac[0], HEX));
    page.concat(":");
    page.concat(String(mac[1], HEX));
    page.concat(":");
    page.concat(String(mac[2], HEX));
    page.concat(":");
    page.concat(String(mac[3], HEX));
    page.concat(":");
    page.concat(String(mac[4], HEX));
    page.concat(":");
    page.concat(String(mac[5], HEX));
    yield();

    page.concat("\nIP ADDRESS.: "); page.concat(ipToString(WiFi.localIP()));
    page.concat("\nGATEWAY....: "); page.concat(ipToString(WiFi.gatewayIP()));
    yield();

    page.concat("\nSUBNET MASK: "); page.concat(ipToString(WiFi.subnetMask()));
    yield();
    page.concat("\nSERVER PORT: "); page.concat(tcpServerPort);
    page.concat("\nCALL STATUS: ");
    if (callConnected) {
        page.concat("CONNECTED TO ");
        page.concat(ipToString(tcpClient.remoteIP()));
        page.concat("\nCALL LENGTH: "); page.concat(connectTimeString()); yield();
    } else {
        page.concat("NOT CONNECTED");
    }
    page.concat("\n");
    webServer.send(200, "text/plain", page);
    delay(100);
}

/**
    Turn on the LED and store the time, so the LED will be shortly after turned off
*/
void led_on()
{
    //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    ledTime = millis();
}

void answerCall() {
    tcpClient = tcpServer.available();
    tcpClient.setNoDelay(true); // try to disable naggle
    //tcpServer.stop();
    sendResult(R1_CONNECT);
    connectTime = millis();
    cmdMode = false;
    callConnected = true;
    setCarrier(callConnected);
    Serial.flush();
}

void handleIncomingConnection() {
    if (callConnected == 1 || (autoAnswer == false && ringCount > 3)) {
        // We're in a call already or didn't answer the call after three rings
        // We didn't answer the call. Notify our party we're busy and disconnect
        ringCount = lastRingMs = 0;
        WiFiClient anotherClient = tcpServer.available();
        anotherClient.print(busyMsg);
        anotherClient.print("\r\n");
        anotherClient.print("CURRENT CALL LENGTH: ");
        anotherClient.print(connectTimeString());
        anotherClient.print("\r\n");
        anotherClient.print("\r\n");
        anotherClient.flush();
        anotherClient.stop();
        return;
    }

    if (autoAnswer == false) {
        if (millis() - lastRingMs > 6000 || lastRingMs == 0) {
            lastRingMs = millis();
            sendResult(R1_RING);
            ringCount++;
        }
        return;
    }

    if (autoAnswer == true) {
        WiFiClient tempClient = tcpServer.available(); // this is the key to keeping the connection open
        tcpClient = tempClient; // hand over the new connection to the global client
        tempClient.stop();   // stop the temporary one
        sendString(String("RING ") + ipToString(tcpClient.remoteIP()));
        delay(1000);
        sendResult(R1_CONNECT);
        connectTime = millis();
        cmdMode = false;
        tcpClient.flush();
        callConnected = true;
        setCarrier(callConnected);
    }
}

void dialOut(String upCmd) {
    // Can't place a call while in a call
    if (callConnected) {
        sendResult(R1_ERROR);
        return;
    }
    /*static String lastDialed;
    if (upCmd.indexOf("ATDL") == 0) {
        if (lastDialed != "") {
            upCmd = cmd = lastDialed;
            upCmd.toUpperCase();
        }
        else {
            sendResult(R1_ERROR);
            return;
        }
    }*/
    String host, port;
    int portIndex;
    // Dialing a stored number
    if (upCmd.indexOf("ATDS") == 0) {
        byte speedNum = upCmd.substring(4, 5).toInt();
        portIndex = speedDials[speedNum].indexOf(':');
        if (portIndex != -1) {
            host = speedDials[speedNum].substring(0, portIndex);
            port = speedDials[speedNum].substring(portIndex + 1);
        }
        else {
            port = "23";
        }
    }
    else {
        // Dialing an ad-hoc number
        int portIndex = cmd.indexOf(":");
        if (portIndex != -1){
            host = cmd.substring(4, portIndex);
            port = cmd.substring(portIndex + 1, cmd.length());
        }
        else {
            host = cmd.substring(4, cmd.length());
            port = "23"; // Telnet default
        }
    }
    host.trim(); // remove leading or trailing spaces
    port.trim();
    Serial.print("DIALING "); Serial.print(host); Serial.print(":"); Serial.println(port);
    char *hostChr = new char[host.length() + 1];
    host.toCharArray(hostChr, host.length() + 1);
    int portInt = port.toInt();
    tcpClient.setNoDelay(true); // Try to disable naggle
    if (tcpClient.connect(hostChr, portInt)){
        tcpClient.setNoDelay(true); // Try to disable naggle
        sendResult(R1_CONNECT);
        connectTime = millis();
        cmdMode = false;
        Serial.flush();
        callConnected = true;
        setCarrier(callConnected);
        //if (tcpServerPort > 0) tcpServer.stop();
    }
    else{
        sendResult(R1_NOANSWER);
        callConnected = false;
        setCarrier(callConnected);
    }
    delete hostChr;
    //lastDialed = cmd;
}

/**
    Perform a command given in command mode
*/
void command()
{
    cmd.trim();
    if (cmd == "") return;
    Serial.println();
    String upCmd = cmd;
    upCmd.toUpperCase();

    /**** Just AT ****/
    if (upCmd == "AT") sendResult(R1_OK);

    /**** Dial to host ****/
    else if ((upCmd.indexOf("ATDT") == 0) || (upCmd.indexOf("ATDP") == 0) || (upCmd.indexOf("ATDS") == 0) || (upCmd.indexOf("ATDL") == 0)) // (upCmd.indexOf("ATDI") == 0) 
    {
        dialOut(upCmd);
    }

    /**** Change telnet mode ****/
    else if (upCmd == "ATNET0") {
        telnet = false;
        sendResult(R1_OK);
    }
    else if (upCmd == "ATNET1") {
        telnet = true;
        sendResult(R1_OK);
    }

    else if (upCmd == "ATNET?") {
        Serial.println(String(telnet));
        sendResult(R1_OK);
    }

    /**** Answer to incoming connection ****/
    else if ((upCmd == "ATA") && tcpServer.hasClient()) {
        answerCall();
    }

    /**** Display Help ****/
    else if (upCmd == "AT?") { // || upCmd == "ATHELP") {
        displayHelp();
        sendResult(R1_OK);
    }

    /**** Reset, reload settings from EEPROM ****/
    else if (upCmd == "ATZ") {
        readSettings();
        sendResult(R1_OK);
    }

    /**** Disconnect WiFi ****/
    else if (upCmd == "AT$C0") {
        disconnectWiFi();
        sendResult(R1_OK);
    }

    /**** Connect WiFi ****/
    else if (upCmd == "AT$C1") {
        connectWiFi();
        sendResult(R1_OK);
    }

    /**** Control local echo in command mode ****/
    else if (upCmd.indexOf("ATE") == 0) {
        if (upCmd.substring(3, 4) == "?") {
            sendString(String(echo));
            sendResult(R1_OK);
        }
        else if (upCmd.substring(3, 4) == "0") {
            echo = 0;
            sendResult(R1_OK);
        }
        else if (upCmd.substring(3, 4) == "1") {
            echo = 1;
            sendResult(R1_OK);
        }
        else {
            sendResult(R1_ERROR);
        }
    }

    /**** Control verbosity ****/
    else if (upCmd.indexOf("ATV") == 0) {
        if (upCmd.substring(3, 4) == "?") {
            sendString(String(verboseResults));
            sendResult(R1_OK);
        }
        else if (upCmd.substring(3, 4) == "0") {
            verboseResults = 0;
            sendResult(R1_OK);
        }
        else if (upCmd.substring(3, 4) == "1") {
            verboseResults = 1;
            sendResult(R1_OK);
        }
        else {
            sendResult(R1_ERROR);
        }
    }

    /**** Control pin polarity of CTS, RTS, DCD ****/
    else if (upCmd.indexOf("AT&P") == 0) {
        if (upCmd.substring(4, 5) == "?") {
            sendString(String(pinPolarity));
            sendResult(R1_OK);
        }
        else if (upCmd.substring(4, 5) == "0") {
            pinPolarity = P_CBM;
        //digitalWrite(RTS_PIN, LOW); //TODO: check behaviour and correct the pin startup value
            sendResult(R1_OK);
            setCarrier(callConnected);
        }
        else if (upCmd.substring(4, 5) == "1") {
            pinPolarity = P_NORMAL;
          //  digitalWrite(RTS_PIN, HIGH); //TODO: check behaviour and correct the pin startup value
            sendResult(R1_OK);
            setCarrier(callConnected);
        }
        else {
            sendResult(R1_ERROR);
        }
    }

    /**** Control Flow Control ****/
    else if (upCmd.indexOf("AT&K") == 0) {
        if (upCmd.substring(4, 5) == "?") {
            sendString(String(flowControl));
            sendResult(R1_OK);
        }
        else if (upCmd.substring(4, 5) == "0") {
            flowControl = 0;
            sendResult(R1_OK);
        }
        else if (upCmd.substring(4, 5) == "1") {
            flowControl = 1;
            sendResult(R1_OK);
        }
        else if (upCmd.substring(4, 5) == "2") {
            flowControl = 2;
            sendResult(R1_OK);
        }
        else {
            sendResult(R1_ERROR);
        }
    }

    /**** Set current baud rate ****/
    else if (upCmd.indexOf("AT$SB=") == 0) {
        setBaudRate(upCmd.substring(6).toInt());
    }

    /**** Display current baud rate ****/
    else if (upCmd.indexOf("AT$SB?") == 0) {
        sendString(String(bauds[serialspeed]));;
    }

    /**** Set busy message ****/
    else if (upCmd.indexOf("AT$BM=") == 0) {
        busyMsg = cmd.substring(6);
        sendResult(R1_OK);
    }

    /**** Display busy message ****/
    else if (upCmd.indexOf("AT$BM?") == 0) {
        sendString(busyMsg);
        sendResult(R1_OK);
    }

    /**** Display Network settings ****/
    else if (upCmd == "ATI" || upCmd == "ATI0" || upCmd == "ATI1") {
        displayNetworkStatus();
        sendResult(R1_OK);
    }

    /**** Display profile settings ****/
    else if (upCmd == "AT&V") {
        displayCurrentSettings();
        waitForSpace();
        displayStoredSettings();
        sendResult(R1_OK);
    }

    /**** Save (write) current settings to EEPROM ****/
    else if (upCmd == "AT&W") {
        writeSettings();
        sendResult(R1_OK);
    }

    /**** Set or display a speed dial number ****/
    else if (upCmd.indexOf("AT&Z") == 0) {
        byte speedNum = upCmd.substring(4, 5).toInt();
        if (speedNum >= 0 && speedNum <= 9) {
            if (upCmd.substring(5, 6) == "=") {
                String speedDial = cmd;
                storeSpeedDial(speedNum, speedDial.substring(6));
                sendResult(R1_OK);
            }
            if (upCmd.substring(5, 6) == "?") {
                sendString(speedDials[speedNum]);
                sendResult(R1_OK);
            }
        } else {
            sendResult(R1_ERROR);
        }
    }

    /**** Set WiFi SSID ****/
    else if (upCmd.indexOf("AT$SSID=") == 0) {
        ssid = cmd.substring(8);
        sendResult(R1_OK);
    }

    /**** Display WiFi SSID ****/
    else if (upCmd == "AT$SSID?") {
        sendString(ssid);
        sendResult(R1_OK);
    }

    /**** Set WiFi Password ****/
    else if (upCmd.indexOf("AT$PASS=") == 0) {
        password = cmd.substring(8);
        sendResult(R1_OK);
    }

    /**** Display WiFi Password ****/
    else if (upCmd == "AT$PASS?") {
        sendString(password);
        sendResult(R1_OK);
    }

    /**** Reset EEPROM and current settings to factory defaults ****/
    else if (upCmd == "AT&F") {
        defaultEEPROM();
        readSettings();
        sendResult(R1_OK);
    }

    /**** Set auto answer off ****/
    else if (upCmd == "ATS0=0") {
        autoAnswer = false;
        sendResult(R1_OK);
    }

    /**** Set auto answer on ****/
    else if (upCmd == "ATS0=1") {
        autoAnswer = true;
        sendResult(R1_OK);
    }

    /**** Display auto answer setting ****/
    else if (upCmd == "ATS0?") {
        sendString(String(autoAnswer));
        sendResult(R1_OK);
    }

    /**** Set PET MCTerm Translate On ****/
    else if (upCmd == "ATPET=1") {
        petTranslate = true;
        sendResult(R1_OK);
    }

    /**** Set PET MCTerm Translate Off ****/
    else if (upCmd == "ATPET=0") {
        petTranslate = false;
        sendResult(R1_OK);
    }

    /**** Display PET MCTerm Translate Setting ****/
    else if (upCmd == "ATPET?") {
        sendString(String(petTranslate));
        sendResult(R1_OK);
    }

    /**** Set HEX Translate On ****/
    else if (upCmd == "ATHEX=1") {
        hex = true;
        sendResult(R1_OK);
    }

    /**** Set HEX Translate Off ****/
    else if (upCmd == "ATHEX=0") {
        hex = false;
        sendResult(R1_OK);
    }

    /**** Hang up a call ****/
    else if (upCmd.indexOf("ATH") == 0) {
        hangUp();
    }

    /**** Hang up a call ****/
    else if (upCmd.indexOf("AT$RB") == 0) {
        sendResult(R1_OK);
        Serial.flush();
        delay(500);
        ESP.restart();
    }

    /**** Exit modem command mode, go online ****/
    else if (upCmd == "ATO") {
        if (callConnected == 1) {
            sendResult(R1_CONNECT);
            cmdMode = false;
        } else {
            sendResult(R1_ERROR);
        }
    }

    /**** Set incoming TCP server port ****/
    else if (upCmd.indexOf("AT$SP=") == 0) {
        tcpServerPort = upCmd.substring(6).toInt();
        sendString("CHANGES REQUIRES NV SAVE (AT&W) AND RESTART (AT&RESTART");
        sendResult(R1_OK);
    }

    /**** Display icoming TCP server port ****/
    else if (upCmd == "AT$SP?") {
        sendString(String(tcpServerPort));
        sendResult(R1_OK);
    }

    /**** See my IP address ****/
    else if (upCmd == "ATIP?") {
        Serial.println(WiFi.localIP());
        sendResult(R1_OK);
    }

    /**** HTTP GET request ****/
    else if (upCmd.indexOf("ATGET") == 0) {
        // From the URL, aquire required variables
        // (12 = "ATGEThttp://")
        int portIndex = cmd.indexOf(":", 12); // Index where port number might begin
        int pathIndex = cmd.indexOf("/", 12); // Index first host name and possible port ends and path begins
        int port;
        String path, host;
        if (pathIndex < 0) {
            pathIndex = cmd.length();
        }
        if (portIndex < 0) {
            port = 80;
            portIndex = pathIndex;
        }
        else {
            port = cmd.substring(portIndex + 1, pathIndex).toInt();
        }
        host = cmd.substring(12, portIndex);
        path = cmd.substring(pathIndex, cmd.length());
        if (path == "") path = "/";
        char *hostChr = new char[host.length() + 1];
        host.toCharArray(hostChr, host.length() + 1);

        // Establish connection
        if (!tcpClient.connect(hostChr, port)) {
            sendResult(R1_NOCARRIER);
            connectTime = 0;
            callConnected = false;
            setCarrier(callConnected);
        } 
        else {
            sendResult(R1_CONNECT);
            connectTime = millis();
            cmdMode = false;
            callConnected = true;
            setCarrier(callConnected);

            // Send a HTTP request before continuing the connection as usual
            String request = "GET ";
            request += path;
            request += " HTTP/1.1\r\nHost: ";
            request += host;
            request += "\r\nConnection: close\r\n\r\n";
            tcpClient.print(request);
        }
        delete hostChr;
    } 
    
    /**** Get FW update from Internet ****/
   // else if (upCmd == "AT$UPDATE") {
   //     updateFirmware();
        //ota_flag = true;
        //sendResult(R1_OK);
    //}
    
    /**** Restart modem ****/
    else if (upCmd == "AT$RESTART") {
        sendResult(R1_OK);
        restart();
    }

    /**** Scan WiFi for available SSID ****/
    else if (upCmd == "AT$SCAN") {
        sendResult(R1_OK);
        scanWIFI();
    }
    /**** Unknown command ****/
    else sendResult(R1_ERROR);

    cmd = "";
}

// RTS/CTS protocol is a method of handshaking which uses one wire in each direction to allow each
// device to indicate to the other whether or not it is ready to receive data at any given moment.
// One device sends on RTS and listens on CTS; the other does the reverse. A device should drive
// its handshake-output wire low when it is ready to receive data, and high when it is not. A device
// that wishes to send data should not start sending any bytes while the handshake-input wire is low;
// if it sees the handshake wire go high, it should finish transmitting the current byte and then wait
// for the handshake wire to go low before transmitting any more.
// http://electronics.stackexchange.com/questions/38022/what-is-rts-and-cts-flow-control
void handleFlowControl() {
    if (flowControl == F_NONE) return;
    if (flowControl == F_HARDWARE) {
//        if (digitalRead(CTS_PIN) == pinPolarity) txPaused = true;
//        else txPaused = false;
    }
    if (flowControl == F_SOFTWARE) {
    /*
    Code  Meaning             ASCII Dec Hex Keyboard
    XOFF  Pause transmission  DC3   19  13  Ctrl+S
    XON   Resume transmission DC1   17  11  Ctrl+Q
    */
    }
}

/*void updateFirmware() {
    if(WiFi.status() == WL_CONNECTED){
        //if((WiFiMulti.run() == WL_CONNECTED)) {
        sendResult(R1_OK);
        sendString("LOOKING FOR UPDATE"); yield();
        sendString("PLEASE WAIT");
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 12, "Looking for update");
        display.drawString(64, 36, "Please wait");
        display.display();
        sendString("CURRENT FW VERSION: " + firmwareVersion);
        t_httpUpdate_return ret = ESPhttpUpdate.update("http://example.com/update-c64-wifimodem.php?doupdate=true", firmwareVersion );
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP UPDATE FAILED ERROR (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;
            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("NO UPDATES AVAILABLE");
                break;
            case HTTP_UPDATE_OK:
                Serial.println("UPDATE SUCCESSFUL, PLEASE RESTART MODEM.");
                break;
        }
    }
    else sendResult(R1_ERROR);
}
*/

void refreshDisplay() {
    static String flow;
    static String polarity;
    switch(flowControl) {
        case 0:
            flow="NONE";
            break;
        case 1:
            flow="HW";
            break;
        case 2:
            flow="SW";
    }
    switch(pinPolarity) {
        case 0:
            polarity="CBM";
            break;
        case 1:
            polarity="NORMAL";
            break;
    }

    // clear the display
    display.clear();  // approx 20 chars per row
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, ipToString(WiFi.localIP()));
    yield();
    if (WiFi.status() == WL_CONNECTED) {
        display.drawString(0, 12, "SSID: " + ssid);
    }
    else {
        display.drawString(0, 12, "WiFi not connected");
    }
    yield();
    display.drawString(0, 24, "Baud: " + String(bauds[serialspeed]));
    display.drawString(0, 36, "Flow: " + flow + " Pol: " + polarity);
    if (callConnected == true) {
        display.drawString(0, 48, "Call time: " + connectTimeString());
    }
    else {
        display.drawString(0, 48, "Server port: " + String(tcpServerPort));
    }
    yield();

    // write the buffer to the display
    display.display();
}

void maintainDisplay() {
    static const unsigned long REFRESH_INTERVAL = 500; // ms
    static unsigned long lastRefreshTime = 0;
    if(millis()-lastRefreshTime >= REFRESH_INTERVAL) {
        lastRefreshTime = millis();
        refreshDisplay();
    }
}

void scanWIFI() {
    Serial.println("SCANNING FOR WIFI");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("NO NETWORKS FOUND");
    }
    else {
        Serial.print(n);
        Serial.println(" NETWORKS FOUND");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.println(")");
            //Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
            delay(50);
        }
    }
    Serial.println("");
}

/*------------------------------------------------------------------------- */
/**
    Arduino main loop function
*/
void loop() {
  //refreshDisplay();
    maintainDisplay();

    // Check flow control
    handleFlowControl();

    // Service the Web server
    //webServer.handleClient();

    // Check to see if user is requesting rate change to 300 baud
    //checkButton();

    // New unanswered incoming connection on server listen socket
    if (tcpServer.hasClient()) {
      handleIncomingConnection();
    }

    /**** AT command mode ****/
    if (cmdMode == true) {

        // In command mode - don't exchange with TCP but gather characters to a string
        if (Serial.available()) {
            char chr = Serial.read();

            if (petTranslate == true)
                // Fix PET MCTerm 1.26C Pet->ASCII encoding to actual ASCII
                if (chr > 127) chr-= 128;
            else
                // Convert uppercase PETSCII to lowercase ASCII (C64) in command mode only
                if ((chr >= 193) && (chr <= 218)) chr-= 96;

            // Return, enter, new line, carriage return.. anything goes to end the command
            if ((chr == '\n') || (chr == '\r')) {
                command();
            }
            // Backspace or delete deletes previous character
            else if ((chr == 8) || (chr == 127) || (chr == 20)) {
                cmd.remove(cmd.length() - 1);
                if (echo == true) {
                    Serial.write(chr);
                }
            }
            else {
                if (cmd.length() < MAX_CMD_LENGTH) cmd.concat(chr);
                if (echo == true) {
                    Serial.write(chr);
                }
                if (hex) {
                    Serial.print(chr, HEX);
                }
            }
        }
    }
    /**** Connected mode ****/
    else {
      // Transmit from terminal to TCP
      if (Serial.available())
      {
        led_on();

        // In telnet in worst case we have to escape every byte
        // so leave half of the buffer always free
        int max_buf_size;
        if (telnet == true)
          max_buf_size = TX_BUF_SIZE / 2;
        else
          max_buf_size = TX_BUF_SIZE;

        // Read from serial, the amount available up to
        // maximum size of the buffer
        size_t len = std::min(Serial.available(), max_buf_size);
        Serial.readBytes(&txBuf[0], len);

        // Enter command mode with "+++" sequence
        for (int i = 0; i < (int)len; i++)
        {
          if (txBuf[i] == '+') plusCount++; else plusCount = 0;
          if (plusCount >= 3)
          {
            plusTime = millis();
          }
          if (txBuf[i] != '+')
          {
            plusCount = 0;
          }
        }

        // Double (escape) every 0xff for telnet, shifting the following bytes
        // towards the end of the buffer from that point
        if (telnet == true)
        {
          for (int i = len - 1; i >= 0; i--)
          {
            if (txBuf[i] == 0xff)
            {
              for (int j = TX_BUF_SIZE - 1; j > i; j--)
              {
                txBuf[j] = txBuf[j - 1];
              }
              len++;
            }
          }
        }
        // Fix PET MCTerm 1.26C Pet->ASCII encoding to actual ASCII
        if (petTranslate == true) {
          for (int i = len - 1; i >= 0; i--) {
            if (txBuf[i] > 127) txBuf[i]-= 128;
          }
        }
        // Write the buffer to TCP finally
        tcpClient.write(&txBuf[0], len);
        yield();
      }

      // Transmit from TCP to terminal
      while (tcpClient.available() && txPaused == false)
      {
        led_on();
        uint8_t rxByte = tcpClient.read();

        // Is a telnet control code starting?
        if ((telnet == true) && (rxByte == 0xff))
        {
  #ifdef DEBUG
          Serial.print("<t>");
  #endif
          rxByte = tcpClient.read();
          if (rxByte == 0xff)
          {
            // 2 times 0xff is just an escaped real 0xff
            Serial.write(0xff); Serial.flush();
          }
          else
          {
            // rxByte has now the first byte of the actual non-escaped control code
  #ifdef DEBUG
            Serial.print(rxByte);
            Serial.print(",");
  #endif
            uint8_t cmdByte1 = rxByte;
            rxByte = tcpClient.read();
            uint8_t cmdByte2 = rxByte;
            // rxByte has now the second byte of the actual non-escaped control code
  #ifdef DEBUG
            Serial.print(rxByte); Serial.flush();
  #endif
            // We are asked to do some option, respond we won't
            if (cmdByte1 == DO)
            {
              tcpClient.write((uint8_t)255); tcpClient.write((uint8_t)WONT); tcpClient.write(cmdByte2);
            }
            // Server wants to do any option, allow it
            else if (cmdByte1 == WILL)
            {
              tcpClient.write((uint8_t)255); tcpClient.write((uint8_t)DO); tcpClient.write(cmdByte2);
            }
          }
  #ifdef DEBUG
          Serial.print("</t>");
  #endif
        }
        else
        {
          // Non-control codes pass through freely
          Serial.write(rxByte); yield(); Serial.flush(); yield();
        }
        handleFlowControl();
      }
    }

    // If we have received "+++" as last bytes from serial port and there
    // has been over a second without any more bytes
    if (plusCount >= 3)
    {
      if (millis() - plusTime > 1000)
      {
        //tcpClient.stop();
        cmdMode = true;
        sendResult(R1_OK);
        plusCount = 0;
      }
    }

    // Go to command mode if TCP disconnected and not in command mode
    if ((!tcpClient.connected()) && (cmdMode == false) && callConnected == true)
    {
      cmdMode = true;
      sendResult(R1_NOCARRIER);
      connectTime = 0;
      callConnected = false;
      setCarrier(callConnected);
      //if (tcpServerPort > 0) tcpServer.begin();
    }

    // Turn off tx/rx led if it has been lit long enough to be visible
    //if (millis() - ledTime > LED_TIME) digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // toggle LED state
}

/*------------------------------------------------------------------------- */
