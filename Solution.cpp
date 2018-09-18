/*
Copyright: Marco Cardoso 2016

Project: Kill the Political Yard Signs

Extras:
    1) Menu design
        *Includes the header, and the legend of the game
    2) Colors
        *Houses, Human and PSYs have individual colors
    3) Houses
        *Additional element in the game
    4) Hardcore Mode
        *Limited the vision of the player, only 3 steps around the player location
         are printed
    5) Sounds
        *Sound are played when an effective attack is performed, when an entry in the
         game setup is invalid, and when the game end. (Using method Beep)
    6) Resizable board
        *User can choose the size of the board from 9x9 to 18x18
    7) Help
        *Provide to the user the controls to move the Human using the keyboard
    8) Game Speed
        *Property in struct Board, control the rate of refresh of th display in the screen
    9) Keyboard
        *Uses the keys AWDX to move Horizontal and Vertical, and QECZ to move diagonal;

NOTE: The Colors and music composition were obtained from the Internet
*/
#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <vector>

using namespace std;

enum Speed {
	SLOW = 1000,
	REGULAR = 500,
	FAST = 250
};

enum Colors{
	WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN ,
	GRAY = FOREGROUND_INTENSITY,
	BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE
};

enum Keys {
	UP = 119,
	DOWN = 120,
	LEFT = 97,
	RIGHT = 100,
	UPRIG = 101,
	UPLEF = 113,
	DOWNRIG = 99,
	DOWNLEF = 122,
	HELP = 63,
	EXIT = 27,
	SCS = 111,
	FLAMET = 112
};

enum Element { Empty, Human, PYS_Stupid, PYS_Smart, House };

struct Board {
	int dimension;
	bool hardCore;
	int nPSY;
	int humanIQ;
	Element ** _board;
	Speed gameSpeed;
	bool ** _stunnedAux; //Indicator of the stunned status of Human or PSY
};

struct Point {
	int i;
	int j;
};


//			CONTROLLERS
//Initialize the board values and manage the main menu
Board initializer();

//Create the board and manage how to fill it
Board gameGenerator(int, bool, int, int, int, int);

//Fill the board  with a given element and a given amount, randomly
void randomFill(int, int, Element, Element **);


//			VALIDATORS
//Validates the values entered by the user in the main menu.
int validator(string, Colors, int);

//Evaluate if the game is over using the iQ of the human and the number of PSY
bool endOfGame(Board);

//Evaluate if exist target (given Element) in a given range. If there is one,
//return true modify the var Point with the position of it.
bool targetInRange(Board, Point, int, Point &, Element);

//Evaluate if a point is inside the board
bool inRange(Board, Point);


//			ACTION
//Read the key pressed, evaluate if the movement was valid
bool keyBoardAction(Board &, Point &, int);

//Move the element from point with the increment values int rows and columns
bool moveTo(Board &, Point &, int, int);

//Manage the movement of the Board's elements
void moveElements(Board &);

//Move an element a given times to random positions
void elementRandMove(Board &, Point &, int);

//Return a Point with the human location
Point humanLocation(Board);


//			PYS
//Manage the attack of a given PYS
bool pysAttack(Board &, Point);

//Perform the SHS attack on a given target
void atackSHS(Board &, Point);

//Perform the BS attack on a given target
void atackBS(Board &, Point, Point);

//Perform the given number of actions as PYS stupid
void actionStupidPYS(Board &, Point &, int);

//Perform the given number of actions as PYS smart
void actionSmartPYS(Board &, Point , int );


//			HUMAN
//Perform attack SCS on a given target
void atackSCS(Board &, Point);

//Perform attack Flame T. on a given target
void atackFlameT(Board &, Point);


//			AUX
//Generate a random number from 0 to the number entered as parameter
int randomGenerator(int);

//Return true or false random
bool randomGenerator();

//Convert from int to string
string to_String(int);

//Creates an array of int with values from 0 to parameter int value unorganized
int * randomSequence(int);

//Replace in a given position the element with Empty
void eraseElement(Board &, Point);


//			PRINT
//Print the string received as parameter with the Color entered.
void printColor(string, Colors);
void printColor(char, Colors);

//Print in console the corresponding character
void printElement(Element);

//Print the board on the console
void printBoard(Board);

//Print help
void printHelp();

//Print the logo
void printHeader();

//Print a Warning of not PYS on range
void printWarningAttack();

//Print the remaining IQ of the human and the actions. Print help
void printStatsControl(Board , int);

//Print the result of the game
void printFinalResult(Board);


//Play music at the end of the game
void playSoundLose();

void playSoundWin();


int main(int argc, const char * argv[]) {

	Board board = initializer();
	printBoard(board);
	bool stopPlaying=endOfGame(board);
	while (!stopPlaying) {
		moveElements(board);
		stopPlaying = endOfGame(board);
	}
	printFinalResult(board);
	return 0;
}


//CONTROLLERS
Board initializer() {
	printHeader();
	int dimensions = 0;
	do
	{
		printColor("Enter the dimensions: ", WHITE);
		cin >> dimensions;
		if (dimensions < 9 || dimensions > 18) {
			printColor("\tPlease enter a value between 9 and 18.\n", RED);
			Beep(500, 500);
		}
	} while (dimensions < 9 || dimensions>18);

	int smartPSY = validator("Enter the number of Smart PSY: ", MAGENTA, dimensions*dimensions/8);
	int stupidPSY = validator("Enter the number of Stupid PSY: ", MAGENTA, dimensions*dimensions / 8);
	int houses = validator("Enter the number of Houses: ", BLUE, dimensions*dimensions / 6);
	bool hardCore = validator("Do you want try HARDCORE mode? (1-Yes / 0-No): ", RED, 1);
	int gameSpeed = validator("Enter the game speed (0-Slow | 1-Regular |2-Fast): ", GREEN, 2);

	return gameGenerator(dimensions, hardCore, smartPSY, stupidPSY, houses, gameSpeed);
}

Board gameGenerator(int dimensions, bool hardCore, int psySmart, int psyStupids, int houses, int gameSpeed) {
	Element ** board = new Element*[dimensions];
	bool ** stunnedAux = new bool*[dimensions];
	for (int i = 0; i < dimensions; i++) {
		board[i] = new Element[dimensions]{ Empty };
		stunnedAux[i] = new bool[dimensions] {false};
	}

	randomFill(psySmart, dimensions, PYS_Smart, board);
	randomFill(psyStupids, dimensions, PYS_Stupid, board);
	randomFill(houses, dimensions, House, board);
	randomFill(1, dimensions, Human, board);

	Speed gSpeed=REGULAR;
	switch (gameSpeed)
	{
	case 0:
		gSpeed = SLOW;
		break;
	case 1:
		gSpeed = REGULAR;
		break;
	case 2:
		gSpeed = FAST;
		break;
	default:
		break;
	}

	Board b = { dimensions, hardCore,psySmart + psyStupids,120,board,gSpeed, stunnedAux  };
	return b;
}

void randomFill(int count, int dimensions, Element elem, Element ** board) {
	while (count > 0)
	{
		int i = randomGenerator(dimensions);
		int j = randomGenerator(dimensions);
		if (board[i][j] == Empty) {
			board[i][j] = elem;
			count--;
		}
	}
}


//VALIDATORS
int validator(string message, Colors color, int max) {
	int value = 0;
	do
	{
		printColor(message, color);
		cin >> value;
		if (value<0 || value>max) {
			Beep(500, 500);
			printColor("\tPlease enter a value  between 0 and "+to_String(max)+"\n", RED);
		}
	} while (value<0 || value>max);
	return value;
}

bool inRange(Board board, Point location) {
	if (location.i >= 0 && location.i < board.dimension)
		return (location.j >= 0 && location.j < board.dimension);
	else return false;
}

bool endOfGame(Board board) {
	return (board.nPSY == 0 || board.humanIQ < 70);
}

bool targetInRange(Board board, Point location, int range, Point & targetPos, Element target) {
    int varI[8] = {0,0,-1,1,-1,-1,1,1}; //L R U P DUL DUR DDL DDR
    int varJ[8] = {-1,1,0,0,-1,1,-1,1};
    for(int z=0; z<8; z++){
		Point temp = { location.i + varI[z] * range,location.j + varJ[z] * range };
        if(inRange(board,temp) && board._board[temp.i][temp.j]==target){
			targetPos.i = temp.i;
			targetPos.j = temp.j;
			if (range > 1) {
				for (int i = 1; i < range; i++){
					Point objMid = { location.i + varI[z] * i,location.j + varJ[z] * i };
					if (board._board[objMid.i][objMid.j] != Empty)
						return false;
				}
				return true;
			}
			else return true;
        }
    }
	return false;
}


//ACTIONS
void moveElements(Board & board) {
	Point humLoc = humanLocation(board);
	int actionsLeft = 2;
	if (board._stunnedAux[humLoc.i][humLoc.j]) {
		actionsLeft--;
		board._stunnedAux[humLoc.i][humLoc.j] = false;
	}
	printBoard(board);
	printStatsControl(board, actionsLeft);
	while (actionsLeft>0)
	{
		if (keyBoardAction(board, humLoc, actionsLeft)) {
			actionsLeft--;
			printBoard(board);
			printStatsControl(board, actionsLeft);
		}
	}

	vector<Point> pysList;
	for (int i = 0; i < board.dimension; i++){
		for (int j = 0; j < board.dimension; j++){
			if (board._board[i][j] == PYS_Stupid || board._board[i][j] == PYS_Smart) {
				Point p = { i,j };
				pysList.push_back(p);
			}
		}
	}
	while (!pysList.empty())
	{
		Point p = pysList.back();
		pysList.pop_back();
		if (board._board[p.i][p.j] == PYS_Smart)
			actionSmartPYS(board, p, 2);
		else actionStupidPYS(board, p, 2);
	}
}

Point humanLocation(Board board) {
	Point location = { -1,-1 };
	for (int i = 0; i < board.dimension; i++) {
		for (int j = 0; j < board.dimension; j++) {
			if (board._board[i][j] == Human) {
				location.i = i;
				location.j = j;
			}
		}
	}
	return location;
}

bool keyBoardAction(Board & board, Point & location, int actionsLeft) {
	int key = _getch();
	switch (key) {
	case UP:
		return moveTo(board, location, -1, 0);
	case RIGHT:
		return moveTo(board, location, 0, 1);;
	case DOWN:
		return moveTo(board, location, 1, 0);
	case LEFT:
		return moveTo(board, location, 0, -1);
	case UPRIG:
		return moveTo(board, location, -1, 1);
	case UPLEF:
		return moveTo(board, location, -1, -1);
	case DOWNLEF:
		return moveTo(board, location, 1, -1);
	case DOWNRIG:
		return moveTo(board, location, 1, 1);
	case SCS:
		Point targetSCS;
		if (targetInRange(board, location, 1, targetSCS, PYS_Smart) || targetInRange(board, location, 1, targetSCS, PYS_Stupid)) {
			atackSCS(board, targetSCS);
			return true;
		}
		else printWarningAttack();
		break;
	case FLAMET:
		Point targetFlamt;
		if (targetInRange(board, location, 2, targetFlamt, PYS_Smart) || targetInRange(board, location, 2, targetFlamt, PYS_Stupid)) {
			atackFlameT(board, targetFlamt);
			return true;
		}
		else printWarningAttack();
		break;
	case HELP:
		printHelp();
		printBoard(board);
		printStatsControl(board, actionsLeft);
		break;
	case EXIT:
		exit(0);
		break;
	default:
		break;
	}
	return false;
}

bool moveTo(Board & board, Point & location, int incI, int incJ) {
	Point temp = { location.i + incI, location.j + incJ };
	if (inRange(board, temp) && board._board[temp.i][temp.j] == Empty){
		swap(board._board[temp.i][temp.j], board._board[location.i][location.j]);
		swap(board._stunnedAux[temp.i][temp.j], board._stunnedAux[location.i][location.j]);
		location.i = temp.i;
		location.j = temp.j;
		printBoard(board);
		return true;
	}
	else return false;
}

void elementRandMove(Board & board, Point & location, int times) {
	while (times > 0)
	{
		int * randomSeq = randomSequence(8);
		int varI[8] = { 0,0,-1,1,-1,-1,1,1 }; //Directional Array
		int varJ[8] = { -1,1,0,0,-1,1,-1,1 };
		for (int z = 0; z < 8; z++)
		{
				Point temp = { location.i + varI[randomSeq[z]] ,location.j + varJ[randomSeq[z]] };
				if (inRange(board, temp)) {
					if (moveTo(board, location, varI[randomSeq[z]], varJ[randomSeq[z]]))
						break;
				}
		}
		times--;
	}
}


//PYS
void actionStupidPYS(Board & board, Point & location, int actionsLeft) {
	if (board._stunnedAux[location.i][location.j]) {
		actionsLeft--;
		board._stunnedAux[location.i][location.j] = false;
	}
    while(actionsLeft>0){
        bool atack = randomGenerator();
        if(!(atack && pysAttack(board,location)))
			elementRandMove(board,location,1);
        actionsLeft--;
    }
}

void actionSmartPYS(Board & board, Point location, int actionsLeft) {
	if (board._stunnedAux[location.i][location.j]) {
		actionsLeft--;
		board._stunnedAux[location.i][location.j] = false;
	}
	while (actionsLeft>0)
	{
		bool actAsStupid = randomGenerator();
		if (actAsStupid) {
			actionStupidPYS(board, location, 1);
			actionsLeft--;
		}
		else {
			if (!pysAttack(board, location))
				elementRandMove(board, location, 1);
			actionsLeft--;
		}
	}
}

bool pysAttack(Board & board, Point location) {
	Point targetPos = { -1,-1 };
	if (targetInRange(board, location, 1, targetPos, Human)) {
		atackSHS(board, targetPos);
		return true;
	}
	else if (targetInRange(board,location, 2, targetPos, Human)) {
		atackBS(board,targetPos,location);
		return true;
	}
	return false;
}

void atackSHS(Board & board, Point targetPos) {
	int chancePercent = 45;
	int r = randomGenerator(100);
	if (r <= chancePercent) {
        Beep(200, 150);
		board._stunnedAux[targetPos.i][targetPos.j] = true;
		board.humanIQ -= 10;
		if (!endOfGame(board)) {
			int times = randomGenerator(5);//Run away
			elementRandMove(board, targetPos, times);
		}
	}
}

void atackBS(Board & board, Point targetPos, Point attacker) {
	int chancePercent = 75;
	if (board._stunnedAux[targetPos.i][targetPos.j]) chancePercent = 90;
	int r = randomGenerator(100);
	if (r <= chancePercent) {
        Beep(200, 150);
		board._stunnedAux[targetPos.i][targetPos.j] = true;
		board.humanIQ -= 10;
		int iDirec = (targetPos.i - attacker.i);
		int jDirec = (targetPos.j - attacker.j);
		if (!moveTo(board, targetPos, iDirec, jDirec))
			moveTo(board, targetPos, iDirec/2, jDirec/2);
	}
}


//HUMAN
void atackSCS(Board & board, Point targetPos) {
    int r = randomGenerator(100);
	if (r <= 50) {
		board._stunnedAux[targetPos.i][targetPos.j] = true;
		Beep(1200, 150);
	}
}

void atackFlameT(Board & board, Point targetPos) {
    int chancePercent = 60;
    if(board._stunnedAux[targetPos.i][targetPos.j])
        chancePercent=90;
    int r=randomGenerator(100);
	if (r <= chancePercent) {
		eraseElement(board, targetPos);
		Beep(1200, 150);
	}
}


//AUX
int randomGenerator(int max) {
	return rand() % max;
}

bool randomGenerator() {
	return rand() % 2;
}

string to_String(int number){
	stringstream result;
	result << number;
    return result.str();
}

int * randomSequence(int max){
	int * sequence = new int[max];
	for (int i = 0; i < max; i++)
		sequence[i] = i;
	for (int i = 0; i < max * 2; i++) {
		int r1=randomGenerator(max);
		int r2 = randomGenerator(max);
		swap(sequence[r1], sequence[r2]);
	}
	return sequence;
}

void eraseElement(Board & board, Point location){
    board._board[location.i][location.j]=Empty;
    board._stunnedAux[location.i][location.j]=false;
	board.nPSY--;
	printBoard(board);
}


//PRINT METHODS
void printElement(Element element) {
	switch (element) {
	case Human:
		printColor(char(233), GREEN);
		break;
	case PYS_Stupid:
		printColor(char(248), RED);
		break;
	case PYS_Smart:
		printColor(char(248), RED);
		break;
	case House:
		printColor(char(254), BLUE);
		break;
	default: //EMPTY
		printColor(char(250), GRAY);
		break;
	}
	cout << " ";
}

void printBoard(Board board) {
	Sleep(board.gameSpeed);
	system("CLS");
	string borderHorizontal = "";
	for (int i = 0; i < board.dimension; i++) {
		borderHorizontal += char(196);
		borderHorizontal += char(196);
	}
	Point humanLoc = humanLocation(board);

	cout << char(218) + borderHorizontal +char(191) +"\n";
	for (int i = 0; i < board.dimension; i++)
	{
		cout << "|";
		for (int j = 0; j < board.dimension; j++){
			if (board.hardCore) {
				int iDistance = abs((humanLoc.i - i));
				int jDistance = abs((humanLoc.j - j));
				if(iDistance<4 && jDistance<4) printElement(board._board[i][j]);
				else cout << "  ";
			}
			else printElement(board._board[i][j]);
		}
		cout << "|\n";
	}
	cout << char(192) + borderHorizontal + char(217);
	cout << "\n Human -> ";
	printColor(char(233), GREEN);
	cout << "\n PSY -> ";
	printColor(char(248), RED);
	cout << "\n House -> ";
	printColor(char(254), BLUE);
	cout << "\n";
	printColor("     IQ: ", RED);
	cout << board.humanIQ << endl;
}

void printColor(string txt, Colors color) {
	HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hconsole, color);
	cout << txt;
	SetConsoleTextAttribute(hconsole, WHITE);
}

void printColor(char txt, Colors color) {
	HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hconsole, color);
	cout << txt;
	SetConsoleTextAttribute(hconsole, WHITE);
}

void printHeader() {
	cout << "        (";
	cout << char(169);
	printColor(char(254),BLUE);
	cout << "_";
	printColor(char(254), BLUE);
	cout << ")-";
	printColor('=',GRAY);
	printColor(char(203), GRAY);
	printColor(char(209), GRAY);
	printColor('#', RED);
	printColor("- - - -", YELLOW);
	cout << "(";
	printColor("X  X", MAGENTA);
	cout << ")\n" << endl;
	printColor("- - - Kill the Political Yard Signs - - -\n\n", GREEN);
}

void printWarningAttack() {
	printColor("\nNo PYS on range", RED);
	Sleep(700);
}

void printStatsControl(Board board, int actionsLeft) {
	printColor("\nIS YOUR TURN!\n", YELLOW);
	printColor("Actions: ", GREEN);
	cout << actionsLeft << endl;
	printColor("\n?-Help\tEsc-Exit\n", GRAY);
}

void printHelp() {
	system("CLS");
	printColor("- - - HELP - - -\n\n", YELLOW);
	cout << "Attacks:\n\tO- Spray Can of Silence \n\tP- Flamethrower";
	cout << "\n\nMovements: \nUp- W\nRight- D\nDown- X\nLeft- A\nDiagonals- Q, E, Z and C\n";
	Sleep(4000);
}

void printFinalResult(Board board) {
	if (board.nPSY == 0) {
		system("CLS");
		printColor("\n\tYOU WIN", GREEN);
		playSoundWin();
	}
	else if (board.humanIQ < 70) {
		system("CLS");
		printColor("\n\tYOU LOST", RED);
		playSoundLose();
	}
}

void playSoundLose() {
	Beep(440, 500);
	Beep(440, 500);
	Beep(440, 500);
	Beep(349, 350);
	Beep(523, 150);
	Beep(440, 500);
	Beep(349, 350);
	Beep(523, 150);
	Beep(440, 1000);
	Beep(659, 500);
	Beep(659, 500);
	Beep(659, 500);
	Beep(698, 350);
	Beep(523, 150);
	Beep(415, 500);
	Beep(349, 350);
	Beep(523, 150);
	Beep(440, 1000);
}

void playSoundWin() {
	Beep(659, 250);
	Beep(659, 250);
	Beep(659, 300);
	Beep(523, 250);
	Beep(659, 250);
	Beep(784, 300);
	Beep(392, 300);
	Beep(523, 275);
	Beep(392, 275);
	Beep(330, 275);
	Beep(440, 250);
	Beep(494, 250);
	Beep(466, 275);
	Beep(440, 275);
	Beep(392, 275);
	Beep(659, 250);
	Beep(784, 250);
	Beep(880, 275);
	Beep(698, 275);
	Beep(784, 225);
	Beep(659, 250);
	Beep(523, 250);
	Beep(587, 225);
	Beep(494, 225);
}
