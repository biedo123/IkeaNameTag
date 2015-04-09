#include <SoftwareSerial.h>
#include <stdio.h>
#include <util/delay_basic.h>

// Bluetooth
#define RxD 6 // Pin to transmit from Bluetooth(BT_TX) to Arduino(RxD)
#define TxD 7 // Pin to receive from Bluetooth(BT_RX) to Arduino(TxD)
#define BT_ONBOARD_LED 13 // On board LED
SoftwareSerial blueToothSerial(RxD, TxD);

// Bluetooth Shield Debug
#define DEBUG_ENABLED 1
#define PIN_CLOCK 9 //RGB LED Clock Pin (Digital 9)
#define PIN_DATA 8 //RGB LED Data Pin (Digital 8)

// Pins
#define PIN_BUTTON 2
#define PIN_LED 4

// Pin States
int ledState = LOW;
int previousButtonState;

//
long time = 0;
long debounce = 200;

void setup()
{
	// Setup Bluetooth Shield
	pinMode(RxD, INPUT);
	pinMode(TxD, OUTPUT);
	pinMode(BT_ONBOARD_LED, OUTPUT);

	// Bluetooth debug
	pinMode(PIN_DATA, OUTPUT);
	pinMode(PIN_CLOCK, OUTPUT);

	// Initialize Bluetooth
	InitializeBluetooth();


	// Set pins
	pinMode(PIN_BUTTON, INPUT);
	pinMode(PIN_LED, OUTPUT);

	// Default pin settings
	digitalWrite(PIN_LED, ledState);

	// Allow Serial communication via USB cable to computer (if required)
	Serial.begin(9600);
}

void loop()
{
	int currentButtonState = digitalRead(PIN_BUTTON); // Read if button is HIGH or LOW

	if (currentButtonState == HIGH && previousButtonState == LOW && millis() - time > debounce) // If button is pressed once
	{
		Serial.print("Button is pressed"); // LOG
		changeLedState(); // Change LED state
		time = millis(); // Set time to check in next loop
	}

	digitalWrite(PIN_LED, ledState); // Apply ledState to LED
	Serial.print("LED set to " + ledState); // LOG

	if (ledState == HIGH) // If LED is on
		sendBluetoothData("employee_busy:true");
	else if (ledState == LOW) // If LED is off
		sendBluetoothData("emplyee_busy:false");
}

void InitializeBluetooth()
{
	blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
	blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
	blueToothSerial.print("\r\n+STNA=NaambordjeJanus\r\n"); //set the bluetooth name as "NaambordjeJanus"
	blueToothSerial.print("\r\n+STPIN=1337\r\n");//Set SLAVE pincode"1337"
	blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
	blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
	delay(2000); // This delay is required.
	blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
	delay(2000); // This delay is required.
	blueToothSerial.flush();
}

void ClkProduce(void)
{
	digitalWrite(PIN_CLOCK, LOW);
	delayMicroseconds(20);
	digitalWrite(PIN_CLOCK, HIGH);
	delayMicroseconds(20);
}

void Send32Zero(void)
{
	unsigned char i;
	for (i = 0; i < 32; i++)
	{
		digitalWrite(PIN_DATA, LOW);
		ClkProduce();
	}
}

void sendBluetoothData(String data)
{
	while (1)
	{
		if(blueToothSerial.available())
		{
			blueToothSerial.print(data);
			Serial.print("Sent data over bluetooth: " + data); // LOG
		}
	}
}

void changeLedState()
{
	// Switch LED state
	if (ledState == HIGH)
		ledState == LOW;
	else if (ledState == LOW)
		ledState == HIGH;

	Serial.print("Changed LED state to " + ledState); // LOG
}