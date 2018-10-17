//#include <prismino.h>
#include <Wire.h>

uint8_t outbuf[6];
int cnt = 0;
int joy_x_axis, joy_y_axis, dirY, dirX, speedLeft, speedRight;
//uint16_t accX, accY, accZ;
//uint8_t button_c, button_z;

int LED = 13;

uint8_t zone;
uint8_t previousZone;
uint8_t zoneCount;
uint8_t zoneHistory[256];

long composite;

  bool cButton;  
  bool zButton;  
  bool mousePressed;

void setup()
{
  Serial.begin(9600);

  //Serial.println("Inside setup()");
  //delay(1000);

  pinMode(LED, OUTPUT);
  Wire.begin();
  Serial.println("After Wire.begin()");

  nunchuckInit();
  Serial.println("After nunchuckInit()");

  //  play(440, 300);
  //  play(880, 300);
  Keyboard.begin();
  Mouse.begin();
  mousePressed = 0;

}

void nunchuckInit()
{
  Serial.println("Inside nunchuckInit()");
  // transmit to device 0x52
  Wire.beginTransmission(0x52);

  // sends memory address
//  Wire.write(0x40);
  Wire.write(0xf0);

  // sends sent a zero
  Wire.write(0x55);

  //delay(1000);

  // sends memory address
  Wire.write(0xfb);

  // sends sent a zero
  Wire.write(0x00);

  // sends another  zero
  Wire.write(0x00);

  // stop transmitting
  Wire.endTransmission();
}

void sendZero()
{
  // transmit to device 0x52
  Wire.beginTransmission(0x52);
  // sends one byte
  Wire.write(0x00);
  // stop transmitting
  Wire.endTransmission();
}

void showZoneHistory()
{
  uint8_t index = 0;

  while (index < zoneCount)
  {
   // Serial.print("   ");
    //Serial.print(zoneHistory[index]);
    index++;
  }
  //Serial.println("");



}

void loop()
{

  uint8_t data;
  uint8_t xValue, yValue;
	char msg[256];

  int xMove;
  int yMove;

  int xCoord, yCoord;

  long radius;
  long cosAngle;


  // request data from nunchuck
  Wire.requestFrom(0x52, 6);


  while(Wire.available())
  {
    // receive byte as an integer
    data = Wire.read();
    outbuf[cnt] = data;
    //digitalWrite(LED, HIGH);
    cnt++;
  }

  xValue = outbuf[0];
  yValue = outbuf[1];
  cButton = outbuf[5] & 0x2;
  zButton = outbuf[5] & 0x1;

 xCoord = xValue - 127;
 yCoord = yValue - 127;

  radius = xCoord * xCoord + yCoord * yCoord;
  cosAngle = ((long) xCoord * (long) xCoord * (long) 100) / radius;

  if (radius < 400)
  {
	zone = 0;
  }

  else if (cosAngle > 50)
  {
    if (xCoord > 0)
      {
        zone = 1;
      } 	
    else
    {
      zone = 3;
    }
  }

  else 
  {
    if (yCoord > 0)
      {
        zone = 2;
      } 	
    else
    {
      zone = 4;
    }
  }

/*
  Serial.print("   ");
  Serial.print(zone);
  Serial.print("   ");
  Serial.print(cosAngle);
  Serial.print("   ");
  Serial.print(radius);
  Serial.print("   ");
  Serial.print(xCoord);
  Serial.print("   ");
  Serial.print(yCoord);
  Serial.print("   ");
  Serial.print(xValue);
  Serial.print("   ");
  Serial.print(yValue);
  Serial.print("   ");
  Serial.print(cButton);
  Serial.print("   ");
  Serial.print(zButton);
  Serial.print("   ");
  Serial.print("\r");
*/


  cnt = 0;
  // send the request for next bytes

if (!cButton)
  {

	xMove = 0;
	if (xCoord < -20) xMove = -1;
	if (xCoord > 20) xMove = 1;

	yMove = 0;
	if (yCoord < -20) yMove = 1;
	if (yCoord > 20) yMove = -1;

  /*Serial.print(" cButton is pressed - ");
  Serial.print(xCoord);
  Serial.print(" - ");
  Serial.print(yCoord);
  Serial.print(" - ");
  Serial.print(xMove);
  Serial.print(" - ");
  Serial.print(yMove);
  Serial.print("\r");*/

  if ((xMove != 0) || (yMove != 0))
  {
  	Mouse.move(xMove, yMove);
  }

  if ((!zButton) && (!Mouse.isPressed()))
  {
	  Mouse.press();
  }
  if (zButton)
  {
	  Mouse.release();

  }

  
  }



else if ((zone != previousZone) && (zone == zoneHistory[zoneCount -2]) && (zoneCount > 1))
{
  //Serial.print("New zone. Back to previous zone\n");
	//Serial.println("Debouncing");
    //Serial.print("Previous Previous zone   ");
    //Serial.print(zoneHistory[zoneCount - 2]);
    //Serial.print(" ");
	zoneCount--;
}
  else if (zone != previousZone)
  {


  //Serial.print("New zone\n");
    //Serial.print("Previous zone   ");
    //Serial.print(previousZone);

    //Serial.print("New zone   ");
    //Serial.println(zone);



    if (zone != 0)
    {

	zoneHistory[zoneCount++] = zone;
    }
    else
    {
	//Serial.println("Back to zone 0");

	composite =	zoneHistory[0];
	if (zoneCount > 1)	composite +=  16 * zoneHistory[1] ; 
	if (zoneCount > 2)	composite +=  256 * zoneHistory[2] ; 
	if (zoneCount > 3)	composite +=  4096 * zoneHistory[3] ; 
	if (zoneCount > 4)	composite +=  16L * 4096L  * zoneHistory[4] ; 
	if (zoneCount > 5)	composite +=  256L * 4096L  * zoneHistory[5] ; 
	//showZoneHistory();
	//sprintf( msg, "Before calling parseSequence zoneCount is %x composite is %lx ", zoneCount, (long) composite);	
	//Serial.println(msg);
 	parseSequence();
        zoneCount = 0;
    }

    previousZone = zone;

  }



  sendZero();
  delay(10);
}




void parseSequence()
{
	char msg[256];
	int index = 0;
	char inKey;

	//Serial.println("Inside parseSequence");

	if (zoneCount > 5)

	{
		Serial.println("Not supported yet");
	}

	sprintf( msg, "Inside parseSequence zoneCount is %x composite is %lx ", zoneCount, (long) composite);	
	//Serial.println(msg);

	index = strlen(msg);
	msg[0] = 0;
	
	while (index > 47) 
	{
		sprintf(msg, "%s %c ", msg, msg[index--]);
		
	}
	//Serial.println(msg);

	if (zButton)
	{
	switch (composite)
	{
		// Cross 1
		case 0x0001:			inKey = ' ';	break;
		case 0x0002:			inKey = ' ';	break;
		case 0x0003:			inKey = '\b';	break;
		case 0x0004:			inKey = '\n';	break;

		case 0x0021:			inKey = 'a';	break;
		case 0x0032:			inKey = 's';	break;
		case 0x0043:			inKey = 'i';	break;
		case 0x0014:			inKey = 'o';	break;
	
		case 0x0041:			inKey = 'n';	break;
		case 0x0034:			inKey = 'e';	break;
		case 0x0023:			inKey = 't';	break;
		case 0x0012:			inKey = 'y';	break;

		// Cross 2
		case 0x0321:			inKey = 'r';	break;
		case 0x0432:			inKey = 'd';	break;
		case 0x0143:			inKey = 'h';	break;
		case 0x0214:			inKey = 'u';	break;

		case 0x0341:			inKey = 'm';	break;
		case 0x0234:			inKey = 'l';	break;
		case 0x0123:			inKey = 'c';	break;
		case 0x0412:			inKey = 'b';	break;

		// Cross 3
		case 0x4321:			inKey = 'x';	break;
		case 0x3214:			inKey = 'v';	break;
		case 0x2143:			inKey = 'j';	break;
		case 0x1432:			inKey = 'g';	break;

		case 0x2341:			inKey = 'f';	break;
		case 0x1234:			inKey = 'k';	break;
		case 0x4123:			inKey = 'z';	break;
		case 0x3412:			inKey = 'p';	break;

		// Cross 4
		case 0x23412:			inKey = 'q';	break;
		case 0x43214:			inKey = 'w';	break;
	}
	}
	else
	{
	switch (composite)
	{
		// Cross 1
		case 0x0001:			inKey = ' ';	break;
		case 0x0002:			inKey = ' ';	break;
		case 0x0003:			inKey = '\b';	break;
		case 0x0004:			inKey = '\n';	break;

		case 0x0021:			inKey = '2';	break;
		case 0x0032:			inKey = '8';	break;
		case 0x0043:			inKey = '6';	break;
		case 0x0014:			inKey = '4';	break;
	
		case 0x0041:			inKey = '3';	break;
		case 0x0034:			inKey = '5';	break;
		case 0x0023:			inKey = '7';	break;
		case 0x0012:			inKey = '1';	break;

		// Cross 2
		case 0x0321:			inKey = 'r';	break;
		case 0x0432:			inKey = 'd';	break;
		case 0x0143:			inKey = 'h';	break;
		case 0x0214:			inKey = 'u';	break;

		case 0x0341:			inKey = 'm';	break;
		case 0x0234:			inKey = 'l';	break;
		case 0x0123:			inKey = 'c';	break;
		case 0x0412:			inKey = 'b';	break;

		// Cross 3
		case 0x4321:			inKey = 'x';	break;
		case 0x3214:			inKey = 'v';	break;
		case 0x2143:			inKey = 'j';	break;
		case 0x1432:			inKey = 'g';	break;

		case 0x2341:			inKey = 'f';	break;
		case 0x1234:			inKey = 'k';	break;
		case 0x4123:			inKey = 'z';	break;
		case 0x3412:			inKey = 'p';	break;

		// Cross 4
		case 0x23412:			inKey = 'q';	break;
		case 0x43214:			inKey = 'w';	break;
	}
	}
//	Serial.println("");	
	//Serial.print("inKey ");	
//	Serial.println(inKey);	
	Keyboard.write(inKey);	
}

 
