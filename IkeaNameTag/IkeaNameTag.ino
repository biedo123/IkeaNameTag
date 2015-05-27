#include <SoftwareSerial.h>
#include <stdio.h>
#include <util/delay_basic.h>

// Bluetooth

#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long int

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
#define PIN_LED 13

// Pin States
int ledState = LOW;
int previousButtonState = LOW;

//Timers
long time = 0; //Button
long debounce = 200; //Button
long btTime = 0; //BT
long btDebounce = 1000; //BT

void setup()
{
  Serial.begin(9600);
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

}

void loop()
{
        digitalWrite(BT_ONBOARD_LED,LOW);
	int currentButtonState = digitalRead(PIN_BUTTON); // Read if button is HIGH or LOW
	if (currentButtonState == HIGH && previousButtonState == LOW && millis() - time > debounce) // If button is pressed once
	{
		Serial.print("Button is pressed"); // LOG
		changeLedState(); // Change LED state
		time = millis(); // Set time to check in next loop
	}

	previousButtonState = currentButtonState; // Set current button state as previous for next loop
	digitalWrite(PIN_LED, ledState); // Apply ledState to LED
	char data;

        if(millis() - btTime > btDebounce)
        {
          if (ledState == HIGH)
          { // If LED is on
            Serial.print("true");	
  	    sendBluetoothData("/n employee_busy:true ");
          }
  	  else if (ledState == LOW)
          { // If LED is off
            Serial.print("false");	
  	    sendBluetoothData("/n emplyee_busy:false ");
          }
          
          btTime = millis();
        }

        if(blueToothSerial.available())
        {
           blueToothSerial.read();
        }
}

void InitializeBluetooth()
{
  Serial.print("begin");
  Serial.print(blueToothSerial.available());
	blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
	blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
	blueToothSerial.print("\r\n+STNA=Naambordje\r\n"); //set the bluetooth name as "NaambordjeJanus"
	blueToothSerial.print("\r\n+STPIN=0000\r\n");//Set SLAVE pincode"1337"
	blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
	blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
	delay(2000); // This delay is required.
	blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
	 Serial.println("The slave bluetooth is inquirable!");
        delay(2000); // This delay is required.
	blueToothSerial.flush();
Serial.print(blueToothSerial.available());
Serial.print("End");
}


//BEGIN het in stand houden van blu
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
uint8 TakeAntiCode(uint8 dat){
 uint8 tmp = 0;
 if ((dat & 0x80) == 0){
 tmp |= 0x02; 
 }
 
 if ((dat & 0x40) == 0){
 tmp |= 0x01; 
 }
 
 return tmp;
}
 
// gray data
void DatSend(uint32 dx){
 uint8 i;
 for (i=0; i<32; i++){
 if ((dx & 0x80000000) != 0){
 digitalWrite(PIN_DATA, HIGH);
 } else {
 digitalWrite(PIN_DATA, LOW);
 }
 
 dx <<= 1;
 ClkProduce();
 }
}
 
// data processing
void DataDealWithAndSend(uint8 r, uint8 g, uint8 b){
 uint32 dx = 0;
 
 dx |= (uint32)0x03 << 30; // highest two bits 1，flag bits
 dx |= (uint32)TakeAntiCode(b) << 28;
 dx |= (uint32)TakeAntiCode(g) << 26; 
 dx |= (uint32)TakeAntiCode(r) << 24;
 
 dx |= (uint32)b << 16;
 dx |= (uint32)g << 8;
 dx |= r;
 
 DatSend(dx);
}



void sendBluetoothData(char data[128])
{  
   
   
                Serial.print(" bbb ");
                Serial.print(blueToothSerial.available());
                Serial.print(" AAA  ");
                Serial.print(data);
                blueToothSerial.print("Hello");
		if(blueToothSerial.available())
		{
                         
			  blueToothSerial.println(data);
			  Serial.print("Sent data over bluetooth: "); // LOG
			  Serial.print(data);
                }
    
   
}


void changeLedState()
{
	// Switch LED state
	if (ledState == HIGH)
        {
	        ledState = LOW;
        }
	else if (ledState == LOW)
        {
		ledState = HIGH;
        }
}
