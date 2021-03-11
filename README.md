# VibHub-Mini
This is the experimental 18650 version of VibHub with 2 stacked USB ports.

If you're looking for the API documentation, you can find that on the original VibHub client git: 
https://github.com/JasXSL/VibHub-Client/wiki

Official website: https://vibhub.io


# Installing

1. Install Arduino, and the ESP32 libraries. There are good guides on google for this.
2. Clone or download this git project to your sketch folder.
3. Install the following arduino libraries
    1. https://github.com/JasXSL/WiFiManager (Needs to be cloned manually into your library folder, as the official library doesn't support AJAX)
    2. ArduinoJson - Installable via the arduino library manager
    3. https://github.com/Kadah/arduinoWebSockets (Needs to be cloned manually into your library folder)
    4. https://github.com/Kadah/socket.io-client (Also needs to be manually installed)
4. Connect your device with USB.
5. In Arduino, select ESP32 Dev Module as your board.
6. Select the COM port corresponding to your VibHub. If you're not sure which one it is, unplug it and see which one goes way.
7. Partition scheme can be left to default 4MB with SPIFFS.
8. Upload speed can be set to 921600. You can use a lower one if you want, but 921600 is fine for me. Note that you may want to plug it into a USB 3 port though.
9. Leave the rest at default.
10. You should now be able to upload. But please file an issue if you have any problems compiling.
