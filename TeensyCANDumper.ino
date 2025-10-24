#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;
CAN_message_t msg;

// Current baud rates for each bus
uint32_t baudRate1 = 250000;
uint32_t baudRate2 = 250000;
uint32_t baudRate3 = 250000;

bool inSetupMode = false;

void setup(void) {
	Serial.begin(115200);
	while (!Serial && millis() < 3000); // Wait for serial connection
	
	Serial.println("TeensyCANDumper - Press 'S' for setup");
	Serial.println("Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8");
	
	initializeBuses();
}

void initializeBuses() {
	can1.begin();
	can1.setBaudRate(baudRate1);
	can2.begin();
	can2.setBaudRate(baudRate2);
	can3.begin();
	can3.setBaudRate(baudRate3);
}

void printSetupMenu() {
	Serial.println("\n========== SETUP MENU ==========");
	Serial.println("Current Configuration:");
	Serial.print("  CAN1 Baud Rate: ");
	Serial.println(baudRate1);
	Serial.print("  CAN2 Baud Rate: ");
	Serial.println(baudRate2);
	Serial.print("  CAN3 Baud Rate: ");
	Serial.println(baudRate3);
	Serial.println("\nOptions:");
	Serial.println("  1 - Configure CAN1 Baud Rate");
	Serial.println("  2 - Configure CAN2 Baud Rate");
	Serial.println("  3 - Configure CAN3 Baud Rate");
	Serial.println("  X - Exit Setup and Resume Capture");
	Serial.println("================================");
	Serial.print("Enter choice: ");
}

void printBaudRateMenu() {
	Serial.println("\n--- Select Baud Rate ---");
	Serial.println("  A - 125 kbps  (125000)");
	Serial.println("  B - 250 kbps  (250000) [Default]");
	Serial.println("  C - 500 kbps  (500000)");
	Serial.println("  D - 1000 kbps (1000000)");
	Serial.println("  E - 83.3 kbps (83333)");
	Serial.println("  F - 33.3 kbps (33333)");
	Serial.println("  X - Cancel");
	Serial.print("Enter choice: ");
}

uint32_t getBaudRateFromChoice(char choice) {
	switch (toupper(choice)) {
		case 'A': return 125000;
		case 'B': return 250000;
		case 'C': return 500000;
		case 'D': return 1000000;
		case 'E': return 83333;
		case 'F': return 33333;
		default: return 0;
	}
}

void configureBus(int busNum) {
	printBaudRateMenu();
	
	while (!Serial.available()) {
		delay(10);
	}
	
	char choice = Serial.read();
	while (Serial.available()) Serial.read(); // Clear buffer
	
	Serial.println(choice);
	
	if (toupper(choice) == 'X') {
		Serial.println("Cancelled.");
		return;
	}
	
	uint32_t newBaudRate = getBaudRateFromChoice(choice);
	
	if (newBaudRate == 0) {
		Serial.println("Invalid choice!");
		return;
	}
	
	// Update the baud rate and reinitialize the bus
	switch (busNum) {
		case 1:
			baudRate1 = newBaudRate;
			can1.setBaudRate(baudRate1);
			Serial.print("CAN1 baud rate set to ");
			Serial.println(baudRate1);
			break;
		case 2:
			baudRate2 = newBaudRate;
			can2.setBaudRate(baudRate2);
			Serial.print("CAN2 baud rate set to ");
			Serial.println(baudRate2);
			break;
		case 3:
			baudRate3 = newBaudRate;
			can3.setBaudRate(baudRate3);
			Serial.print("CAN3 baud rate set to ");
			Serial.println(baudRate3);
			break;
	}
}

void handleSetupMode() {
	printSetupMenu();
	
	while (inSetupMode) {
		if (Serial.available()) {
			char choice = Serial.read();
			while (Serial.available()) Serial.read(); // Clear buffer
			
			Serial.println(choice);
			
			switch (toupper(choice)) {
				case '1':
					configureBus(1);
					printSetupMenu();
					break;
				case '2':
					configureBus(2);
					printSetupMenu();
					break;
				case '3':
					configureBus(3);
					printSetupMenu();
					break;
				case 'X':
					inSetupMode = false;
					Serial.println("\nExiting setup mode...");
					Serial.println("Resuming capture...");
					Serial.println("Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8");
					break;
				default:
					Serial.println("Invalid choice!");
					printSetupMenu();
					break;
			}
		}
		delay(10);
	}
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
	// Check for setup mode command
	if (Serial.available()) {
		char cmd = Serial.read();
		if (toupper(cmd) == 'S' && !inSetupMode) {
			inSetupMode = true;
			while (Serial.available()) Serial.read(); // Clear buffer
			handleSetupMode();
		}
	}
	
	// Only process CAN messages when not in setup mode
	if (!inSetupMode) {
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
}
