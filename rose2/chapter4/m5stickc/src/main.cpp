#include <M5StickC.h>
#include <sys/time.h>
#include <Adafruit_INA219.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Adafruit_INA219 ina219;

#define BUF_SISE 128
char buf[BUF_SISE];

int64_t time_us = 0;
int64_t last_disp_us = 0;
bool fDisp = false;

void setup(void) 
{
  M5.begin();

  Serial.begin(115200);
  while (!Serial) {
      delay(1);
  }

  SerialBT.begin("M5StickC-BT");
    
  Wire.begin(0, 26);
  if (! ina219.begin(&Wire)) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  struct timeval tm;
  gettimeofday(&tm, NULL);
  
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  // BT Serial
  memset(buf, '¥0', BUF_SISE);
  time_us = (int64_t)tm.tv_sec * 1000000L + (int64_t)tm.tv_usec;
  sprintf(buf, "%12llu, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f", time_us,busvoltage,shuntvoltage,loadvoltage,current_mA,power_mW);
  SerialBT.println(buf);

  // USB Serial output
  if(time_us - last_disp_us >= 1 * 1000000L){
    last_disp_us = time_us;

    // Show on USB Serial console
    Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
    Serial.println("");

    // Show on Display
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.setRotation(1);
    M5.Lcd.print("Bus Voltage:   "); M5.Lcd.print(busvoltage); M5.Lcd.println(" V");
    M5.Lcd.print("Shunt Voltage: "); M5.Lcd.print(shuntvoltage); M5.Lcd.println(" mV");
    M5.Lcd.print("Load Voltage:  "); M5.Lcd.print(loadvoltage); M5.Lcd.println(" V");
    M5.Lcd.print("Current:       "); M5.Lcd.print(current_mA); M5.Lcd.println(" mA");
    M5.Lcd.print("Power:         "); M5.Lcd.print(power_mW); M5.Lcd.println(" mW");
    fDisp = true;
  }

  delay(100);

  if(fDisp){
    M5.update();
    fDisp = false;
  }
}