#include <iostream>

using namespace std;


//LOGIC FOR PLAY SCORE UPDATING------------------------------------------------------------------------------------------------------------
const int TOTAL_PLAYERS = 4;

void displayScore(int playerScore[])
{
	displayString(6, "Red: %d", playerScore[0]);
	displayString(8, "Yellow: %d", playerScore[1]);
	displayString(10, "Green: %d", playerScore[2]);
	displayString(12, "Blue: %d", playerScore[3])
}

void updateScore(int colourInt, int playerScore[], bool addScore)
{
	if (addScore) 
	{
			playerScore[colourInt]++;
	}
	else
	{
		playerScore[colourInt] -= 2;
	}
	displayScore(playerScore);
}

//MAIN ------------ SCORE UPDATING ------------------
int main()
{
	//0 - Boot Up, 1 - Game Running, 2 - Wait for cards, 3 - Sorting/Scoring (), 4 - Wait for new round, 5 - Declare Winner then Shutdown
	int GameState = 0;

	//0 - Red, 1 - Yellow, 2 - Green, 3 - Blue
	int playerScore[TOTAL_PLAYERS] = {};

	while (true) 
	{
		switch (GameState)
		{
			//Boot up - Greet players, Ask for each player to sit in designated Spot, Tell player to hit button to start
			case(0):

			//Game Running - Play Music, Wait for end putton to be pressed
			case(1):

			//Wait for cards - Wait for players to input the mixed cards in holder, indicate if the cards are being added or subtracted from players scores
			case(2):

			//Sorting/Scoring - Sort cards logic, Tally scores and display them as they are changed
			case(3):

			//Wait for new round - Display the Current scores (possibly standings, Prompt if players will play a new round or end the game
			case(4):

			//Declare the winner and break out of the code ending the game
			case(5):
		}
	}
}
