#include <SoftwareSerial.h>
#include <stdio.h>
#include <util/delay_basic.h>

#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long int
#define RxD 6 // This is the pin that the Bluetooth (BT_TX) will transmit to the Arduino (RxD)
#define TxD 7 // This is the pin that the Bluetooth (BT_RX) will receive from the Arduino (TxD)
	
//ledjes op de bluetooth shield die aangeven of hij verbonden is, zoekt of niks doet. 
#define DEBUG_ENABLED 1
int Clkpin = 9; //RGB LED Clock Pin (Digital 9)
int Datapin = 8; //RGB LED Data Pin (Digital 8)
 
SoftwareSerial blueToothSerial(RxD,TxD);

void setup()
{
	//bluetooth setup
	Serial.begin(9600); //zorgt ervoor dat serial communicatie via usb met pc mogenlijk is.  
	pinMode(RxD, INPUT); // INPUT van bluetooth shield gaat via Digital pin 6 naar arduino.
	pinMode(TxD, OUTPUT); // OUTPUT van de arduino gaat via Ditigal pin 7 naar bluetooth shield
	pinMode(13,OUTPUT); // Zorgt ervoor dat het Ledje op de shield gebruikt kan worden indien nodig (debugging)
	setupBlueToothConnection(); //Initialiseerd de bluetooth shield

	pinMode(Datapin, OUTPUT); // Setup the RGB LED Data Pin
	pinMode(Clkpin, OUTPUT); // Setup the RGB LED Clock pin
	
}
void setupBlueToothConnection()
{
	blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
	blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
	blueToothSerial.print("\r\n+STNA=NametagJanusRinus\r\n"); //set the bluetooth name as "Pluckr"
	blueToothSerial.print("\r\n+STPIN=9151\r\n");//Set SLAVE pincode"8001"
	blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
	blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
	delay(2000); // This delay is required.
	blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
	delay(2000); // This delay is required.
	blueToothSerial.flush();
}


//veranderd led op arduino
void ClkProduce(void){
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
 
uint8 TakeAntiCode(uint8 dat)
{
	uint8 tmp = 0;
	if ((dat & 0x80) == 0)
	{
		tmp |= 0x02; 
	}
 
	if ((dat & 0x40) == 0)
	{
		tmp |= 0x01; 
	}
	return tmp;
}
 
// gray data
void DatSend(uint32 dx)
{
	uint8 i;
	for (i=0; i<32; i++)
	{
		if ((dx & 0x80000000) != 0)
		{
			digitalWrite(Datapin, HIGH);
		} 
		else 
		{
			digitalWrite(Datapin, LOW);
		}
		dx <<= 1;
		ClkProduce();
	}
}
 
// data processing
void DataDealWithAndSend(uint8 r, uint8 g, uint8 b)
{
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

void loop()
{
	digitalWrite(13,LOW); //Turn off the onboard Arduino LED
	char recvChar;
	while(1){
		if(blueToothSerial.available()){//check if there's any data sent from the remote bluetooth shield
			recvChar = blueToothSerial.read();
			Serial.print(recvChar); // Print the character received to the Serial Monitor (if required)
  /* add main program code here */
		}
	}
}
