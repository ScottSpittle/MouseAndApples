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
const int HIGH_SCORES_TO_KEEP(5);
const int MAX_CHEESE (6); //Maximum number of cheese to place in the game

//defining symbols used for display of the grid & content
const char PLAYER ('@');
const char TUNNEL (' ');
const char WALL ('#');
const char APPLE ('O');
const char STORE ('+');
const char CHEESE ('*');

//defining the command letters to move the blob on the maze
const int UP (72);
const int DOWN (80);
const int RIGHT (77);
const int LEFT (75);

//defining the other command letters
const char QUIT ('Q');
const char SAVE ('S');
const char LOAD ('L');
const char CHEAT ('C');
const char UNDO('U');
const char BEST_SCORES('B');

//player Data Structure
struct Player {
    int X, Y, HighScore, Score, Level, LastKey;
    string Name;
    bool Cheated, TimedGame;
    const char Symbol;
};

//HighScorers Structure
struct HighScorer {
    int HighScore;
    string Name;
};

//Item Data Structure
struct Item {
    int X, Y;
    bool Stored;
    const char Symbol;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------
int main()
{
    // Function declarations (prototypes)
    void BeginGame(Player&);
    void initialiseGame(char[][SIZEX+1], char[][SIZEX+1], Player&, string&, vector<Item>&, vector<Item>&);
    void paintGame(const char [][SIZEX+1], string&, Player, double = 0);
    void endProgram();
    void moveplayer(const char[][SIZEX + 1], Player&, int, string&, vector<Item>&, vector<Item>&);
    void updateGrid(char[][SIZEX + 1], const char[][SIZEX + 1], Player, vector<Item>&, vector<Item>&);
    int getKeyPress();
    bool isArrowKey(int);
	void DisplayHighScores(vector<HighScorer>);
	void updateGameStats(Player&, const vector<Item>&, vector<HighScorer>&);
	void processCheat(Player&, string&, vector<Item>&, vector<Item>&);
	void checkForSave(Player, string&);

    //local variable declarations 
    char grid[SIZEY+1][SIZEX+1];		// Grid for display
    char maze[SIZEY+1][SIZEX+1];		// Buffer Grid

    Player player = { ((SIZEX / 2) + 2), ((SIZEY / 2) + 1), 500, 0, 1, 0, "Scott", false, false, PLAYER};

    vector<Item> Apples;				// Vector to hold the Apples
    vector<Item> Cheese;				// Vector to hold the Cheese
    vector<HighScorer> HighScorers;		// Vector to hold the HighScorers
    string GameMessage("LET'S START...");	// Current GameMessage to player

    //Locals for Timed Game option
    time_t timer;
    struct tm gameStartTime;
    double secondsElapsed(0);

    BeginGame(player);

    Clrscr(); //clear screen

    initialiseGame(grid, maze, player, GameMessage, Apples, Cheese); // Initialise grid (incl. walls & player)
	updateGameStats(player, Apples, HighScorers); //updates stats(moves made, apples stored.)
    paintGame(grid, GameMessage, player); //display game info, modified grid & messages
    saveGame(grid, player, GameMessage, true); //Save the game for an initial undo.

    int key(getKeyPress());	//read in  selected key: arrow or letter command

    time(&timer);  /* get current time; same as: timer = time(NULL)  */
    gameStartTime = *localtime(&timer); //Tracking the time elapsed

    while (key != QUIT) //while user does not want to quit
    {
        //if (player.TimedGame)
        //{
        //	time(&timer);  /* get current time; same as: timer = time(NULL)  */
        //	secondsElapsed = difftime(timer, mktime(&gameStartTime));

        //	if (secondsElapsed >= TIMER_MAX)
        //	{
        //		GameMessage = "Game Over! Timer Expired";
        //		endProgram();
        //	}
        //}
        
        if (isArrowKey(key))
        {
            saveGame(grid, player, GameMessage, true); //Save the game for undo
            moveplayer(grid, player, key, GameMessage, Apples, Cheese); //move player in that direction
            updateGrid(grid, maze, player, Apples, Cheese); //update grid information
        }
        else if (toupper(key) == BEST_SCORES)
        {
        	DisplayHighScores(HighScorers); //Display HighScores
			while (!getch()); //Resume Game on key press
        }
        else if (toupper(key) == SAVE)
        {
        	if (!player.Cheated && !player.TimedGame)//So long as the user has not cheated and it's not a timed game, Save
        		saveGame(grid, player, GameMessage);
        	else
        		GameMessage = "Cannot save if you've cheated or it's a timed game.";
        }
        //else if (toupper(key) == LOAD)
        //{
        //	if (!player.cheated && !timedGame)//So long as the user has not cheated and it's not a timed game, Save
        //	{
        //		if (loadGame(grid, player, GameMessage, apples, cheese))//Ensure it loads successfully before updating the grid
        //		{
        //			moveplayer(grid, player, key, GameMessage, apples, cheese); //move player in that direction
        //			updateGrid(grid, maze, player, apples, cheese); //update grid information
        //		}
        //	}
        //	else
        //		GameMessage = "Cannot load if you've cheated or it's a timed game.";
        //}
        //else if (toupper(key) == UNDO)
        //{
        //	if (!player.cheated && !timedGame)//Undo the last move if allowed.
        //	{
        //		if (keyLast != UNDO)
        //		{
        //			loadGame(grid, player, GameMessage, apples, cheese, true); //Recycling the load procedure for the undo
        //			moveplayer(grid, player, key, GameMessage, apples, cheese); //move player in that direction
        //			updateGrid(grid, maze, player, apples, cheese); //update grid information
        //		}
        //		else
        //			GameMessage = "No!!";
        //	}
        //	else
        //		GameMessage = "You cheated, you can't undo.";
        //}
        else if (toupper(key) == CHEAT)
        {
        	processCheat(player, GameMessage, Apples, Cheese); //Call Process Cheat
        	moveplayer(grid, player, key, GameMessage, Apples, Cheese); //move player in that direction
        	updateGrid(grid, maze, player, Apples, Cheese); //update grid information
        }

        checkForSave(player, GameMessage); //Checks for save if there is not timed game
        updateGameStats(player, Apples, HighScorers); //updates stats(moves made, apples stored.)
        paintGame(grid, GameMessage, player, secondsElapsed); //display game info, modified grid & messages

        //if (player.applesStored == (int)apples.size() && player.cheeseEaten == (int)cheese.size()) //Check if the game has been won
        //	gameCompleted(grid, maze, player, level, GameMessage, apples, cheese, highScorers);
        
        player.LastKey = key;
        key = getKeyPress(); //display menu & read in next option
    }

    endProgram(); //display final GameMessage
    return 0;
}

//Checking if a save already exsists for the player
void checkForSave(Player player, string& GameMessage)
{
    ifstream loadFileStream;
    loadFileStream.open(player.Name+".sav", ios::in); //Does a save file exsist

    if (!loadFileStream.fail())
    	if (!player.Cheated && !player.TimedGame)
    		GameMessage = "Save found, press L to load.";

    loadFileStream.close();
}

//---------------------------------------------------------------------------
//----- Begin Game
//---------------------------------------------------------------------------
void BeginGame(Player& player)
{
    char timerOption;

    // Get the user's name
    cout << "Please enter your name: ";
    cin >> player.Name;

    // Get the level
    cout << "Select Level 1, 2, 3: ";
    cin >> player.Level;

    while (player.Level < 1 || player.Level > 3)
    {
        cout << "\nPlease enter a level between 1 and 3: ";
        cin >> player.Level;
    }

    // Does the user want a Timed Game?
    cout << "Would you like to plat a timed game? You will have 90 seconds to complete it.";
    cin >> timerOption;

    while (toupper(timerOption) != 'Y' && toupper(timerOption) != 'N')
    {
        cout << "\nPlease enter 'y' or 'n': ";
        cin >> timerOption;
    }

    // Setting Timed Game Option
    if (toupper(timerOption) == 'Y')
        player.TimedGame = true;

    return;
}

//---------------------------------------------------------------------------
//----- Cheat Procesure
//---------------------------------------------------------------------------
void processCheat(Player& player, string& GameMessage, vector<Item>& Apples, vector<Item>& Cheese)
{    
    cout << '\a' << '\a' << '\a'; //beep the alarm
    player.Cheated = true;
    player.Score = 1000;
    GameMessage = "You Cheated!";

    //Setting all cheese to eaten
    for (int i = 0; i < (int)Cheese.size(); i++)
		Cheese[i].Stored = true;

    //Move all the apples into the store
	for (int i = 0; i < (int)Apples.size(); i++)
    	if (!Apples[i].Stored)
    	{
			Apples[i].Stored = true;
			Apples[i].X = 0;
			Apples[i].Y = 0;
    	}

    //depending on the level, place the last apple at the store.
    switch (player.Level)
    {
	case 2:
		Apples[Apples.size() - 1].Stored = false;
		Apples[Apples.size() - 1].X = 4;
		Apples[Apples.size() - 1].Y = 5;
    	player.X = 5;
    	player.Y = 5;
    	break;
	default:
		Apples[Apples.size() - 1].Stored = false;
		Apples[Apples.size() - 1].X = 16;
		Apples[Apples.size() - 1].Y = 8;
    	player.X = 15;
    	player.Y = 8;
    	break;
    }
}

//---------------------------------------------------------------------------
//----- Game Complete
//---------------------------------------------------------------------------
void gameCompleted(char grid[][SIZEX + 1], char maze[][SIZEX + 1], Player& player, int& level, string& GameMessage, vector<Item>& apples, vector<Item>& cheese, vector<HighScorer>& highScorers)
{
    //void initialiseGame(char grid[][SIZEX+1], char maze[][SIZEX+1], Player& player, int level, string& GameMessage, vector<Item>& apples, vector<Item>& cheese);
    //void paintGame(const char grid[][SIZEX + 1], string& GameMessage, Player player, int level, double secondsElapsed = 0);
    //void updateGameStats(Player& player, const vector<Item>& apples, vector<HighScorer>& highScorers, int level);
    //void restartGame(Player& player, vector<Item>& apples, string& GameMessage, vector<Item>& cheese);
    //void setHighScoreFinal(Player& player, vector<HighScorer>& highScorers, int level);
    //void endProgram();
    //
    //SelectTextColour(clYellow);
    //Gotoxy(50, 16);

    ////As long as it's not the last level.
    //if (level < 3)
    //{
    //	if (player.cheated) //If they cheated.
    //		cout << "\nWell done! Level: " << level << " complete, but you cheated! \nNext Level? y/n ";
    //	else
    //		cout << "\nWell done! Level: " << level << "complete \nNext Level? y/n ";

    //	if (getch() == 'y') //Set the nwe level
    //		level += 1;
    //	else
    //	{
    //		setHighScoreFinal(player, highScorers, level);// Setting final highscorers.
    //		exit(0);
    //	}
    //}
    //else //Game Complete
    //{ 
    //	cout << "Game Complete. Press a key to close.";
    //	getch();
    //	exit(0);
    //}
    //
    ////Restart the game.
    //restartGame(player, apples, GameMessage, cheese);

    //SelectTextColour(clWhite);

    //initialiseGame(grid, maze, player, level, GameMessage, apples, cheese); //initialise grid (incl. walls & player)
    //updateGameStats(player, apples, vector<HighScorer>(), level); //updates stats(moves made, apples stored.)
    //setHighScoreFinal(player, highScorers, level-1);
    //paintGame(grid, GameMessage, player, level); //display game info, modified grid & messages
}

//reset variable/object values
void restartGame(Player& player, vector<Item>& apples, string& GameMessage, vector<Item>& cheese)
{
    //apples.clear(); //clear all the apples
    //cheese.clear();

    ////reset player.
    //player.X = 0;
    //player.Y = 0;
    //player.moves = 0;
    //player.highscore = 500;
    //player.cheated = false;
    //player.cheeseEaten = 0;

    //GameMessage = ""; //reset GameMessage.
}
//---------------------------------------------------------------------------
//----- Update Game Stats
//---------------------------------------------------------------------------
void updateGameStats(Player& player, const vector<Item>& Apples, vector<HighScorer>& highScorers)
{
    //count how many apples are marked as stores.
    void getHighScore(Player& player, vector<HighScorer>& highScorers);
    void setHighScore(Player& player);

    //get the players new highscore
    getHighScore(player, highScorers);

    if (!player.Cheated)
    	setHighScore(player); //Set players highscore if not cheated
}

//getting highscore from the players .sav file
void getHighScore(Player& player, vector<HighScorer>& highScorers)
{
    string highScore; // String object of highscore for casting
    ifstream loadFileStream; //load file stream
    
    loadFileStream.open(player.Name+".txt", ios::in);
    if (loadFileStream.good())
    {
    	getline(loadFileStream, highScore); // get moves
    	istringstream(highScore) >> player.HighScore;
		player.HighScore = player.HighScore > 500 ? player.HighScore : 500; //set the players highscore to 500 if more than 500
    }
    else
    	player.HighScore = 500;//set the players highscore if one isn't found

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
    			highScorers[i].Name += "Anonymous";
    			highScorers[i].HighScore = 500;
    		}
    	}

    	saveFileStream.close();
    }
    else //If BestScores is found. populate the vector
    {
    	for (int i = 0; i < HIGH_SCORES_TO_KEEP; i++)
    	{
    		if ((int)highScorers.size() < HIGH_SCORES_TO_KEEP)
    			highScorers.push_back(HighScorer()); //add a new HighScorer object

    		getline(loadFileStream, highScore);
    		istringstream(highScore) >> highScorers[i].Name; //csating string to int via string stream.
    		getline(loadFileStream, highScore);
    		istringstream(highScore) >> highScorers[i].HighScore;
    	}
    }

    loadFileStream.close();
}

//Setting highscore to external save
void setHighScore(Player& player)
{
    ofstream saveFileStream;
    
    saveFileStream.open(player.Name+".txt", ios::out);
    saveFileStream.clear();

    if (saveFileStream.good())
    	saveFileStream << player.HighScore;

    saveFileStream.close();
}

//Setting highscore to external save
void setHighScoreFinal(Player& player, vector<HighScorer>& highScorers, int level)
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
    		if (player.Score < highScorers[i].HighScore && !indicesAffected)
    		{
    			saveFileStream << player.Name << endl;
    			saveFileStream << player.Score << endl;
    			indicesAffected = true;
    		}
    		else
    		{
    			saveFileStream << highScorers[i].Name << endl;
    			saveFileStream << highScorers[i].HighScore << endl;
    		}
    	}
    }
    saveFileStream.close();
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------
void initialiseGame(char grid[][SIZEX+1], char maze[][SIZEX+1], Player& player, string& GameMessage, vector<Item>& apples, vector<Item>& cheese)
{
    //initialise grid & place player in middle
    void setInitialMazeStructure(char [][SIZEX+1], Player, string&, vector<Item>&, vector<Item>&);
    void setInitialplayerCoordinates(Player&);
    void updateGrid(char [][SIZEX+1], const char [][SIZEX+1], Player, vector<Item>&, vector<Item>&);

    setInitialMazeStructure(maze, player, GameMessage, apples, cheese); // initialise maze
    setInitialplayerCoordinates(player);									// initialise player's position
    updateGrid(grid, maze, player, apples, cheese);						// prepare grid
}

//set the position of the walls in the maze
void setInitialMazeStructure(char maze[][SIZEX + 1], Player player, string& GameMessage, vector<Item>& Apples, vector<Item>& Cheese)
{ 
    //initialise maze configuration, local array to store the maze structure
    // with '#' for wall, ' ' for tunnel and 'X' for unused part of array
    ifstream loadFileStream;
    int row(1), appleCount(0);
    char item;

    switch (player.Level)
    {
    case 3:
        {
            loadFileStream.open("maze3.maz", ios::in);		//load maze3 file
            if (loadFileStream.fail())						//if it fails to open the .maz file
                GameMessage = "Failed to initialize maze 3.";
            break;
        }
    case 2:
        {
            loadFileStream.open("maze2.maz", ios::in);
            if (loadFileStream.fail())
                GameMessage = "Failed to initialize maze 2.";
            break;
        }
    default:
        {
            loadFileStream.open("maze1.maz", ios::in);
            if (loadFileStream.fail())
                GameMessage = "Failed to initialize maze 1.";
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
                    Apples.push_back(Item { i, row, false, APPLE });
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
        bool EmptySlot = false; //is there already cheese there
        int x, y; //checking location

        do
        {
			EmptySlot = false; //reset
            x = Random(SIZEX); //random coords
            y = Random(SIZEY);

            //if its all clear place a cheese
            if (maze[y][x] == TUNNEL)
            {
                for (int j = 0; j < Cheese.size(); j++)
                    if (Cheese[j].X == x && Cheese[j].Y == y)
                    {
						EmptySlot = true;
                        break;
                    }
                for (int k = 0; k < Apples.size(); k++)
                    if (Apples[k].X == x && Apples[k].Y == y)
                    {
						EmptySlot = true;
                        break;
                    }
					if (!EmptySlot)
                {
                    Cheese.push_back(Item{ x, y, false, CHEESE });
                    CheesePlaced = true;
                }
            }
        } while (!CheesePlaced);
    }
}

//calculate player's coordinates at beginning of game
void setInitialplayerCoordinates(Player& player)
{
    switch (player.Level)
    {
    case 2:
        player.X = 7;
        player.Y = 5;
        break;
    default:
        player.X = 15;
        player.Y = 9;
        break;
    }
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------
void updateGrid(char grid[][SIZEX+1], const char maze[][SIZEX+1], Player player, vector<Item>& Apples, vector<Item>& Cheese)
{
    //update grid configuration after each move
    void setMaze(char grid[][SIZEX+1], const char maze[][SIZEX+1]);
    void placeplayer(char grid[][SIZEX+1], Player player);
    void placeApples(char grid[][SIZEX + 1], vector<Item>& Apples);
    void placeCheese(char grid[][SIZEX + 1], vector<Item>& Cheese);

    setMaze(grid, maze); //reset the empty maze configuration into grid
    placeCheese(grid, Cheese);
    placeApples(grid, Apples);
    placeplayer(grid, player); //set player in grid
}

//reset the empty/fixed maze configuration into grid
void setMaze(char grid[][SIZEX+1], const char maze[][SIZEX+1])
{
    for (int row(1); row <= SIZEY; ++row) //for each row (vertically)
        for (int col(1); col <= SIZEX; ++col) //for each column (horizontally)
            grid[row][col] = maze[row][col];
}

//place player at its new position in grid
void placeplayer(char grid[][SIZEX+1], Player player)
{
    grid[player.Y][player.X] = player.Symbol;
}

//place apples at there new positions in the grid
void placeApples(char grid[][SIZEX + 1], vector<Item>& Apples)
{
    for (int i = 0; i < (int)Apples.size(); i++) //for each apple in the vector
        if (!Apples[i].Stored) //if the apple has not been stored
            grid[Apples[i].Y][Apples[i].X] = Apples[i].Symbol; //draw apple
}

//place cheese at there new positions in the grid
void placeCheese(char grid[][SIZEX + 1], vector<Item>& Cheese)
{
    for (int i = 0; i < (int)Cheese.size(); i++) //for each apple in the vector
        if (!Cheese[i].Stored) //if the apple has not been stored
            grid[Cheese[i].Y][Cheese[i].X] = Cheese[i].Symbol; //draw apple
}

//---------------------------------------------------------------------------
//----- move player in required direction
//---------------------------------------------------------------------------
void moveplayer(const char grid[][SIZEX+1], Player& player, int key, string& GameMessage, vector<Item>& Apples, vector<Item>& Cheese)
{
    void setKeyDirection(int key, int& dirX, int& dirY); //calculate direction of movement required by key - if any
    int dirX(0), dirY(0);

    setKeyDirection(key, dirX, dirY); //find direction indicated by key

    //check new target position in grid & update player coordinates if move is possible
    //depending on what's on the target position in grid...
    switch(grid[player.Y+dirY][player.X+dirX])
    {
    case TUNNEL:
        player.Y += dirY; //go in that Y direction
        player.X += dirX; //go in that X direction
        player.Score++;
        break;
    case CHEESE:
        player.Y += dirY; //go in that Y direction
        player.X += dirX; //go in that X direction
        for (int i = 0; i < (int)Cheese.size(); i++) //for each apple in the vector
        {
            if (Cheese[i].X == player.X && Cheese[i].Y == player.Y) //if it is the cheese you hit
            {
                Cheese[i].Stored = true;
				Cheese[i].X = 0;
				Cheese[i].Y = 0;
                player.Score++;
            }
        }
        break;
    case APPLE:
		switch (grid[player.Y + (dirY * 2)][player.X + (dirX * 2)])
		{
			case APPLE:
				break;
			case CHEESE:
				for (int i = 0; i < (int)Apples.size(); i++)
				{
					if (Apples[i].X == player.X + dirX && Apples[i].Y == player.Y + dirY)
					{
						Apples[i].X += dirX;
						Apples[i].Y += dirY;
					}
				}
				player.X += dirX;
				player.Y += dirY;
				break;
			case TUNNEL:
				for (int i = 0; i < (int)Apples.size(); i++)
				{
					if (Apples[i].X == player.X + dirX && Apples[i].Y == player.Y + dirY)
					{
						Apples[i].X += dirX;
						Apples[i].Y += dirY;
					}
				}
				player.X += dirX;
				player.Y += dirY;
				break;
			case STORE:
				for (int i = 0; i < (int)Apples.size(); i++)
				{
					if (Apples[i].X == player.X + dirX && Apples[i].Y == player.Y + dirY)
					{
						Apples[i].Stored = true;
						Apples[i].X = 0;
						Apples[i].Y = 0;
					}
				}
				player.X += dirX;
				player.Y += dirY;
				break;
		}
        break;
    case WALL:
		if (player.X + dirX >= SIZEX)
			if (grid[player.Y][2] == TUNNEL)
			{
				player.X = 2;
				player.Score++;
			}
			else
				GameMessage = "Path Blocked";
		else if (player.Y + dirY >= SIZEY)
			if (grid[2][player.X] == TUNNEL)
			{
				player.Y = 2;
				player.Score++;
			}
			else
				GameMessage = "Path Blocked";
		else if (player.X + dirX <= 1)
			if (grid[player.Y][SIZEX - 1] == TUNNEL)
			{
				player.X = SIZEX - 1;
				player.Score++;
			}
			else
				GameMessage = "Path Blocked";
		else if (player.Y + dirY <= 1)
			if (grid[SIZEY - 1][player.X] == TUNNEL)
			{
				player.Y = SIZEY - 1;
				player.Score++;
			}
			else
				GameMessage = "Path Blocked";
		else
			cout << '\a'; //beep the alarm
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
    	cout << highScorers[i-3].Name << " : " << highScorers[i-3].HighScore;
    }
}

void paintGame(const char grid[][SIZEX + 1], string& GameMessage, Player player, double secondsElapsed = 0)
{ 
    //display game title, messages, maze, player & apples on screen
    void paintGrid(const char grid[][SIZEX+1]);

    Clrscr();

    //display game title
    SelectTextColour(clYellow);
    Gotoxy(0, 0);
    cout << "___player AND APPLES GAME___\n" << endl;
    SelectTextColour(clWhite);
    Gotoxy(50, 0);
    cout << "Group 1V - Scott, Josh, Damon";

    // display grid contents
    paintGrid(grid);
    
    Gotoxy(50, 1);
    cout << GetDate() << " " << GetTime();
    if (player.TimedGame)
    {
        Gotoxy(50, 2);
        cout << "Timer: " << secondsElapsed << "/" << TIMER_MAX;
    }

    SelectTextColour(clRed);
    Gotoxy(50, 3);
    cout << "Welcome " << player.Name << " - Level: " << player.Level;
    Gotoxy(50, 4);
    cout << "        Score: " << player.Score;
    Gotoxy(50, 5);
    cout << "    HighScore: " << player.HighScore;

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

    cout << GameMessage;	//display current GameMessage
    GameMessage = "";		//reset GameMessage to blank
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
            else if (grid[row][col] == PLAYER)
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
    Gotoxy(50, 16);

    cout << "Game Over!\n";

    system("PAUSE");
}

//---------------------------------------------------------------------------
//----- Save Game
//---------------------------------------------------------------------------
bool saveGame(const char grid[][SIZEX + 1], const Player player, string& GameMessage, bool tracker = false)
{
	ofstream saveFileStream;

	if (tracker) //Is it a save for undo?
		saveFileStream.open(player.Name + ".track", ios::out);
	else
		saveFileStream.open(player.Name + ".sav", ios::out);

	if (saveFileStream.fail())
		GameMessage = "Failed to create file to save the game";
	else
	{
		for (int row(1); row <= SIZEY; ++row)	//for each row (vertically)
		{
			saveFileStream << '\n';
			for (int col(1); col <= SIZEX; ++col)	//for each column (horizontally)
				saveFileStream << grid[row][col]; //output cell content
		}

		saveFileStream << '\n';
		saveFileStream << player.X << '\n';
		saveFileStream << player.Y << '\n';
		saveFileStream << player.Score << '\n';

		if (!tracker)
			GameMessage = "Successfully saved the game.";

		saveFileStream.close();

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//----- Load Game -
//---------------------------------------------------------------------------
bool loadGame(char grid[][SIZEX + 1], Player& player, string& GameMessage, vector<Item>& Apples, vector<Item>& Cheese, bool undo = false)
{
	ifstream loadFileStream;
	if (undo)// Is it a load for undo?
		loadFileStream.open(player.Name + ".track", ios::in);
	else
		loadFileStream.open(player.Name + ".sav", ios::in);

	if (loadFileStream.fail())
		GameMessage = "Failed to open " + player.Name + ".txt";
	else
	{
		int row(1), appleCount(0), cheeseCount(0);
		char item;
		string sitem;
		Clrscr();
		Apples.clear();
		Cheese.clear();
		//While there are rows to insert into the maze grid. initialize each apple they find and build maze[][]
		while (row < SIZEY + 1)
		{
			for (int i = 0; i < SIZEX + 1; i++)
			if (loadFileStream.good())
			{
				item = loadFileStream.get();
				if (item == APPLE)
				{
					Apples.push_back(Item { i, row, false, APPLE});
					grid[row][i] = TUNNEL;
					appleCount++;
				}
				else if (item == CHEESE)
				{
					Cheese.push_back(Item{ i, row, false, CHEESE});
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
		istringstream(sitem) >> player.X;

		getline(loadFileStream, sitem); // get y
		istringstream(sitem) >> player.Y;

		getline(loadFileStream, sitem); // get moves
		istringstream(sitem) >> player.Score;

		if (!undo)
			GameMessage = "Successfully loaded the game";
		else
			GameMessage = "Last move has been undone";

		loadFileStream.close();

		return true;
	}

	return false;
}