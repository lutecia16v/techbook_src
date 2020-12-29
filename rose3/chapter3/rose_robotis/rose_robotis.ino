#include<stdint.h>

// CRC-16-IBM
uint16_t CRC_calc(uint8_t *data, int length)
{
  uint16_t crc16;
  int i,j;
  
  crc16 = 0x0000; // 初期値=0
  for(i=0;i<length;i++){
    crc16 ^= ( ((uint16_t)data[i]) << 8);
    for(j=0;j<8;j++){
      if(crc16 & 0x8000){
        crc16 = (crc16 << 1) ^ 0x8005; // 生成多項式
      }else{
        crc16 <<= 1;
      }
    }
  }
  return crc16;
}

bool sendpos(uint8_t id, uint16_t pos) {
  // インストラクションパケット
  uint8_t tx_buf[] = { // 多バイトの数値はリトルエンディアン
    0xFF, 0xFF, 0xFD, 0x00, // Header(固定値)
    id,         // ID
    0x09, 0x00, // Length
    0x03,       // Instruction (0x03 = WRITE)
    0x74, 0,      // Address (0x74 = GoalPosition)
    (uint8_t)(pos & 0xFF), (uint8_t)(pos >> 8), 0x00, 0x00, // Data
    0, 0        // CRC (下で計算)
  };
  Serial.print(tx_buf[10],HEX);
  Serial.print(",");
  Serial.print(tx_buf[11],HEX);
  // CRC計算
  uint16_t crc = CRC_calc(tx_buf, 14);
  tx_buf[14] = (uint8_t)(crc & 0xFF);
  tx_buf[15] = (uint8_t)(crc >> 8);
  // 送信
  //while (Serial7.available ()) Serial7.read (); // バッファクリア
  Serial7.write (tx_buf, 16);
  
  // ステータスパケット受信
  uint8_t rx_buf[11];
  if (Serial7.readBytes (rx_buf, 11) == 11){
    if(rx_buf[4] != id){ // IDの一致をチェック
     Serial.println("ID not matched!");
     return false;
    }
    if(rx_buf[8] != 0){ // エラーをチェック
     Serial.print("Status Error = "); Serial.println(rx_buf[8]);
     return false;
    }
    Serial.println("OK!");
    return true;
  }else{
    Serial.println("Bad response!");
    return false;
  }
}

bool sendTorqueOn(uint8_t id) {
  // インストラクションパケット
  uint8_t tx_buf[] = { // 多バイトの数値はリトルエンディアン
    0xFF, 0xFF, 0xFD, 0x00, // Header(固定値)
    id,         // ID
    0x07, 0x00, // Length
    0x03,       // Instruction (0x03 = WRITE)
    64, 0,      // Address (64 = Torque Enable)
    1, 0x00,    // Data
    0, 0        // CRC (下で計算)
  };
  // CRC計算
  uint16_t crc = CRC_calc(tx_buf, 12);
  tx_buf[12] = (uint8_t)(crc & 0xFF);
  tx_buf[13] = (uint8_t)(crc >> 8);
  // 送信
  while (Serial7.available ()) Serial7.read (); // バッファクリア
  Serial7.write (tx_buf, 14);
  
  // ステータスパケット受信
  uint8_t rx_buf[11];
  if (Serial7.readBytes (rx_buf, 11) == 11){
    if(rx_buf[4] != id){ // IDの一致をチェック
     Serial.println("ID not matched!");
     return false;
    }
    if(rx_buf[8] != 0){ // エラーをチェック
     Serial.print("Status Error = "); Serial.println(rx_buf[8]);
     return false;
    }
    Serial.println("OK!");
    return true;
  }else{
    Serial.println("Bad response!");
    return false;
  }
}

void setup() {
  Serial7.begin(57600); // 1Mbps
  Serial7.setTimeout(50);
  
  Serial.begin(115200);
  
  delay(1000);
  sendTorqueOn(1);
  sendTorqueOn(2);
  sendTorqueOn(3);
}
 
void loop() {
  for(int id = 1; id <= 3; id++){
    sendpos (id, 0);
    delay (1000);
    sendpos (id, 1023);
    delay (1000);
  }
}
