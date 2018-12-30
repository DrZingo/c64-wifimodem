# c64-wifimodem

![modem](resources/modem.png)

## src
Source code for my c64-wifimodem

Feel free to make changes and correction.

## Important
- Please change example.com to your own domain for OTA updates. 
- You will also have to create a backend for OTA to work.

## Stuff that needs work
- Changing pin polarity does not work as expected.
  - V-1541 and commodoreserver.com should work if implemented correct. I had it working, but then normal cbm polarity didn't work as it should.
- Higher baud rates
  - 19200 and 38400 will work but is only used with flipped polarity as I know of.
- Incoming calls
  - I am not enough familiar with bbs servers answering mechanism, and couldn't get it to work with my badly set up bbs server.
  - Direct call connection to the modem does work, sort of.
- S-registers
  - May be useful, I don't know.

## c64-wifimodem
PCB files in kicad format. Nothing much to say about it.

![pcb](resources/pcb.png)

![schematic](resources/drawing.png)

## BOM

```
 C1      22uF Case B, 3528
 J4      User Port edge connector
 Q1-Q5   BSS138 SOT-23
 R1-R12  10k 0805
 U1      LM1117-3.3 SOT-223
 U2      ESP8266 12E or 12F
 U3      OLED 0.96" GND,VCC,SCL,SDA
```
