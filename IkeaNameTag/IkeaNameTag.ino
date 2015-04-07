#include <SoftwareSerial.h> //Software Serial Port
#include <stdio.h>
#include <util/delay_basic.h>

//Bluetooth shield v1.1 by seeed studio used for this project
//////SWITCH CODE////////////////////
int inPin = 2;         // the number of the input pin
int outPin = 4;       // the number of the output pin
boolean busy = false;
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

//////////////SWITCH CODE END///////////////

//////////////BLUETOOTH CODE////////////////
#define RxD 6 // This is the pin that the Bluetooth (BT_TX) will transmit to the Arduino (RxD)
#define TxD 7 // This is the pin that the Bluetooth (BT_RX) will receive from the Arduino (TxD)
 
#define DEBUG_ENABLED 1

int Clkpin = 9; //RGB LED Clock Pin (Digital 9)
int Datapin = 8; //RGB LED Data Pin (Digital 8)
 
SoftwareSerial blueToothSerial(RxD,TxD);
///////////BLUETOOTH CODE END////////////////


void setup()
{
	///////SWITCH CODE//////////////
	pinMode(inPin, INPUT);
	pinMode(outPin, OUTPUT);
	////////SWITCH CODE END//////////
	///////BLUETOOTH CODE////////////
	Serial.begin(9600); // Allow Serial communication via USB cable to computer (if required)
	pinMode(RxD, INPUT); // Setup the Arduino to receive INPUT from the bluetooth shield on Digital Pin 6
	pinMode(TxD, OUTPUT); // Setup the Arduino to send data (OUTPUT) to the bluetooth shield on Digital Pin 7
	pinMode(13,OUTPUT); // Use onboard LED if required.
	setupBlueToothConnection(); //Used to initialise the Bluetooth shield
 
	pinMode(Datapin, OUTPUT); // Setup the RGB LED Data Pin
	pinMode(Clkpin, OUTPUT); // Setup the RGB LED Clock pin
}

void loop()
{
	 //////SWITCH CODE//////////////////////////
	reading = digitalRead(inPin);

	// if the input just went from LOW and HIGH and we've waited long enough
	// to ignore any noise on the circuit, toggle the output pin and remember
	// the time
	if (reading == HIGH && previous == LOW && millis() - time > debounce) 
	{
		if (state == HIGH)
		{
			state = LOW;
			busy = false;
		}
		else
		{
			state = HIGH;
			busy = true;
		}
		time = millis();
	}

	digitalWrite(outPin, state);
	previous = reading;
	/////////SWITCH CODE END///////////////////////////
	/////////BLUETOOTH CODE////////////////////////////
	digitalWrite(13,LOW); //Turn off the onboard Arduino LED
//	char recvChar;
	while(1)
	{
		if(blueToothSerial.available())
		{
			//check if there's any data sent from the remote bluetooth shield
			busy = blueToothSerial.read();
			Serial.print(busy);
			if (busy == false)
			{
				blueToothSerial.print("false");
			}
			else if (busy == true)
			{
				blueToothSerial.print("true");
			}
		}
		if(Serial.available())
		{
			busy = Serial.read();
			//This will send value obtained (recvChar) to the phone. The value will be displayed on the phone.
			blueToothSerial.print(busy);
		}
	}
}
////nodig voor setup van bluetooth shield
void setupBlueToothConnection()
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
//veranderd led op arduino
void ClkProduce(void)
{
	digitalWrite(Clkpin, LOW);
	delayMicroseconds(20); 
	digitalWrite(Clkpin, HIGH);
	delayMicroseconds(20); 
}
 
void Send32Zero(void)
{
	unsigned char i;
	for (i=0; i<32; i++)
	{
		digitalWrite(Datapin, LOW);
		ClkProduce();
	}
}