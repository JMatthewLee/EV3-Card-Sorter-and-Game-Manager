//Authored by Matthew Lee, Feb 7th

void configureAllSensors() // S3 - Colour, S4 - Gyro
{
	SensorType[S3] = sensorEV3_Color;
	SensorType[S4] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_RGB_Raw;
	SensorMode[S4] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
}

char getCardColour() //Returns 1 - RED, 2 - BLUE, 3 - GREEN, 4 - YELLOW
{
	char colourChar = '\0';
	int r = 0, g = 0, b = 0;

	getColorRawRGB(S3, r, g, b)
	//CAN BE USED FOR TESTING PURPOSES
	displayBigTextLine(1 ,"R:%d",r);
	displayBigTextLine(4,"G:%d",g);
	displayBigTextLine(7,"B:%d",b);

	//GATHERED DATA/IDEAS
	/*
	note: done in a midly dark room with only natural light
	note: needs to be done again with the proper cards
	note: tests done at ~1.5cm awayfrom card looking at the center
	RED
	1.5cm - 115 92 94

	GREEN
	1.5cm - 95 125 102

	BLUE
	1.5cm - 84 117 116

	YELLOW
	1.5cm - 120 121 72

	*/

	if (r > g||r > b)
	{
		if (abs(r-g)>10)
		{
			displayBigTextLine(13,"RED");
		}
		else
		{
			displayBigTextLine(13,"YELLOW");
		}
	}
	else
	{
		if (abs(b-g)>10)
		{
			displayBigTextLine(13,"GREEN");
		}
		else
		{
		displayBigTextLine(13,"BLUE");
		}
	}


	return colourChar;
}

void rotateToPlayer(char colourChar,int motorSpeed)
{
	int nextPos = 0;

	/*Enventually change this code to a hash map to dynamically
	set each players position to where they are at the table*/
	switch (colourChar)
	{
		case('r'):
			nextPos = 0;
			break;
		case('g'):
			nextPos = 90;
			break;
		case('b'):
			nextPos = 180;
			break;
		case('y'):
			nextPos = 270;
			break;
	}
}


task main()
{
	configureAllSensors();
	while(!getButtonPress(buttonAny))
	{getCardColour();}
}
