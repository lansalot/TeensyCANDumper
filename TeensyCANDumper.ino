#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;
CAN_message_t msg;

void setup(void) {
	Serial.println("Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8");
	can1.begin();
	can1.setBaudRate(250000);
	can2.begin();
	can2.setBaudRate(250000);
	can3.begin();
	can3.setBaudRate(250000);
}

void printFormattedMessage(int bus) {
	/*  CANBUS log format
		Time Stamp, ID, Extended, Dir, Bus, LEN, D1, D2, D3, D4, D5, D6, D7, D8
		10646301, 18FEE8F7, true, Rx, 0, 8, 00, 00, 00, 00, FF, FF, FF, FF,
	*/
	Serial.print(micros());
	Serial.print(",");
	Serial.print(msg.id, HEX);
	Serial.print(",");
	if (msg.flags.extended) {
		Serial.print("TRUE");
	}
	else {
		Serial.print("FALSE");
	}
	Serial.print(",Rx,");
	Serial.print(msg.bus);
	Serial.print(",");
	Serial.print(msg.len, HEX);
	for (uint8_t i = 0; i < msg.len; i++) {
		Serial.print(",");
		Serial.print(msg.buf[i], HEX);
	}
	Serial.println();
}

void loop() {
	if (can1.read(msg)) {
		printFormattedMessage(1);
	}
	if (can2.read(msg)) {
		printFormattedMessage(2);
	}
	if (can3.read(msg)) {
		printFormattedMessage(3);
	}
}
