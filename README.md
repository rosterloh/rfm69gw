## RFM69 Gateway [![Dependency Status](https://dependencyci.com/github/rosterloh/rfm69gw/badge)](https://dependencyci.com/github/rosterloh/rfm69gw)

Thanks to amazing project from [Xose Pérez](https://bitbucket.org/xoseperez/rfm69gw)

## Skip changes to private settings
```bash
$ git update-index --assume-unchanged src/config/general.h
```

**UPDATE**: The RFM69 library works a lot better over ESP8266 changing the SPI clock divider to 2. You will have to modify the source code for RFM69.cpp file with this patch:

```
diff --git a/RFM69.cpp b/RFM69.cpp
index a1e1eeb..ad2e30b 100644
--- a/RFM69.cpp
+++ b/RFM69.cpp
@@ -450,7 +450,7 @@ void RFM69::select() {
   // set RFM69 SPI settings
   SPI.setDataMode(SPI_MODE0);
   SPI.setBitOrder(MSBFIRST);
-  SPI.setClockDivider(SPI_CLOCK_DIV4); // decided to slow down from DIV2 after SPI stalling in some instances, especially visible on mega1284p when RFM69 and FLASH chip both present
+  SPI.setClockDivider(SPI_CLOCK_DIV2); // speeding it up for the ESP8266
   digitalWrite(_slaveSelectPin, LOW);
 }
```

## Building
```bash
> platformio run --target upload -e wire-debug
> platformio run --target uploadfs -e wire-debug
```

Once you have flashed it you can flash it again over-the-air using the ```ota``` environment:

```bash
> platformio run --target upload -e ota-debug
> platformio run --target uploadfs -e ota-debug
```

When using OTA environment it defaults to the IP address of the device in SoftAP mode. If you want to flash it when connected to your home network best way is to supply the IP of the device:

```bash
> platformio run --target upload -e ota-debug--upload-port 192.168.1.151
> platformio run --target uploadfs -e ota-debug --upload-port 192.168.1.151
```
