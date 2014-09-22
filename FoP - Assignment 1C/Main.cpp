//---------------------------------------------------------------------------
//Program: Assignment 1c
//Author: Scott Spittle, Josh Fellingham-Boyce and Damon Reynolds
//Last updated: 22 Mar. 14
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	//for output and input: cout <<, cin >>
#include <iomanip> //for output manipulators
#include <conio.h> //for getch()
#include <cassert> //for assert
#include <fstream> //for file streams
#include <string> //for strings
#include <vector> //for vectors
#include <sstream> //for string streams

using namespace std;

//include our own libraries
#include "ConsoleUtils.h" //Extra calss for prefilled functions.
#include "RandomUtils.h" //Extra calss for prefilled functions.
#include "TimeUtils.h" //Extra calss for prefilled functions.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------
//defining the size of the grid
const int SIZEY (11); //vertical dimension
const int SIZEX (19); //horizontal dimension
const int TIMER_MAX (90);
const int HIGH_SCORES_TO_KEEP(6);

const int MAX_CHEESE (6); //Maximum number of cheese to place in the game

//defining symbols used for display of the grid & content
const char MOUSE ('@'); //Mouse symbol
const char TUNNEL (' '); //tunnel symbol
const char WALL ('#'); //wall symbol
const char APPLE ('O'); //apple symbol
const char STORE ('+');	//store symbol
const char CHEESE ('*'); //store symbol

//defining the command letters to move the blob on the maze
const int UP (72); //up arrow
const int DOWN (80); //down arrow
const int RIGHT (77); //right arrow
const int LEFT (75); //left arrow

//defining the other command letters
const char QUIT ('Q'); //end the game
const char SAVE ('S'); //save game
const char LOAD ('L'); //load game
const char CHEAT ('C'); //Cheat symbol
const char UNDO('U'); //Cheat symbol
const char BEST_SCORES('B'); //BestScore Symbol

//HighScorers Structure
struct HighScorer {
	int highScore;
	string name;
};

//Mouse Data Structure
struct Item {
	int x, y, highscore, moves, score, applesStored, cheeseEaten;
	string name;
	bool cheated;
	const char symbol;
};

//Apple Data Structure
struct Apple {
	int x, y;
	bool stored;
	char symbol;
};

//cheese Data Structure
struct Cheese {
	int x, y;
	bool eaten;
	char symbol;
};
//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------
int main()
{
	//function declarations (prototypes)
	void initialiseGame(char grid[][SIZEX+1], char maze[][SIZEX+1], Item& mouse, int level, string& message, vector<Apple>& apples, vector<Cheese>& cheese);
	void paintGame(const char grid[][SIZEX + 1], string& message, Item mouse, int level, double secondsElapsed = 0);
	bool isArrowKey(int key);
	int getKeyPress();
	void moveMouse(const char grid[][SIZEX+1], Item& mouse, int key, string& message, vector<Apple>& apples, vector<Cheese>& cheese);
	void updateGrid(char grid[][SIZEX+1], const char maze[][SIZEX+1], Item mouse, vector<Apple>& apples, vector<Cheese>& cheese);
	void endProgram();

	//Our function declerations. (prototypes)
	void updateGameStats(Item& Mouse, const vector<Apple>& apples, vector<HighScorer>& highScorers, int level);
	bool saveGame(const char grid[][SIZEX+1], const Item mouse, string& message, bool tracker = false);
	bool loadGame(char grid[][SIZEX + 1], Item& mouse, string& message, vector<Apple>& apples, vector<Cheese>& cheese, bool undo = false);
	void gameCompleted(char grid[][SIZEX+1], char maze[][SIZEX+1], Item& mouse, int& level, string& message, vector<Apple>& apples, vector<Cheese>& cheese, vector<HighScorer>& highScorers);
	void processCheat(Item& mouse, string& message, vector<Apple>& apples, int level, vector<Cheese>& cheese);
	void checkForSave(Item Mouse, string& message, bool timedGame = false);
	void DisplayHighScores(vector<HighScorer> highScorers);

	//local variable declarations 
	char grid[SIZEY+1][SIZEX+1]; //grid for display
	char maze[SIZEY+1][SIZEX+1]; //structure of the maze
	Item mouse = {(SIZEX/2) + 2, (SIZEY/2) + 1, 500, 0, 0, 0, 0, "scott", false, MOUSE}; //mouse's position and symbol
	vector<Apple> apples; //Vector to hold tha Apples
	vector<Cheese> cheese; //vector to hold the Cheese
	vector<HighScorer> highScorers; // Vector to hold the HighScorers
	string message("LET'S START..."); //current message to player
	int level(1), keyLast; //default level
	bool timedGame = false; //To determine if it is a timed game
	char timerOption; //Users choice on game start.

	//Locals for Timed Game option
	time_t timer;
	struct tm gameStartTime;
	double secondsElapsed(0);

	// Get the user's name
	cout << "Please enter your name: ";
	cin >> mouse.name;

	// Get the level the user wants to play
	cout << "Select Level 1, 2, 3: ";
	cin >> level;

	// Does the user want a Timed Game?
	cout << "Would you like to plat a timed game? You will have 90 seconds to complete it.";
	cin >> timerOption;

	//Setting Timed Game Option
	if (toupper(timerOption) == 'Y')
	{
		timedGame = true;
	}

	Clrscr(); //clear screen

	initialiseGame(grid, maze, mouse, level, message, apples, cheese); //initialise grid (incl. walls & mouse)
	updateGameStats(mouse, apples, highScorers, level); //updates stats(moves made, apples stored.)
	paintGame(grid, message, mouse, level); //display game info, modified grid & messages
	saveGame(grid, mouse, message, true); //Save the game for an initial undo.

	int key(getKeyPress());	//read in  selected key: arrow or letter command

	time(&timer);  /* get current time; same as: timer = time(NULL)  */
	gameStartTime = *localtime(&timer); //Tracking the time elapsed

	while (key != QUIT) //while user does not want to quit
	{
		if (timedGame)
		{
			time(&timer);  /* get current time; same as: timer = time(NULL)  */
			secondsElapsed = difftime(timer, mktime(&gameStartTime));

			if (secondsElapsed >= TIMER_MAX)
			{
				message = "Game Over! Timer Expired";
				endProgram();
			}
		}
		
		if (isArrowKey(key))
		{
			saveGame(grid, mouse, message, true); //Save the game for undo
			moveMouse(grid, mouse, key, message, apples, cheese); //move mouse in that direction
			updateGrid(grid, maze, mouse, apples, cheese); //update grid information
		}
		else if (toupper(key) == BEST_SCORES)
		{
			DisplayHighScores(highScorers); //Display HighScores
			while (!getch()) //Resume Game on key press
			{

			}
		}
		else if (toupper(key) == SAVE)
		{
			if (!mouse.cheated && !timedGame)//So long as the user has not cheated and it's not a timed game, Save
				saveGame(grid, mouse, message);
			else
				message = "Cannot save if you've cheated or it's a timed game.";
		}
		else if (toupper(key) == LOAD)
		{
			if (!mouse.cheated && !timedGame)//So long as the user has not cheated and it's not a timed game, Save
			{
				if (loadGame(grid, mouse, message, apples, cheese))//Ensure it loads successfully before updating the grid
				{
					moveMouse(grid, mouse, key, message, apples, cheese); //move mouse in that direction
					updateGrid(grid, maze, mouse, apples, cheese); //update grid information
				}
			}
			else
				message = "Cannot load if you've cheated or it's a timed game.";
		}
		else if (toupper(key) == UNDO)
		{
			if (!mouse.cheated && !timedGame)//Undo the last move if allowed.
			{
				if (keyLast != UNDO)
				{
					loadGame(grid, mouse, message, apples, cheese, true); //Recycling the load procedure for the undo
					moveMouse(grid, mouse, key, message, apples, cheese); //move mouse in that direction
					updateGrid(grid, maze, mouse, apples, cheese); //update grid information
				}
				else
					message = "No!!";
			}
			else
				message = "You cheated, you can't undo.";
		}
		else if (toupper(key) == CHEAT)
		{
			processCheat(mouse, message, apples, level, cheese); //Call Process Cheat
			moveMouse(grid, mouse, key, message, apples, cheese); //move mouse in that direction
			updateGrid(grid, maze, mouse, apples, cheese); //update grid information
		}

		checkForSave(mouse, message, timedGame); //Checks for save if there is not timed game
		updateGameStats(mouse, apples, highScorers, level); //updates stats(moves made, apples stored.)
		paintGame(grid, message, mouse, level, secondsElapsed); //display game info, modified grid & messages

		if (mouse.applesStored == (int)apples.size() && mouse.cheeseEaten == (int)cheese.size()) //Check if the game has been won
			gameCompleted(grid, maze, mouse, level, message, apples, cheese, highScorers);
		
		keyLast = key;
		key = getKeyPress(); //display menu & read in next option
	}

	endProgram(); //display final message
	return 0;
}

//Checking if a save already exsists for the player
void checkForSave(Item mouse, string& message, bool timedGame = false)
{
	ifstream loadFileStream;
	loadFileStream.open(mouse.name+".sav", ios::in); //Does a save file exsist

	if (!loadFileStream.fail())
		if (!mouse.cheated && !timedGame)
			message = "Save found, press L to load.";

	loadFileStream.close();
}

//---------------------------------------------------------------------------
//----- Cheat Procesure
//---------------------------------------------------------------------------
void processCheat(Item& mouse, string& message, vector<Apple>& apples, int level, vector<Cheese>& cheese)
{
	int appleCounter(0); // Keeping track of how many apples are not stored
	int applesEffected(0); // Track how many apples have been effected
	int lastApple;// Which appple is the last to be moved. int index of apples[]
	
	cout << '\a' << ' ' << '\a' <<  ' ' << '\a'; //beep the alarm
	mouse.cheated = true;
	mouse.moves = 1000;
	mouse.score = 1000;
	message = "You Cheated!";

	//counting how many apples are unstored
	for (int i = 0; i < (int)apples.size(); i++)
		if (!apples[i].stored)
			appleCounter++;

	//Setting all cheese to eaten
	for (int i = 0; i < (int)cheese.size(); i++)
	{
		cheese[i].eaten = true;
		mouse.cheeseEaten = (int)cheese.size();
	}

	//Move all the apples into the store
	while (appleCounter > 1)
	{
		for (int k = applesEffected; k < (int)apples.size(); k++)
			if (!apples[k].stored)
			{
				apples[k].stored = true;
				appleCounter--;
				applesEffected++;
				break;
			}
	}

	//get the last apple left.
	for (int i = (int)apples.size() - 1; i >= 0; i--)
		if (!apples[i].stored)
		{
			lastApple = i;
			break;
		}

	//depending on the level, place the last apple at the store.
	switch (level)
	{
	case 1:
		apples[lastApple].x = 16;
		apples[lastApple].y = 8;
		mouse.x = 15;
		mouse.y = 8;
		break;
	case 2:
		apples[lastApple].x = 4;
		apples[lastApple].y = 5;
		mouse.x = 5;
		mouse.y = 5;
		break;
	case 3:
		apples[lastApple].x = 16;
		apples[lastApple].y = 8;
		mouse.x = 15;
		mouse.y = 8;
		break;
	}
}

//---------------------------------------------------------------------------
//----- Game Complete
//---------------------------------------------------------------------------
void gameCompleted(char grid[][SIZEX + 1], char maze[][SIZEX + 1], Item& mouse, int& level, string& message, vector<Apple>& apples, vector<Cheese>& cheese, vector<HighScorer>& highScorers)
{
	void initialiseGame(char grid[][SIZEX+1], char maze[][SIZEX+1], Item& mouse, int level, string& message, vector<Apple>& apples, vector<Cheese>& cheese);
	void paintGame(const char grid[][SIZEX + 1], string& message, Item mouse, int level, double secondsElapsed = 0);
	void updateGameStats(Item& Mouse, const vector<Apple>& apples, vector<HighScorer>& highScorers, int level);
	void restartGame(Item& mouse, vector<Apple>& apples, string& message, vector<Cheese>& cheese);
	void setHighScoreFinal(Item& mouse, vector<HighScorer>& highScorers, int level);
	void endProgram();
	
	SelectTextColour(clYellow);
	Gotoxy(50, 16);

	//As long as it's not the last level.
	if (level < 3)
	{
		if (mouse.cheated) //If they cheated.
			cout << "\nWell done! Level: " << level << " complete, but you cheated! \nNext Level? y/n ";
		else
			cout << "\nWell done! Level: " << level << "complete \nNext Level? y/n ";

		if (getch() == 'y') //Set the nwe level
			level += 1;
		else
		{
			setHighScoreFinal(mouse, highScorers, level);// Setting final highscorers.
			exit(0);
		}
	}
	else //Game Complete
	{ 
		cout << "Game Complete. Press a key to close.";
		getch();
		exit(0);
	}
	
	//Restart the game.
	restartGame(mouse, apples, message, cheese);

	SelectTextColour(clWhite);

	initialiseGame(grid, maze, mouse, level, message, apples, cheese); //initialise grid (incl. walls & mouse)
	updateGameStats(mouse, apples, vector<HighScorer>(), level); //updates stats(moves made, apples stored.)
	setHighScoreFinal(mouse, highScorers, level-1);
	paintGame(grid, message, mouse, level); //display game info, modified grid & messages
}

//reset variable/object values
void restartGame(Item& mouse, vector<Apple>& apples, string& message, vector<Cheese>& cheese)
{
	apples.clear(); //clear all the apples
	cheese.clear();

	//reset mouse.
	mouse.x = 0;
	mouse.y = 0;
	mouse.moves = 0;
	mouse.highscore = 500;
	mouse.cheated = false;
	mouse.cheeseEaten = 0;

	message = ""; //reset message.
}
//---------------------------------------------------------------------------
//----- Update Game Stats
//---------------------------------------------------------------------------
void updateGameStats(Item& mouse, const vector<Apple>& apples, vector<HighScorer>& highScorers, int level)
{
	//count how many apples are marked as stores.
	void endProgram();
	void getHighScore(Item& mouse, vector<HighScorer>& highScorers, int level);
	void setHighScore(Item& mouse);

	int appleCounter(0); //track how many apples are stored

	//count how many apples are stored.
	for (int i = 0; i < (int)apples.size(); i++)
	{
		if (apples[i].stored)
			appleCounter++;
	}

	//get the players new highscore
	getHighScore(mouse, highScorers, level);

	if (!mouse.cheated)
		setHighScore(mouse); //Set players highscore if not cheated

	mouse.applesStored = appleCounter; //set the number of store apples to how many that have stored attribute set to true.
}

//getting highscore from the players .sav file
void getHighScore(Item& mouse, vector<HighScorer>& highScorers, int level)
{
	string highScore; // String object of highscore for casting
	int loadhighScore; // for players individual highscore
	ifstream loadFileStream; //load file stream
	
	loadFileStream.open(mouse.name+".txt", ios::in);
	if (loadFileStream.good())
	{
		getline(loadFileStream, highScore); // get moves
		istringstream(highScore) >> loadhighScore;

		mouse.highscore = loadhighScore > 500 ? loadhighScore: 500; //set the players highscore to 500 if more than 500
	}
	else
		mouse.highscore = 500;//set the players highscore if one isn't found

	loadFileStream.close(); //Close the file

	loadFileStream.open("BestScores.txt", ios::in); //Load bestScores.

	if (!loadFileStream.good())
	{
		//If failed to open, create a template BestScores.
		ofstream saveFileStream;

		saveFileStream.open("BestScores.txt", ios::out);
		saveFileStream.clear();

		if (saveFileStream.good())
		{
			for (int i = 0; i < HIGH_SCORES_TO_KEEP; i++)
			{
				highScorers.push_back(HighScorer());
				saveFileStream << "Anonymous " << endl;
				saveFileStream << "500" << endl;
				highScorers[i].name += "Anonymous";
				highScorers[i].highScore = 500;
			}
		}

		saveFileStream.close();
	}
	else //If BestScores is found. populate the vector
	{
		for (int i = 0; i < HIGH_SCORES_TO_KEEP; i++)
		{
			if ((int)highScorers.size() < HIGH_SCORES_TO_KEEP)
			{
				highScorers.push_back(HighScorer()); //add a new HighScorer object
			}
			getline(loadFileStream, highScore);
			istringstream(highScore) >> highScorers[i].name; //csating string to int via string stream.
			getline(loadFileStream, highScore);
			istringstream(highScore) >> highScorers[i].highScore;
		}
	}

	loadFileStream.close();
}

//Setting highscore to external save
void setHighScore(Item& mouse)
{
	ofstream saveFileStream;
	
	saveFileStream.open(mouse.name+".txt", ios::out);
	saveFileStream.clear();
	if (saveFileStream.good())
		saveFileStream << mouse.highscore;

	saveFileStream.close();
}
//Setting highscore to external save
void setHighScoreFinal(Item& mouse, vector<HighScorer>& highScorers, int level)
{
	ofstream saveFileStream;

	saveFileStream.open("BestScores.txt", ios::out);
	saveFileStream.clear();

	//Checking best scores and replacing the first one it finds that the player has beaten with the players highscore.
	if (saveFileStream.good())
	{
		bool indicesAffected = false;
		for (int i = 0; i < HIGH_SCORES_TO_KEEP; i++)
		{
			if (mouse.score < highScorers[i].highScore && !indicesAffected)
			{
				saveFileStream << mouse.name << endl;
				saveFileStream << mouse.score << endl;
				indicesAffected = true;
			}
			else
			{
				saveFileStream << highScorers[i].name << endl;
				saveFileStream << highScorers[i].highScore << endl;
			}
		}
	}
	saveFileStream.close();
}
//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------
void initialiseGame(char grid[][SIZEX+1], char maze[][SIZEX+1], Item& mouse, int level, string& message, vector<Apple>& apples, vector<Cheese>& cheese)
{
	//initialise grid & place mouse in middle
	void setInitialMazeStructure(char g[][SIZEX+1], int level, string& message, vector<Apple>& apples, vector<Cheese>& cheese);
	void setInitialMouseCoordinates(Item& mouse, int level);
	void updateGrid(char g[][SIZEX+1], const char m[][SIZEX+1], Item mouse, vector<Apple>& apples, vector<Cheese>& cheese);

	setInitialMazeStructure(maze, level, message, apples, cheese); //initialise maze
	setInitialMouseCoordinates(mouse, level);	//initialise mouse's position
	updateGrid(grid, maze, mouse, apples, cheese); //prepare grid
}

//set the position of the walls in the maze
void setInitialMazeStructure(char maze[][SIZEX+1], int level, string& message, vector<Apple>& apples, vector<Cheese>& cheese)
{ 
	//initialise maze configuration, local array to store the maze structure
	// with '#' for wall, ' ' for tunnel and 'X' for unused part of array
	ifstream loadFileStream;
	int row(1), appleCount(0), tunnelCount(0);
	char item;

	switch (level)
	{
	case 3:
		{
			loadFileStream.open("maze3.maz", ios::in);		//load maze3 file
			if (loadFileStream.fail())						//if it fails to open the .maz file
				message = "Failed to initialize maze 3.";
			break;
		}
	case 2:
		{
			loadFileStream.open("maze2.maz", ios::in);
			if (loadFileStream.fail())
				message = "Failed to initialize maze 2.";
			break;
		}
	default:
		{
			loadFileStream.open("maze1.maz", ios::in);
			if (loadFileStream.fail())
				message = "Failed to initialize maze 1.";
			break;
		}
	}

	//While there are rows to insert into the maze grid. initialize each apple they find and build maze[][]
	while (row < SIZEY+1)
	{
		for (int i = 0; i < SIZEX+1; i++)
			if (loadFileStream.good())
			{
				item = loadFileStream.get();
				if (item == APPLE)
				{
					apples.push_back(Apple());
					apples[appleCount].x = i;
					apples[appleCount].y = row;
					maze[row][i] = TUNNEL;
					appleCount++;
				}
				else
					maze[row][i] = item;
			}
			row++;
	}
	loadFileStream.close(); //close the filestream as we are done loading.
	
	//Randomly placing the cheese
	for (int i = 0; i < MAX_CHEESE; i++)
	{
		bool CheesePlaced = false; //has that cheese been places
		bool CheeseAlreadyThere = false; //is there already cheese there
		int x, y; //checking location

		do
		{
			CheeseAlreadyThere = false; //reset
			x = Random(SIZEX); //random coords
			y = Random(SIZEY);

			//if its all clear place a cheese
			if (maze[y][x] == TUNNEL)
			{
				for (int j = 0; j < cheese.size(); j++)
					if (cheese[j].x == x && cheese[j].y == y)
					{
						CheeseAlreadyThere = true;
						break;
					}
				for (int k = 0; k < apples.size(); k++)
					if (apples[k].x == x && apples[k].y == y)
					{
						CheeseAlreadyThere = true;
						break;
					}
				if (!CheeseAlreadyThere)
				{
					cheese.push_back(Cheese());
					cheese[i].x = x;
					cheese[i].y = y;
					CheesePlaced = true;
				}
			}
		} while (!CheesePlaced);
	}
}

//calculate mouse's coordinates at beginning of game
void setInitialMouseCoordinates(Item& mouse, int level)
{
	switch (level)
	{
	case 2:
		mouse.x = 7;
		mouse.y = 5;
		break;
	default:
		mouse.x = 15;
		mouse.y = 9;
		break;
	}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------
void updateGrid(char grid[][SIZEX+1], const char maze[][SIZEX+1], Item mouse, vector<Apple>& apples, vector<Cheese>& cheese)
{
	//update grid configuration after each move
	void setMaze(char grid[][SIZEX+1], const char maze[][SIZEX+1]);
	void placeMouse(char grid[][SIZEX+1], Item mouse);
	void placeApples(char grid[][SIZEX+1], vector<Apple>& apples);
	void placeCheese(char grid[][SIZEX+1], vector<Cheese>& cheese);

	setMaze(grid, maze); //reset the empty maze configuration into grid
	placeCheese(grid, cheese);
	placeApples(grid, apples);
	placeMouse(grid, mouse); //set mouse in grid
}

//reset the empty/fixed maze configuration into grid
void setMaze(char grid[][SIZEX+1], const char maze[][SIZEX+1])
{
	for (int row(1); row <= SIZEY; ++row) //for each row (vertically)
		for (int col(1); col <= SIZEX; ++col) //for each column (horizontally)
			grid[row][col] = maze[row][col];
}

//place mouse at its new position in grid
void placeMouse(char grid[][SIZEX+1], Item mouse)
{
	grid[mouse.y][mouse.x] = '@';
}

//place apples at there new positions in the grid
void placeApples(char grid[][SIZEX+1], vector<Apple>& apples)
{
	for (int i = 0; i < (int)apples.size(); i++) //for each apple in the vector
		if (!apples[i].stored) //if the apple has not been stored
			grid[apples[i].y][apples[i].x] = APPLE; //draw apple
}

//place cheese at there new positions in the grid
void placeCheese(char grid[][SIZEX+1], vector<Cheese>& cheese)
{
	for (int i = 0; i < (int)cheese.size(); i++) //for each apple in the vector
		if (!cheese[i].eaten) //if the apple has not been stored
			grid[cheese[i].y][cheese[i].x] = CHEESE; //draw apple
}

//---------------------------------------------------------------------------
//----- move mouse in required direction
//---------------------------------------------------------------------------
void moveMouse(const char grid[][SIZEX+1], Item& mouse, int key, string& message, vector<Apple>& apples, vector<Cheese>& cheese)
{
	void setKeyDirection(int key, int& dirX, int& dirY); //calculate direction of movement required by key - if any
	int dirX(0), dirY(0);

	setKeyDirection(key, dirX, dirY); //find direction indicated by key

	//check new target position in grid & update mouse coordinates if move is possible
	//depending on what's on the target position in grid...
	switch(grid[mouse.y+dirY][mouse.x+dirX])
	{
	case TUNNEL: //can move
		mouse.y += dirY; //go in that Y direction
		mouse.x += dirX; //go in that X direction
		mouse.score++;
		mouse.moves++;
		break;
	case CHEESE: //can move
		mouse.y += dirY; //go in that Y direction
		mouse.x += dirX; //go in that X direction
		for (int i = 0; i < (int)cheese.size(); i++) //for each apple in the vector
		{
			if (cheese[i].x == mouse.x && cheese[i].y == mouse.y) //if it is the cheese you hit
			{
				cheese[i].eaten = true;
				mouse.cheeseEaten++;
				mouse.score++;
				mouse.moves++;
			}
		}
		break;
	case APPLE: //can move
		if (dirY == 1)
		{
			if (grid[mouse.y + 1][mouse.x] == APPLE) //if the mouse hit an apple
			{
				if (grid[mouse.y + 2][mouse.x] == TUNNEL || grid[mouse.y + 2][mouse.x] == STORE || grid[mouse.y + 2][mouse.x] == CHEESE) //if there is nothing after that apple they hit
				{
					for (int i = 0; i < (int)apples.size(); i++) //for each apple in the vector
					{
						if (apples[i].x == mouse.x && apples[i].y == mouse.y + 1) //if it is the apple you hit
						{
							apples[i].y++;
							mouse.y += dirY;
							mouse.moves++;
							mouse.score++;
							if (grid[mouse.y + 2][mouse.x] == STORE)
								apples[i].stored = true;
							break;
						}
					}
				}
			}
		}
		else if (dirY == -1)
		{
			if (grid[mouse.y - 1][mouse.x] == APPLE)
			{
				if (grid[mouse.y - 2][mouse.x] == TUNNEL || grid[mouse.y - 2][mouse.x] == STORE || grid[mouse.y - 2][mouse.x] == CHEESE)
				{
					for (int i = 0; i < (int)apples.size(); i++)
					{
						if (apples[i].x == mouse.x && apples[i].y == mouse.y - 1)
						{
							apples[i].y--;
							mouse.y += dirY;
							mouse.moves++;
							mouse.score++;
							if (grid[mouse.y - 2][mouse.x] == STORE)
								apples[i].stored = true;
							break;
						}
					}
				}
			}
		}
		else if (dirX == 1)
		{
			if (grid[mouse.y][mouse.x + 1] == APPLE)
			{
				if (grid[mouse.y][mouse.x + 2] == TUNNEL || grid[mouse.y][mouse.x + 2] == STORE ||grid[mouse.y][mouse.x + 2] == CHEESE)
				{
					for (int i = 0; i < (int)apples.size(); i++)
					{
						if (apples[i].x == mouse.x + 1 && apples[i].y == mouse.y)
						{
							apples[i].x++;
							mouse.x += dirX;
							mouse.moves++;
							mouse.score++;
							if (grid[mouse.y][mouse.x + 1] == STORE)
								apples[i].stored = true;
							break;
						}
					}
				}
			}
		}
		else if (dirX == -1)
		{
			if (grid[mouse.y][mouse.x - 1] == APPLE)
			{
				if (grid[mouse.y][mouse.x - 2] == TUNNEL || grid[mouse.y][mouse.x - 2] == STORE || grid[mouse.y][mouse.x - 2] == CHEESE)
				{
					for (int i = 0; i < (int)apples.size(); i++)
					{
						if (apples[i].x == mouse.x - 1 && apples[i].y == mouse.y)
						{
							apples[i].x--;
							mouse.x += dirX;
							mouse.moves++;
							mouse.score++;
							if (grid[mouse.y][mouse.x - 1] == STORE)
								apples[i].stored = true;
							break;
						}
					}
				}
			}
		}
		break;
	case WALL: //hit a wall & stay there
		cout << '\a'; //beep the alarm
		if (mouse.x == SIZEX-1 && dirX == 1)
			if (grid[mouse.y][2] == TUNNEL) //if the opposite wall is not blocked
			{
				mouse.x = 2; //set mouse location
				mouse.moves++; //increment moves
				mouse.score++;
			}
			else
				message = "Something is blocking your path";
		else if (mouse.y == SIZEY-1 && dirY == 1)
			if (grid[2][mouse.x] == TUNNEL)
			{
				mouse.y = 2;
				mouse.moves++;
				mouse.score++;
			}
			else
				message = "Something is blocking your path";
		else if (mouse.x == 2 && dirX == -1)
			if (grid[mouse.y][SIZEX-1] == TUNNEL)
			{
				mouse.x = SIZEX-1;
				mouse.moves++;
				mouse.score++;
			}
			else
				message = "Something is blocking your path";
		else if (mouse.y == 2 && dirY == -1)
			if (grid[SIZEY-1][mouse.x] == TUNNEL)
			{
				mouse.y = SIZEY-1;
				mouse.moves++;
				mouse.score++;
			}
			else
				message = "Something is blocking your path";
		break;
	}
}

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(int key, int& dirX, int& dirY)
{ 
	//depending on the selected key...
	switch(key)
	{
	case LEFT: //when LEFT arrow pressed...
		dirX = -1; //decrease the X coordinate
		dirY = 0;
		break;
	case RIGHT: //when RIGHT arrow pressed...
		dirX = +1; //increase the X coordinate
		dirY = 0;
		break;
	case UP: //when UP arrow pressed...
		dirX = 0;
		dirY = -1; //decrease the Y coordinate
		break;
	case DOWN: //when DOWN arrow pressed...
		dirX = 0;
		dirY = +1; //increase the Y coordinate
		break;
	}
}

//get key or command selected by user
int getKeyPress()
{
	int keyPressed;
	keyPressed = getch(); //read in the selected arrow key or command letter
	while (keyPressed == 224) //ignore symbol following cursor key
		keyPressed = getch();

	return(toupper(keyPressed)); //return it in uppercase 
}

//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
bool isArrowKey(int key)
{	
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------
void DisplayHighScores(vector<HighScorer> highScorers) {
	int highScorerCount = 0;
	//Outputting all the highscores onto the screen.
	Gotoxy(30, 2);
	cout << "_High Scorers_";

	for (int i = 3; i < 3 + (int)highScorers.size(); i++)
	{
		switch (i)
		{
		case 3:
			SelectTextColour(clGreen);
			break;
		case 4:
			SelectTextColour(clBlue);
			break;
		case 5:
			SelectTextColour(clRed);
			break;
		case 6:
			SelectTextColour(clCyan);
			break;
		case 7:
			SelectTextColour(clDarkYellow);
			break;
		default:
			break;
		}
		Gotoxy(30, i);
		cout << highScorers[highScorerCount].name << " : " << highScorers[highScorerCount].highScore;
		highScorerCount++;
	}
}

void paintGame(const char grid[][SIZEX + 1], string& message, Item mouse, int level, double secondsElapsed = 0)
{ 
	//display game title, messages, maze, mouse & apples on screen
	void paintGrid(const char grid[][SIZEX+1]);

	Clrscr();

	//display game title
	SelectTextColour(clYellow);
	Gotoxy(0, 0);
	cout << "___MOUSE AND APPLES GAME___\n" << endl;
	SelectTextColour(clWhite);
	Gotoxy(50, 0);
	cout << "Group 1V - Scott, Josh, Damon";

	// display grid contents
	paintGrid(grid);
	
	Gotoxy(50, 1);
	cout << GetDate() << " " << GetTime();
	if (secondsElapsed > 0)
	{
		Gotoxy(50, 2);
		cout << "Timer: " << secondsElapsed << "/" << TIMER_MAX;
	}

	SelectTextColour(clRed);
	Gotoxy(50, 3);
	cout << "Welcome " << mouse.name << " - Level: " << level;
	Gotoxy(50, 4);
	cout << "Apples Stored: " << mouse.applesStored;
	Gotoxy(50, 5);
	cout << "        Moves: " << mouse.moves;
	Gotoxy(50, 6);
	cout << " Cheese Eaten: " << mouse.cheeseEaten;
	Gotoxy(50, 7);
	cout << "        Score: " << mouse.score;
	Gotoxy(50, 8);
	cout << "    HighScore: " << mouse.highscore;

	//display menu options available
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(50, 10);
	cout << "TO MOVE USE KEYBOARD ARROWS";

	Gotoxy(50, 11);
	cout << "TO SAVE ENTER 'S'";

	Gotoxy(50, 12);
	cout << "TO LOAD ENTER 'L'";

	Gotoxy(50, 13);
	cout << "TO QUIT ENTER 'Q'";

	Gotoxy(50, 14);
	cout << "TO CHEAT ENTER 'C'";

	//print auxiliary messages if any
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(50, 16);

	cout << message;	//display current message
	message = "";		//reset message to blank
}

//display grid content on screen
void paintGrid(const char grid[][SIZEX+1])
{
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(0, 2);

	for (int row(1); row <= SIZEY; ++row) //for each row (vertically)
	{
		for (int col(1); col <= SIZEX; ++col) //for each column (horizontally)
		{
			if (grid[row][col] == APPLE)
				SelectTextColour(clRed);
			else if (grid[row][col] == CHEESE)
				SelectTextColour(clYellow);
			else if (grid[row][col] == MOUSE)
				SelectTextColour(clGreen);
			else if (grid[row][col] == STORE)
				SelectTextColour(clBlue);

			cout << grid[row][col]; //output cell content
			SelectTextColour(clWhite);
		}
		cout << endl;
	}
}

void endProgram()
{
	char choice;
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(50, 13);

	cout << "\n Press Q to quit\n";
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	cin >> choice;
	while (toupper(choice) != 'Q')
	{
		cin >> choice;
	}
	system("PAUSE");
}

//---------------------------------------------------------------------------
//----- Save Game
//---------------------------------------------------------------------------
bool saveGame(const char grid[][SIZEX+1], const Item mouse, string& message, bool tracker = false)
{
	ofstream saveFileStream;

	if (tracker) //Is it a save for undo?
		saveFileStream.open(mouse.name + ".track", ios::out);
	else
		saveFileStream.open(mouse.name + ".sav", ios::out);

	if (saveFileStream.fail())
		message = "Failed to create file to save the game";
	else
	{
		for (int row(1); row <= SIZEY; ++row)	//for each row (vertically)
		{
			saveFileStream << '\n';
			for (int col(1); col <= SIZEX; ++col)	//for each column (horizontally)
				saveFileStream << grid[row][col]; //output cell content
		}

		saveFileStream << '\n';
		saveFileStream << mouse.x << '\n';
		saveFileStream << mouse.y << '\n';
		saveFileStream << mouse.score << '\n';
		saveFileStream << mouse.moves << '\n';
		saveFileStream << mouse.applesStored << '\n';
		saveFileStream << mouse.cheeseEaten << '\n';

		if (!tracker)
			message = "Successfully saved the game.";

		saveFileStream.close();

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//----- Load Game -
//---------------------------------------------------------------------------
bool loadGame(char grid[][SIZEX + 1], Item& mouse, string& message, vector<Apple>& apples, vector<Cheese>& cheese, bool undo = false)
{
	ifstream loadFileStream;
	if (undo)// Is it a load for undo?
		loadFileStream.open(mouse.name + ".track", ios::in);
	else
		loadFileStream.open(mouse.name + ".sav", ios::in);

	if (loadFileStream.fail())
		message = "Failed to open " + mouse.name + ".txt";
	else
	{
		int row(1), appleCount(0), cheeseCount(0);
		char item;
		string sitem;
		Clrscr();
		apples.clear();
		cheese.clear();
		//While there are rows to insert into the maze grid. initialize each apple they find and build maze[][]
		while (row < SIZEY+1)
		{
			for (int i = 0; i < SIZEX+1; i++)
				if (loadFileStream.good())
				{
					item = loadFileStream.get();
					if (item == APPLE)
					{
						apples.push_back(Apple());
						apples[appleCount].x = i;
						apples[appleCount].y = row;
						apples[appleCount].stored = false;
						grid[row][i] = TUNNEL;
						appleCount++;
					}
					else if (item == CHEESE)
					{
						cheese.push_back(Cheese());
						cheese[cheeseCount].x = i;
						cheese[cheeseCount].y = row;
						cheese[cheeseCount].eaten = false;
						grid[row][i] = TUNNEL;
						cheeseCount++;
					}
					else
						grid[row][i] = item;
				}
				row++;
		}
		item = loadFileStream.get(); // new line
		getline(loadFileStream, sitem); // get x
		istringstream (sitem) >> mouse.x;

		getline(loadFileStream, sitem); // get y
		istringstream (sitem) >> mouse.y;

		getline(loadFileStream, sitem); // get moves
		istringstream (sitem) >> mouse.score;
		getline(loadFileStream, sitem); // get moves
		istringstream (sitem) >> mouse.moves;
		getline(loadFileStream, sitem); // get applesStored
		istringstream(sitem) >> mouse.applesStored;
		getline(loadFileStream, sitem); // get applesStored
		istringstream(sitem) >> mouse.cheeseEaten;

		if (!undo)
			message = "Successfully loaded the game";
		else
			message = "Last move has been undone";
		
		loadFileStream.close();

		return true;
	}

	return false;
}