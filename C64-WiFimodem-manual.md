
# C64-WiFimodem instructions

This is a WiFi modem for your Commodore 64 and the modern era of internet connectivity.

## Release history
2017-12-01 (online manual update)
* Added info for unencrypted WiFi

2017-09-16 (v0.1.9)
* Small typo correction. Update not necessary

2017-09-09 (online manual update)
* Added 'Hints and Tips' to manual

2017-09-06 (online manual update)
* Added pre-0.1.8 notice to 'Configure WiFi' section in manual

2017-08-30 (v0.1.8 - First release)
* The first available firmware loaded with features.
* Polarity is very experimental at the moment, and your modem will most probably stop responding if you change it. It will not harm your modem nor your computer, but you will have to reboot.
* To have a BBS Server, you will probably need RI-signal, which this modem does not supply. I will have to look further into this. But you can call from one terminal to another, i.e. someone calls you at your server port while you are in a terminal.

## Installation
1. First power off your Commodore. 
2. Plug C64-WiFimodem in the User Port of your C64.
3. Start your C64 and load a terminal program, preferrable [CCGMS 2017 V6], which is the terminal supported by this modem.
4. Now you should see the modem display telling you to press Return.
5. Change your terminal to ASCII mode and 300 baud. 

   Press ```F7``` to enter CCGMS settings.  
   Press ```B``` a couple of times until baud shows '300'.  
   Then press ```RETURN``` to go back to terminal mode.  
   Back in terminal mode you press ```F8``` (Shift+F7) to change to Anscii.
6. Press ```RETURN```, and you should see a welcome message in your C64 terminal.
7. type ```AT?``` and you should see some help.

## Configure WiFi

First type
```
AT$SCAN<RETURN>
```
to see all avaliable WiFi networks

Be aware that some characters show up differently on your Commodore screen. For instance underscore ```'_'``` will show up as left arrow ```'⬅'```. This is also how you have to type it in on your Commodore.

Then type
```
AT$SSID=<Your SSID here as shown in the SCAN listing><RETURN>
AT$PASS=<Your SSID password here><RETURN>
```
If you are connecting to a WiFi without encryption, just put anything in AT$PASS, it can not be empty. E.g. ```AT$PASS=123```

Now type
```
AT$C1<RETURN>
```
pre-0.1.8 use ATC1 instead
```
ATC1<RETURN>
```
and the modem will try to connect to the specified router. It may fail to connect the first time which is normal. Just try again, and if it won't connect, check for typos in your SSID or password.

Now when you have connected, you can save your settings with
```
AT&W<RETURN>
```
The modem will now try to connect to this network every time you start your Commodore and send the first ```RETURN``` to it.

## Extra configuration

If you prefer to have for example 2400 baud at startup, follow these commands:
```
AT$SB=2400<RETURN>
```
This will change the modem to 2400 baud. Now press ```F7``` to go to Terminal settings. 
Change terminal baud-rate with ```B``` to 2400. You may also have to change "Modem Type" to either "User Port 300-2400" or "UP9600" with ```M``` a couple of times. I use UP9600 most of the time.
Now press ```<RETURN>``` to go back to Terminal Mode.
Test with
```
AT<RETURN>
```
You will see "OK". If there was some garbage in the buffer it will give you "ERROR". Just try again.
Now type
```
AT&W
```
This will store your settings to the modem. Then press ```F7``` again and then ```S``` to save your terminal settings.


## Hints and Tips

If you experience graphics tearing in UP9600 mode, you can try using hardware handshake.
In offline mode type:
```
at&k1<RETURN>
```
Or in online mode type:
```
+++ (NO RETURN, wait for 'ok', and you are in cmd mode)
at&k1<RETURN>
ato<RETURN> (going back to online mode)
```
If you experience lockups while changing baud rates, always set UP9600 as modem type first. You can use UP9600 for 300-9600 baud, so there is no need to use other modem types.
Press ```<F7>``` to enter CCGMS settings.
Press ```M``` to set UP9600.
Press ```RETURN``` to go back to terminal mode.

Please have hardware handshaking off, while changing baudrate, and preferrable be in Graphics mode, not Anscii. This will minimize possible modem lockups.
Example:
```
at&k0<RETURN>
<F8 to get to Graphics mode>
at$sb=9600
at&k1<RETURN>
```



## Commands


|    DIALING COMMANDS | Explanation | Example |
|    ---------------- | ----------- | ------- |
|    ATDTHOST:PORT | Dial a host | ```ATDTBBS.RETROHACK.SE:6464``` |
|    ATDSN (0-9) | Dial a stored host | ```ATDS1``` |
|    AT&ZN=HOST:PORT | Store a host and port to speed dials where N is a number from 0 to 9 | ```AT&Z1=BBS.RETROHACK.SE:6464``` |
|    ATH | Hangup from a connected call | ```ATH``` |
|    +++ | Enter command mode from online mode | ```+++``` |
|    ATO | Exit command mode and go to online mode again | ```ATO``` |

|    SERVER COMMANDS | Explanation | Example |
|    --- | --- | --- |
|    ATA | Answer an incoming call manually | ```ATA``` |
|    AT$SP=N | Set the port where callers will reach you | ```AT$SP=6400``` |
|    ATS0/ATS1 | Set auto answer, OFF/ON | ```ATS0``` |
|    AT$BM=MESSAGE | Set the message that callers will see when your server is busy | ```AT$BM=SORRY, THE SERVER IS CURRENTLY BUSY. PLEASE TRY LATER.``` |

|    INFO | Explanation | Example |
|    ---- | ----------- | --- |
|    ATI | Show modem info | ```ATI``` |

|    GENERAL SETTINGS | Explanation | Example |
|    ---------------- | ----------- | ------- |
|    AT$SB=N (300-38400) | Set the modem baud rate | ```AT$SB=2400``` |
|    AT&PN (0=CBM,1=NORM) | Set the pin polarity to commodore or normal | ```AT&P0``` |
|    AT&KN (0=NONE,1=HW) | Set the flow control to none or hardware | ```AT&K0``` |
|    ATE0/ATE1 | Set the modem to echo back to terminal, OFF/ON | ```ATE1``` |
|    ATV0/ATV1 | Set verbose modem responds, OFF/ON | ```ATV1``` |
|    ATZ | Load settings from flash | ```ATZ``` |
|    AT&W | Write current settings to flash | ```AT&W``` |
|    AT&V | View current and stored settings | ```AT&V``` |
|    AT&F | Restore modem to factory defaults | ```AT&F``` |

|    WIFI SETTINGS | Explanation | Example |
|    ------------- | ----------- | ------- |
|    AT$C0/AT$C1 | Disconnect from wifi / Connect to wifi | ```AT$C1``` (old firmware ```ATC1``` and ```ATC0```) |
|    AT$SSID=[SSID] | Set the wifi SSID to connect to. The common underscore is a left arrow on c64. See ```HOME_ROUTER``` in example. | ```AT$SSID=HOME⬅ROUTER``` |
|    AT$PASS=PASS | Set the wifi password of the SSID you are connecting to. Your terminal have to be in ASCII/Anscii mode, and some characters may show differently in your terminal. For instance, underscore '_' will show as left arrow '⬅'. Some characters may not even work. Give it some trial and error, and if it won't work, please change your routers password to less obscure characters. | ```AT$PASS=S3cr3tP4ssw0rd``` | 
|    AT$SCAN | The modem scans for available wifi SSIDs you can connect to. Password enabled networks will have an asterisk (*) suffix. This will show you what characters to enter when setting up with AT$SSID. | ```AT$SCAN``` |

|    OTHER COMMANDS | Explanation | Example |
|    -------------- | ----------- | ------- |
|    AT$UPDATE | Downloads and updates the firmware to the latest version. Updates will sometimes restore the settings to default values. This happens when an update adds fuctionality that needs to reorganize the stored settings. Please restart your modem after update (```AT$RESTART```).  | ```AT$UPDATE``` |
|    AT$RESTART | Restarts the modem and loads the stored settings. Remember to change the terminal settings according to your stored settings afterwards. | ```AT$RESTART``` |
|    ATCOMMAND? | Shows the current settings of specified command. | ```AT&K?```  ```ATE?``` |

## License

All files are licensed under GPLv3


---
©2017 µ2bits, Markus Borgelin


[//]: # (Referencer m.m.)

[CCGMS 2017 V6]: <http://csdb.dk/release/?id=156523>
