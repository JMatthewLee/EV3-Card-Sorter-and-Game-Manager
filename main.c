 	//Dutch Blitzer - Game Manager and Card Sorter

	//Global Constants
	const int TOTAL_PLAYERS = 4;
	const int COLOR_TOL_INT = 18; 		//Tolerance for Raw RGB Values
	const int MOTOR_TOL_DEGREES = 20; //Tolerance for Colour Indicator Rotation
	const int ROTATION_SPEED = 100; 		//Speed for All Motors
	const int DEBOUNCE_DELAY = 100;		//Time Delay Used after button Presses
	const int ANNOUNCE_DELAY = 2500;	//Time Delay Used between voice lines
	const int ANNOUNCE_DELAY_SHORT = 1000; //Time Delay Used between short lines
	const int DISPENSE_DELAY = 360;		//Time Delay Used for card dispensing
	const int SOUND_FILE_TIME = 7500; //Time Needed for Each sound file
	const int BUTTON_WAIT_MOD = 750;	//Modify time between x button presses
	const int PLAYER_MIN_SPACING = 45;//Min degrees required between players
	const int NO_CARD_LIGHT = 80;			//Colour Sensor Value for no card in holder

	//Configure, Touch Sensor, Colour Sensor, and Gyro
	void configureAllSensors()
	{
		SensorType[S1] = sensorEV3_Touch;
		SensorType[S3] = sensorEV3_Color;
		wait1Msec(50);
		SensorMode[S3] = modeEV3Color_Color;
		wait1Msec(50);
		SensorType[S4] = sensorEV3_Gyro;
		wait1Msec(50);
		SensorMode[S4] = modeEV3Gyro_Calibration;
		wait1Msec(100);
		SensorMode[S4] = modeEV3Gyro_RateAndAngle;
		wait1Msec(50);
	}

/*
	NOTES

	Motors:
		Motor A & D are the turntables
		Motor B is card dispenser
		Motor C is indicator wheel

	Colours Index/IntegerValues:
		Red - 0
		Yellow - 1
		Blue - 2
		Green - 3
*/

//GENERAL USE FUNCTIONS -------------------------------------------------------------------------------------------------------------

	//Author: Matthew Lee
	void getButtonRelease()
	{
		while (SensorValue(S1) != 1)
		{
		}
		playSoundFile("vineboom.rsf");
		while (SensorValue(S1) == 1)
		{
		}
	}

	//Author: Matthew Lee
	void clearAllText()
	{
		displayBigTextLine(0, "         ");
		displayBigTextLine(2, "         ");
		displayBigTextLine(4, "         ");
		displayBigTextLine(6, "         ");
		displayBigTextLine(8, "         ");
		displayBigTextLine(10, "         ");
		displayBigTextLine(12, "         ");
		displayBigTextLine(14, "         ");
	}

	//Author: Iliya Belyatskiy
	void spinTurntable(int speed)
	{
    motor[motorA]=motor[motorD]=speed;
	}

	//Author: Matthew Lee
	int getMultiplePresses(int maxPresses)
	{
		int waitTime = maxPresses * BUTTON_WAIT_MOD;
		getButtonRelease();
		wait1Msec(DEBOUNCE_DELAY); // Debounce Delay - Important for touch sensor
		int pressCount = 1;

		clearTimer(T1);
		while ((time1[T1] < waitTime) && (pressCount < maxPresses))
		{
			while (SensorValue(S1) != 1 && time1[T1] < waitTime)
			{
			}
			while (SensorValue(S1) == 1 && time1[T1] < waitTime)
			{
			}
			if (time1[T1] < waitTime)
			{
				pressCount++;
			}
			wait1Msec(DEBOUNCE_DELAY);
		}
		return pressCount;
	}

	//Author: Emmanuel Nera
	int getLongPress()
	{
		if (SensorValue(S1)==1)
		{
			for (int c=0;c<40;c++)
			{
				if (SensorValue(S1)!=1)
				{
					//returns 1 for short press
					return 1;
				}
				else
				{
					wait1Msec(DEBOUNCE_DELAY/2);
				}
			}
			return 2; //return 2 for long press
		}
		else
		{
			return 0; //return 0 for no press
		}
	}

	//Author: Emmanuel Nera
	//Spin the indicator wheel (red, yellow, blue, green) while it's not
	//at the right colour, stop the wheel and return true when colour is reached
	bool changeIndicatorColour(int colour)
	{
		if (fabs((nMotorEncoder[motorC] % 360) - (colour * 90)) < MOTOR_TOL_DEGREES)
		{
			motor[motorC] = 0;
			return true;
		}
		else
		{
			motor[motorC] = ROTATION_SPEED;
			return false;
		}
	}

	//Author: Matthew Lee
	int returnPlayerPos(int colourInt)
	{
		while (!changeIndicatorColour(colourInt))
		{
		}
		getButtonRelease();
		int angle = getGyroDegrees(S4);
		return (angle % 360);
	}

	//Author: Iliya Belyatskiy & Matthew Lee
	void rotateToPlayer(int angle, int colour)
    {
        int currentAngle = SensorValue[S4];
        bool indicatorDone = false;
        if (angle > currentAngle)
        {
            spinTurntable(-ROTATION_SPEED);// Rotates left or counterclockwise
            while (SensorValue[S4] < angle || !indicatorDone)
            {
                if (!indicatorDone)
                {
                    indicatorDone=changeIndicatorColour(colour);
                }
                //we keep calling changeIndicatorColour until it returns True
                if (SensorValue[S4] >= angle)
                {
                    spinTurntable(0);
                }

            }
            spinTurntable(0);
        }
        if (angle < currentAngle)
        {
            spinTurntable(ROTATION_SPEED); // Rotates right or clockwise
            while (SensorValue[S4] > angle || !indicatorDone)
            {
                if (!indicatorDone)
                {
                    indicatorDone=changeIndicatorColour(colour);
                }
                if (SensorValue[S4] <= angle)
                {
                    spinTurntable(0);
                }
            }
            spinTurntable(0);
        }

    }

	//Author: Donald Alexander
	void dispenseCard()
	{
		motor[motorB] = -100;
		wait1Msec(DISPENSE_DELAY);
		motor[motorB] = 100;
		wait1Msec(DISPENSE_DELAY);
		motor[motorB] = 0;
	}

//GAMESTATE 0 --------------------------------------------------------------------------------------------------------------------------

	//Author: Emmanuel Nera
	//use references to the player positions array,
	//update them to the actual positions of the players
	bool setPositions(int & rAngle, int & yAngle, int & gAngle, int & bAngle)
	{
	    resetGyro(S4);
	    spinTurntable(10);

	    //wait for player red to press button (when card dispenser is
	    //facing player red, then iterate for yellow, blue, and green)
	    rAngle = returnPlayerPos(0);

	    do
	    {
	        yAngle = returnPlayerPos(1);
	 	 	}while (abs(yAngle-rAngle)<PLAYER_MIN_SPACING);
	    // make sure that players can't sit too close to each other

	    do
	    {
	        gAngle = returnPlayerPos(2);
	    }while (abs(gAngle-rAngle)<PLAYER_MIN_SPACING
	    				|| abs(gAngle-yAngle)<PLAYER_MIN_SPACING);

	    do
	    {
	        bAngle = returnPlayerPos(3);
	    }while (abs(bAngle-rAngle)<PLAYER_MIN_SPACING
	    				|| abs(bAngle-yAngle)<PLAYER_MIN_SPACING
	    				|| abs(bAngle-gAngle)<PLAYER_MIN_SPACING);

	    spinTurntable(0);
	    return true;
	}

//Author: Emmanuel Nera
//GAMESTATE 1 (Gameplay) ---------------------------------------------------------------------------------------------------------------


	//Bad Apple (song 0)
	bool playBadApple(int & songIndex)
	{
		int presscheck = 0;
		string stringc = "";
		string name = "badapple.rsf";
		for (int c=1;c<=27;c++){
			stringc = c;
			string filename= stringc + name;
			playSoundFile(filename);
			wait1Msec(SOUND_FILE_TIME); //sound files are 7.5 seconds each
			presscheck = getLongPress();
			if (presscheck==1)
			{
				songIndex = 1; //swap song on short press
				return true;
			}
			else if (presscheck == 2)
			{
				songIndex = -1; //when long press detected, exit song selection
				return true;
			}
		}
		songIndex = 1;
		return true;
	}

	// Kick Back (song 1)
	bool playKickBack(int & songIndex)
	{
		int presscheck = 0;
		string stringc = "";
		string name = "kickback.rsf";
		for (int c=1;c<=27;c++){
			stringc = c;
			string filename= stringc + name;
			playSoundFile(filename);
			wait1Msec(SOUND_FILE_TIME); //sound files are 7.5 seconds each
			presscheck = getLongPress();
			if (presscheck==1)
			{
				songIndex = 2; //swap song on short press
				return true;
			}
			else if (presscheck == 2)
			{
				songIndex = -1; //when long press detected, exit song selection
				return true;
			}
		}
		songIndex = 2;
		return true;
	}

	// Rick (Never gonna give you up - April Fools) (song 2)
	bool playRick(int & songIndex)
	{
		int presscheck = 0;
		string stringc = "";
		string name = "rick.rsf";
		for (int c=1;c<=9;c++){
			stringc = c;
			string filename= stringc + name;
			playSoundFile(filename);
			wait1Msec(SOUND_FILE_TIME); //sound files are 7.5 seconds each
			presscheck = getLongPress();
			if (presscheck == 1)
			{
				songIndex = 0; //swap song on short press
				return true;
			}
			else if (presscheck == 2)
			{
				songIndex = -1; //when long press detected, exit song selection
				return true;
			}
		}
		songIndex = 0;
		return true;
	}

	bool songSelection(){
		int songIndex = 0;
		while (songIndex != -1){
			if (songIndex == 0){
				playBadApple(songIndex);
			}
			else if (songIndex == 1){
				playKickBack(songIndex);
			}
			else{
				playRick(songIndex);
			}
		}
		return true;
	}
//GAMESTATE 2 --------------------------------------------------------------------------------------------------------------------------

	//Author: Matthew Lee
	bool waitForCards()
	{
		displayBigTextLine(0, "Insert Cards");
		displayBigTextLine(3, "Then Hit Button");
		playSoundFile("insertcards.rsf");
		wait1Msec(ANNOUNCE_DELAY);

		getButtonRelease();
		wait1Msec(DEBOUNCE_DELAY);

		displayBigTextLine(0, "1 Press +");
		playSoundFile("pressonce.rsf");
		wait1Msec(ANNOUNCE_DELAY);
		displayBigTextLine(3, "2 Press -");
		playSoundFile("presstwice.rsf");
		wait1Msec(ANNOUNCE_DELAY);

		if (getMultiplePresses(2) == 1)
		{
			displayBigTextLine(0, "");
			displayBigTextLine(3, "ADD");
			playSoundFile("addition.rsf");
			wait1Msec(ANNOUNCE_DELAY);

			return true; //ADD
		}
		else
		{
			displayBigTextLine(0, "");
			displayBigTextLine(3, "SUBTRACT");
			playSoundFile("subtraction.rsf");
			wait1Msec(ANNOUNCE_DELAY);
			return false; //Subtract
		}
	}


//GAMESTATE 3 --------------------------------------------------------------------------------------------------------------------------------


	//Author: Matthew Lee
	//The following is the sorting logic
	//for getting the colour of the current card
	int getCardColour()
	{
		int r = 0, g = 0, b = 0;
		int colourInt = -1;

		//Get the Raw RGB values
		/*
			For some reason this only works when
			the colour sensor is called 3+ times.
			Otherwise the sensor returns values
			outside of 0 to 255 (including negatives).
			I have looked into this many times, and I
			cannot find any source telling me why
			this happens other than a bad colour sensor.
		*/
		getColorRawRGB(S3, r, g, b);
		wait1Msec(DEBOUNCE_DELAY);
		getColorRawRGB(S3, r, g, b);
		wait1Msec(DEBOUNCE_DELAY);
		getColorRawRGB(S3, r, g, b);
		wait1Msec(DEBOUNCE_DELAY);

		//AFTER MUCH TESTING, MEASURMENTS AND TOLERENCING
		//THESE CONDITIONS GIVE THE HIGHEST CONSISTENCY FOR
		//PROPERLY SORTING CARDS BY COLOUR
		if (r<NO_CARD_LIGHT && g<NO_CARD_LIGHT && b<NO_CARD_LIGHT) //No Card
		{
			displayString(0, "No Card");
			return -1;
		}
		else if (r > g) // Red
		{
			displayString(0, "RED");
			colourInt = 0;
			return colourInt;
		}
		else if ((b - r) > COLOR_TOL_INT)//Blue
		{
			displayString(0, "BLUE");
			colourInt = 3;
			return colourInt;
		}
		else if ((g - r) < COLOR_TOL_INT) // Yellow
		{
				displayString(0, "YELLOW");
				colourInt = 1;
				return colourInt;
		}
		else //Green
		{
			displayString(0, "GREEN");
			colourInt = 2;
			return colourInt;
		}
	}


	//Author: Matthew Lee
	//Displays the current score of all players
	void displayScore(int r, int y, int g, int b)
	{
		displayString(2, "Red: %d", r);
		displayString(3, "Yellow: %d", y);
		displayString(4, "Green: %d", g);
		displayString(5, "Blue: %d", b);
	}

	//Author: Matthew Lee
	void updateScore(int& playerScore, bool addScore)
	{
		if (addScore)
		{
			playerScore++;
		}
		else
		{
			playerScore -= 2;
		}
	}

	//Author: Matthew Lee
	//This function Handles the process and flow of Gamestate 3
	void sortAndScore(int& rScore, int& yScore, int& gScore, int& bScore,
									bool isAdding,int rPos, int yPos, int gPos, int bPos)
	{
		int cardColour = 0;
		//Clear All text
		clearAllText();

		while (cardColour != -1)
		{
			cardColour = getCardColour();
			switch (cardColour)
			{
				case(0)://Red
					updateScore(rScore, isAdding);
					rotateToPlayer(rPos,0);
					dispenseCard();
					break;

				case(1)://Yellow
					updateScore(yScore, isAdding);
					rotateToPlayer(yPos,1);
					dispenseCard();
					break;

				case(2)://Green
					updateScore(gScore, isAdding);
					rotateToPlayer(gPos,2);
					dispenseCard();
					break;

				case(3)://Blue
					updateScore(bScore, isAdding);
					rotateToPlayer(bPos,3);
					dispenseCard();
					break;
				}
				displayScore(rScore, yScore, gScore, bScore);
		}
	}

//GAMESTATE 4 --------------------------------------------------------------------------------------------------------------------------------
	//Author: Emmanuel Nera
	void announceScore(int colour, int score)
	{
		string stringScore = "";
		stringScore = score;
		char scorearray[10];
		strcpy(scorearray,stringScore);
		string stringc="";
		string filename = "";
		while (!changeIndicatorColour(colour))
			{
			}
		if(colour == 0)
		{

			playSoundFile("red.rsf");
		}
		else if (colour == 1)
		{
			playSoundFile("yellow.rsf");
		}
		else if (colour == 2)
		{
			playSoundFile("green.rsf");
		}
		else
		{
			playSoundFile("blue.rsf");
		}

		wait1Msec(ANNOUNCE_DELAY_SHORT);
		for(int c=0;c<strlen(stringScore);c++)
		{
			stringc = scorearray[c];
			if (stringc=="-")
			{
				filename = "minus.rsf";
			}
			else
			{
				filename = stringc+".rsf";
			}
			playSoundFile(filename);
			wait1Msec(ANNOUNCE_DELAY);
		}
		wait1Msec(ANNOUNCE_DELAY);
	}

	//Authors: Matthew Lee and Emmanuel Nera
	bool waitForNewRound(int rScore, int yScore, int gScore, int bScore)
	{
		displayBigTextLine(0, "Current Scores:");
		announceScore(0,rScore);
		announceScore(1,yScore);
		announceScore(2,gScore);
		announceScore(3,bScore);

		displayBigTextLine(7, "Hit Button");
		displayBigTextLine(9, "To Continue...");
		playSoundFile("presstocontinue.rsf");
		wait1Msec(ANNOUNCE_DELAY);
		displayBigTextLine(11, "            ");

		getButtonRelease();
		wait1Msec(DEBOUNCE_DELAY);

		displayBigTextLine(0, "1 Press New");
		playSoundFile("anotherround.rsf");
		wait1Msec(ANNOUNCE_DELAY);
		displayBigTextLine(2, "2 Press End");
		playSoundFile("endgame.rsf");
		wait1Msec (ANNOUNCE_DELAY);
		displayBigTextLine(4, "         ");

		if (getMultiplePresses(2) == 1)
		{
			return true; //New Round
		}
		else
		{
			return false; //Subtract
		}
	}

///-----------------------------------------------------------------------------------------------------------------------------------------

	task main()
	{
		configureAllSensors();

		int playerScores[TOTAL_PLAYERS] = { 0,0,0,0 };
		int playerPos[TOTAL_PLAYERS] = { 0, 90, 180, 270 };

		/*
		GAMESTATES
		0 - Boot Up
		1 - Game Running
		2 - Wait for cards
		3 - Sorting/Scoring
		4 - Wait for new round,
		5 - Declare Winner then Shutdown
		*/
		int gameState = 0;

		//Used to get a return from Gamestate 2
		//(If scores are being added to or subtracted)
		bool isAdding = true;

		//Repeat Game loop while gamestates are cycling
		//At gamestate 5 breakout condition will be met
		while (gameState < 6)
		{
			switch (gameState)
			{

				//Boot up - Each Player hits a button to indicate
				//Where they are sitting wait for player to hit button to start
			case(0):
				setPositions(playerPos[0], playerPos[1], playerPos[2], playerPos[3]);

				//Waits until positions are set to start the game
				playSoundFile("pressstart.rsf");
				getButtonRelease();
				wait1Msec(DEBOUNCE_DELAY);

				gameState++;
				break;


				//Game Running - Play Music, Wait for end putton to be pressed
			case(1):

				songSelection();
				gameState++;
				getButtonRelease();
				wait1Msec(DEBOUNCE_DELAY);

				break;


				//Wait for cards - Wait for players to insert mixed cards,
				//indicate if points are being added or subtracted from scores
			case(2):
				isAdding = waitForCards();
				gameState += 1;
				break;


				//Sorting/Scoring - Sort cards using the colour logic,
				//Rotate to the player using that colour
				//Dispense card to player, Tally scores of each player
			case(3):
				sortAndScore(playerScores[0], playerScores[1],
										 playerScores[2], playerScores[3],
									   isAdding, playerPos[0],playerPos[1],
									   playerPos[2], playerPos[3]);

				if (isAdding)
				{
					gameState--;
				}
				else
				{
					gameState++;
				}
				break;


				//Wait for new round - Annouce the player Scores,
				//Ask if players will play a new round or end the game
			case(4):
				if (waitForNewRound(playerScores[0], playerScores[1],
														playerScores[2], playerScores[3]))
				{
					gameState = 1;
				}
				else
				{
					gameState ++;
				}
				break;


				//Declare the winner and break out of the code ending the game
			case(5):
				//Gets the Max Score
				int maxIndex = 0;
				int maxScore = -1000;
				for (int i = 0; i<TOTAL_PLAYERS; i++)
				{
					if (maxScore < playerScores[i])
					{
						maxScore = playerScores[i];
						maxIndex = i;
					}
				}
				while (!changeIndicatorColour(maxIndex))
				{
				}
				switch (maxIndex)
				{
					case(0):
						displayBigTextLine(0,"RED WINS!");
						playSoundFile("red.rsf");
						wait1Msec(ANNOUNCE_DELAY_SHORT);
						displayBigTextLine(2,"CONGRATS!");
						displayBigTextLine(4,"SCORE: %d",playerScores[0]);
						break;

					case(1):
						displayBigTextLine(0,"YELLOW WINS!");
						playSoundFile("yellow.rsf");
						wait1Msec(ANNOUNCE_DELAY_SHORT);
						displayBigTextLine(2,"CONGRATS!");
						displayBigTextLine(4,"SCORE: %d",playerScores[1]);
						break;

					case(2):
						displayBigTextLine(0,"GREEN WINS!");
						playSoundFile("green.rsf");
						wait1Msec(ANNOUNCE_DELAY_SHORT);
						displayBigTextLine(2,"CONGRATS!");
						displayBigTextLine(4,"SCORE: %d",playerScores[2]);
						break;

					case(3):
						displayBigTextLine(0,"BLUE WINS!");
						playSoundFile("blue.rsf");
						wait1Msec(ANNOUNCE_DELAY_SHORT);
						displayBigTextLine(2,"CONGRATS!");
						displayBigTextLine(4,"SCORE: %d",playerScores[3]);
						break;
				}
				playSoundFile("wins.rsf");
				wait1Msec (ANNOUNCE_DELAY_SHORT);
				playSoundFile("victory.rsf");
				wait1Msec(ANNOUNCE_DELAY*2);//Extra long sound

				getButtonRelease();
				wait1Msec(DEBOUNCE_DELAY);

				//Reset the Colour Indicator
				while (!changeIndicatorColour(0))
				{}

				//Sets the gamestate to 6 which breaks
				//out of the while loop and ends the program
				gameState++;

				break;
			}
		}
	}
