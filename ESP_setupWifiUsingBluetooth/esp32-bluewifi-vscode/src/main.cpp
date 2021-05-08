#include "weatherInfor.h"
#include "bluetoothAndWifiModule.h"


void setup()
{
  Serial.begin(115200);
  Serial.println("Booting...");

  setupWifi();
  
}

void loop()
{
  wifiScan();

  if(wifi_stage == CONNECTED)
      // TEST : ....
      // call API of the open weather to get the infor
      Serial.println(getWeather());

      delay(2000);
}
