#include "wiring_private.h" // Enable SERCOM stuff

Uart mySerial2 (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM3_Handler() {
  mySerial2.IrqHandler();
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("\n=== Serial Config Fixed (Pin Swap) ===");
  Serial.println("RX defined on Pin 1, TX defined on Pin 0");

  // 1. Init Serial (Standard 8N1)
  mySerial2.begin(57600);
  
  // 2. Pin muxing
  // Pin 1 is RX, Pin 0 is TX
  pinPeripheral(1, PIO_SERCOM); // RX
  pinPeripheral(0, PIO_SERCOM); // TX
  
  Serial.println("Setup done.");
}

// Verify Password Command (Hex): EF 01 ... 13 ...
uint8_t commandPacket[] = {
  0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x07, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B
};

void loop() {
  // --- Clear buffer
  while(mySerial2.available()) mySerial2.read();

  // --- Sending 
  Serial.print("[TX] -> ");
  for (int i = 0; i < sizeof(commandPacket); i++) {
    mySerial2.write(commandPacket[i]);
    Serial.print(commandPacket[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // --- Receiving 
  delay(100); 
  
  if (mySerial2.available()) {
    Serial.print("[RX] <- ");
    while (mySerial2.available()) {
      uint8_t received = mySerial2.read();
      if (received < 0x10) Serial.print("0");
      Serial.print(received, HEX);
      Serial.print(" ");
    }
    Serial.println("\n[Result] Success! We have comms!");
  } else {
    Serial.println("[RX] (No Data)");
    Serial.println("       >>> Check this: Did you actually swap the wires? (Green->1, White->0)");
  }

  Serial.println("------------------------------------------------");
  delay(2000); 
}