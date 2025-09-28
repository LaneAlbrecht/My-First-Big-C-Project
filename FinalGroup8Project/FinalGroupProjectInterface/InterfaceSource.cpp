// GroupProject.cpp : Defines the entry point for the console application.
// Group 8 Pong Game

#include <iostream>
#include <fstream>
#include <time.h>
#include <conio.h>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <algorithm>
using namespace std;

enum eDir { STOP = 0, LEFT = 1, UPLEFT = 2, DOWNLEFT = 3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6 }; //enumerator for directions and ball stop
//ball class
class BALL
{
private:
	int x, y;
	int originalX, originalY;
	eDir direction;

public:
	BALL() {};
	BALL(int posX, int posY) //constructor
	{
		originalX = posX;
		originalY = posY;
		x = posX; y = posY;
		direction = STOP;
	}

	void Reset() //resets ball position
	{
		x = originalX; y = originalY;
		direction = STOP;
	}

	void changeDirection(eDir d)//changes ball direction
	{
		direction = d;
	}

	void randomDirection()//changes ball direction randomly
	{
		direction = (eDir)((rand() % 6) + 1);//only randomizes the directions pertaining to 1-6
	}

	inline int getX() { return x; }
	inline int getY() { return y; }
	inline eDir getDirection() { return direction; }

	void Move() //moving ball function
	{
		switch (direction)
		{
		case STOP:
			break;
		case LEFT: //decrease x cuz goes left on screen number plane things
			x--;
			break;
		case RIGHT:
			x++;
			break;
		case UPLEFT: //direction "+" left or right gives ball angled ascent/descent
			x--; y--;
			break;
		case DOWNLEFT:
			x--; y++;
			break;
		case UPRIGHT:
			x++; y--;
			break;
		case DOWNRIGHT:
			x++; y++;
			break;
		}
	}
};
//end ball class

//paddle class
class PADDLE
{
private:
	int x, y; //position variables
	int originalX, originalY; //starting positions

public:
	PADDLE() //constructor to initialize
	{
		x = y = 0;
	}
	PADDLE(int posX, int posY) : PADDLE() //2nd constructor to initialize starting pos. and set paddle location
	{
		originalX = posX;
		originalY = posY;
		x = posX;
		y = posY;
	}

	inline void Reset() { x = originalX; y = originalY; } //reset paddle location
	inline int getX() { return x; } //gets current x coord
	inline int getY() { return y; } //gets current y coord
	inline void moveUp() { y--; } //moves paddle up when called
	inline void moveDown() { y++; } //moves paddle down when called

};
//end paddle class

//Highscore class (for vector purposes)
class HighScoreEntry
{
public:
	string name;
	int score;
	HighScoreEntry(string winner, int Fscore) { name = winner; score = Fscore; }
};
//end highscore class

//sorting vector struct w/ operator overload
struct SortingVector
{
	//Operator overloading
	bool operator()(const HighScoreEntry& score1, const HighScoreEntry& score2)
	{
		return score1.score < score2.score;
	}
};
//end vector sort struct

//the class that keeps everything good
class GMANAGER : public BALL
{ //inherits BALL
private:
	int width, height; //width/height of game
	int score1, score2; //score variables
	char up1, down1, up2, down2; //variables for moving paddles
	bool quit; //quit game bool variable
	BALL ball; //declares  instance of parent so we can call the functions and override the private variables properly
	PADDLE *player1; // pointer towards paddle for p1
	PADDLE *player2; // pointer towards paddle for p2

public:
	string winner;
	int FinalScore;
	GMANAGER() { FinalScore = 0; } //nonparameterized constructor
	GMANAGER(int w, int h); //manager constructor, takes width/height
	~GMANAGER() { delete player1, player2; } //manager destructor frees up the memory made in constructor
	void ScoreUp(PADDLE * player); //increase score based on which player (paddle) scored
	void DisplayHighScores(); //display highscore board 
	void AddHighScore(string player, int highScore);
	void Draw(); //draw function
	void Input(); // gets user input for game mechanics
	void Logic(); //Game logic, score increases, ball bouncing off walls, etc.
	void Run(); //runs game/gets the highscore board info after user quits
};
//parameterized constructor definition
GMANAGER::GMANAGER(int w, int h)
{
	srand(time(NULL)); //randomize function
	quit = false; //sets quit game variable to false so game runs
	up1 = 'w'; up2 = 'o'; //sets up controls for moving paddles up
	down1 = 's'; down2 = 'l'; //sets up controls for moving paddles down
	score1 = score2 = 0; //initializes scores on start up
	width = w; height = h;
	ball = BALL(w / 2, h / 2); //initializes ball start position
	player1 = new PADDLE(1, h / 2 - 3); //puts paddle1 at center of the left side & allocates memory
	player2 = new PADDLE(w - 2, h / 2 - 3); //puts paddle2 at center of right side & allocates memory
}
//gmanager scoreup definition
void GMANAGER::ScoreUp(PADDLE * player)
{
	//detects which player got the point and gives them points
	if (player == player1)
	{
		score1++;
	}
	else if (player == player2)
	{
		score2++;
	}

	ball.Reset(); //resetting pos. after point
	player1->Reset(); //resetting pos. after point
	player2->Reset(); //resetting pos. after point
}
//gmanager DisplayHighscores definition
void GMANAGER::DisplayHighScores()
{
	string line;
	ifstream scores("scores.txt"); //opens file

	cout << "***** LEADERBOARD *****\n";

	if (scores.is_open()) //detects if file is open
	{
		while (getline(scores, line)) {
			if (line != "\0")
				cout << "  " << line << endl; //displays the user and their highscore in txt file
		}
		scores.close();
	}//if no file display message
	else cout << "There are no current High Scores";

	cout << "***********************\n";
}
//gmanager AddHighscores definition
void GMANAGER::AddHighScore(string player, int highScore)
{
	string s;
	int s_Score;
	ifstream scores; //declares file
	vector<HighScoreEntry> scoreboard; //declares scoreboard vector

	scores.open("scores.txt"); //open scores file
	while (!scores.eof())  //read entire file loop
	{
		getline(scores, s, ':'); //get entire leaderboard line in file
		scores >> s_Score; //read
		scoreboard.emplace_back(s, s_Score);
	}
	scoreboard.pop_back(); //pop the increased size
	scoreboard.emplace_back(winner, highScore); //enters new score into the scoreboard
	sort(scoreboard.begin(), scoreboard.end(), SortingVector()); //sort scoreboard

	scores.close(); //close file
	//displays scoreboard
	ofstream scoresOut; //declare file for output
	scoresOut.open("scores.txt", ios::out, ios::trunc); //open file
	for (int i = 0; i < scoreboard.size(); i++)
	{
		int write_score;
		string write_name;
		write_score = scoreboard[i].score;
		write_name = scoreboard[i].name;
		scoresOut << write_name << ": " << write_score << endl; //output name and score
	}
	scoresOut.close(); //close file
}
//gmanager Draw member definition
void GMANAGER::Draw()
{
	system("cls"); //clearscreen command
	for (int i = 0; i < width + 2; i++) //top wall graphic display
		cout << char(219);
	cout << endl;

	for (int i = 0; i < height; i++) //the drawing loop
	{
		for (int j = 0; j < width; j++)
		{
			int ballx = ball.getX(); //gets ball current x coord
			int bally = ball.getY(); //gets ball current y coord
			int player1x = player1->getX(); //gets p1 paddle current x coord
			int player2x = player2->getX(); //gets p2 paddle current x coord
			int player1y = player1->getY(); //gets p1 paddle current x coord
			int player2y = player2->getY(); //gets p2 paddle current y coord

			if (j == 0) //left wall graphic display
				cout << char(219);

			if (ballx == j && bally == i)
				cout << char(254); //ball graphic display
			else if (player1x == j && player1y == i)
				cout << char(222); //player1 bottom of paddle graphic display
			else if (player2x == j && player2y == i)
				cout << char(221); //player2 bottom of paddle graphic display

			else if (player1x == j && player1y + 1 == i)
				cout << char(222); //player1 graphic display above bottom of paddle
			else if (player1x == j && player1y + 2 == i)
				cout << char(222); //player1 graphic display 2 above bottom of paddle
			else if (player1x == j && player1y + 3 == i)
				cout << char(222); //player1 graphic display 3 above bottom of paddle

			else if (player2x == j && player2y + 1 == i)
				cout << char(221); //player2 graphic display above bottom of paddle
			else if (player2x == j && player2y + 2 == i)
				cout << char(221); //player2 graphic display 2 above bottom of paddle
			else if (player2x == j && player2y + 3 == i)
				cout << char(221); //player2 graphic display 3 above bottom of paddle
			else
				cout << " "; //if not where paddle should be add blank space

			if (j == width - 1) //right wall graphic display
				cout << char(219);
		}
		cout << endl; //skips to next line
	}

	for (int i = 0; i < width + 2; i++) //bottom wall graphic display
		cout << char(219);
	cout << endl;

	cout << "Player 1 Score: " << score1 << endl << "Player 2 Score: " << score2 << endl; //display scores
}
//gmanager Input definition
void GMANAGER::Input()
{
	ball.Move(); //moves ball
	//gets x,y coords real time
	int ballx = ball.getX();
	int bally = ball.getY();
	int player1x = player1->getX();
	int player2x = player2->getX();
	int player1y = player1->getY();
	int player2y = player2->getY();

	if (_kbhit()) //gets user keyboard hit to move things
	{
		char current = _getch(); //pushes " " (empty space)
		if (current == up1) //moves paddle (p1) up as long as paddle isn't at the top
			if (player1y > 0)
				player1->moveUp();
		if (current == up2)  //moves paddle (p2) up as long as paddle isn't at the top
			if (player2y > 0)
				player2->moveUp();
		if (current == down1)  //moves paddle (p1) down as long as paddle isn't at the bottom
			if (player1y + 4 < height)
				player1->moveDown();
		if (current == down2)  //moves paddle (p2) down as long as paddle isn't at the bottom
			if (player2y + 4 < height)
				player2->moveDown();

		if (ball.getDirection() == STOP) //if ball stops/hits object project in random direction
			ball.randomDirection();

		if (current == 'q') //if the player hits q it will quit the game
			quit = true;
	}
}
//gmanager Logic member definition
void GMANAGER::Logic()
{ //also outputs sound based on what is hit
	int ballx = ball.getX();
	int bally = ball.getY();
	int player1x = player1->getX();
	int player2x = player2->getX();
	int player1y = player1->getY();
	int player2y = player2->getY();

	//ball hits left paddle
	for (int i = 0; i < 4; i++)
		if (ballx == player1x + 1)
			if (bally == player1y + i) {
				PlaySound("sound\\plop.wav", NULL, SND_FILENAME | SND_ASYNC); //paddle bounce sound
				ball.changeDirection((eDir)((rand() % 3) + 4));
			}
	//ball hits right paddle
	for (int i = 0; i < 4; i++)
		if (ballx == player2x - 1)
			if (bally == player2y + i) {
				PlaySound("sound\\plop.wav", NULL, SND_FILENAME | SND_ASYNC); //paddle bounce sound
				ball.changeDirection((eDir)((rand() % 3) + 2));
			}
	//ball hits bottom wall
	if (bally == height - 1) {
		PlaySound("sound\\wallbeep.wav", NULL, SND_FILENAME | SND_ASYNC); //wall bounce sound
		ball.changeDirection(ball.getDirection() == DOWNRIGHT ? UPRIGHT : UPLEFT);
	}
	//ball hits top wall
	if (bally == 0) {
		PlaySound("sound\\wallbeep.wav", NULL, SND_FILENAME | SND_ASYNC); //wall bounce sound
		ball.changeDirection(ball.getDirection() == UPRIGHT ? DOWNRIGHT : DOWNLEFT);
	}
	//ball hits right wall
	if (ballx == width - 1) {
		PlaySound("sound\\pong.wav", NULL, SND_FILENAME | SND_ASYNC); //score up sound
		ScoreUp(player1);
	}
	//ball hits left wall
	if (ballx == 0) {
		PlaySound("sound\\pong.wav", NULL, SND_FILENAME | SND_ASYNC); //score up sound
		ScoreUp(player2);
	}
}
//gmanager Run definition
void GMANAGER::Run()
{
	while (!quit) //loops the game until user quits with 'q'
	{
		Draw();
		Input();
		Logic();
	}
	//if game is tied display
	if (score1 == score2)
	{
		cout << "It's a Tie! No winner this time :(\n";

		DisplayHighScores();

		system("pause");
		exit(0); //exit command so that the rest of code will not run
	}
	//set winner's score
	if (score1 > score2)
		FinalScore = score1;
	else
		FinalScore = score2;
	//Get Winner's name
	cout << "Enter the winner's name\n";
	cin >> winner;

	AddHighScore(winner, FinalScore); //adds winner to highscore board
	DisplayHighScores(); //displays highscores after game ends
}

int menu(); //menu function prototype
void instructions(); //instructions function prototype
void title(); //title function prototype

//main
void main()
{
	GMANAGER g(40, 20);
	int choice;
	choice = menu(); //gets user input from menu

	PlaySound("sound\\pong.wav", NULL, SND_FILENAME | SND_ASYNC); //user input successful sound

	while (choice <= 0 || choice > 4) //catches invalid user input
	{
		cout << "Invalid Input, please re-enter your choice\n";
		cin >> choice;
	}

	switch (choice) //does stuff on user input
	{
	case 1: { //run game
		g.Run();
	} break;
	case 2: { //go to instructions
		instructions();
	} break;
	case 3: { //display leaderboard
		g.DisplayHighScores();
	} break;
	case 4: { //exit program
		exit;
	} break;
	default:
	{ //default for invalid input
		cout << "Invalid Input again, shutting down\n";
		exit;
	}break;
	}

	system("pause");
}
//end main

//menu function definition
int menu()
{
	int choice;
	title();
	cout << endl;
	//output user's options through menu
	cout << "1. Play The Game!\n" << "2. How To Play\n" << "3. Highscores\n" << "4. Exit\n" << endl;
	cout << "Enter your Selection: \n";
	cin >> choice;
	//return user input for main
	return choice;
}
//end menu function definition

//instructions screen function definition
void instructions()
{
	int choice = 0; //variable for user returning to menu

	cout << "\nHow to Play the Game:\n";
	//top border display
	for (int i = 0; i < 67; i++)
		cout << "*";
	//output instructions
	cout << "\n*   \t\t Player 1 controls: 'w' and 's' \t\t  *" << "\n*   \t\t Player 2 controls: 'o' and 'l' \t\t  *\n";
	cout << "* Hit 'Q' on your keyboard when you make your opponent surrender! *\n";
	//bottom border display
	for (int i = 0; i < 67; i++)
		cout << "*";

	//give user menu options
	cout << "\n Please Enter your Selection: \n";
	cout << "1. Return to Menu\n" << "2. Exit\n" << endl;
	cout << "Enter your Selection: \n";
	cin >> choice;
	cout << endl; //skips line to better distance the menu that appears in main if user goes back
	
	while (choice <= 0 || choice > 2) //catches invalid user input
	{
		cout << "Invalid Input, please re-enter your choice\n";
		cin >> choice;
	}

	//return to menu switch case
	switch (choice)
	{
	case 1:
	{
		main();
	} break;
	case 2:
	{
		exit;
	}break;
	default:
	{ //default for invalid user input
		cout << "Invalid Input again, shutting down\n";
		exit;
	}break;
	}
}
//end instructions definition

//Title definition
void title()
{ //displays title of game with border
	cout << "WELCOME TO\n";
	//Top of Border Display
	for (int i = 0; i < 40; i++)
		cout << "*";
	cout << "\n"; //Display Title of Game
	cout << "* ULTRA VIRTUAL MINI TENNIS 3000 - X   *" << endl;
	cout << "*        (THE REMAKE) v1.01            *" << endl;
	//bottom of border Display
	for (int i = 0; i < 40; i++)
		cout << "*";
	cout << "\n";
}
//end title definition