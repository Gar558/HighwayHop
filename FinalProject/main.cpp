#include <raylib.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

//const Global Variables
const int CIRCLE_RADIUS = 24;
const int CAR_HEIGHT = 80;
const int CAR_WIDTH = 150;
const int UFO_HEIGHT = 40;
const int UFO_WIDTH = 220;
const int GROUND = 610;
const int JUMP_HEIGHT = 400;
const int LEFT = 40;
const int RIGHT = 1550;
const int MAX = 2;  

//Struct/CLasses
typedef struct Person
{
	Vector2 position;
	int radius;
	Color color;
};
typedef struct Car
{
	Rectangle rec;
	Color color;
	bool active;
	int carSpeed;
};

typedef enum GameScreen { TITLE, GAMEPLAY, ENDGAME } GameScreen;  

//Global Variables
int screenWidth = 1600;
int screenHeight = 800;
int currentScore = 0;
int highScore;  //External file
bool quitGame;             //closes window if true
GameScreen currentDisplay;
bool jumpProcessOn = false;     //height increase
bool jumpLeft = false;
bool jumpRight = false;
static bool gameOver;       //sends to ENDGAME if true
//static int score; probably delete this
static Person person;
static Vector2 carPos[MAX]; 
Car* vehiclePtr; //Pointer for dynamic vehicle object.

//Textures
Texture2D titleBackground, gameplayBackground, endGameBackground;
Texture2D car, ufo;

//Sound/Music variables
Sound PlayerJump, PlayerDie;
Sound titleMusic, gamePlaySound, endGameSound;

//Member Variables
bool isDead;  


//Function Prototypes
void LoadResources();

void InitializeGame();

void UpdateAndDraw();

void TitleUpdate();
void TitleDraw();

void GameplayUpdate();
void GameplayDraw();

void EndUpdate();
void EndDraw();

void UnloadExtFiles();   //Unloads textures, music files, etc...


int main()
{
	srand(time(NULL));

	InitWindow(screenWidth, screenHeight, "GAME");
	//InitAudioDevice();
	LoadResources();

	do
	{
		InitializeGame();

		//Main Game Loop
		while (!WindowShouldClose() && !quitGame)
		{
			UpdateAndDraw();
		}
	} while (quitGame == false);
	
	
	//CloseAudioDevice();
	CloseWindow();

	delete vehiclePtr;

	return 0;
}

//Function Definitions

void LoadResources()
{
	//Load Textures
	titleBackground = LoadTexture("resources/title.jpg");
	gameplayBackground = LoadTexture("resources/road.jpg");
	endGameBackground = LoadTexture("resources/gameover.png");
	car = LoadTexture("resources/car4.png");
	ufo = LoadTexture("resources/ufo.png");

	// Load Sounds
	//PlayerJump = LoadSound("resources/sounds/player_jump.wav"); 
	//PlayerDie = LoadSound("resources/sounds/player_die.wav");
}

void InitializeGame()
{
	currentScore = 0;
	ifstream filein;
	filein.open("highscore.txt");
	filein >> highScore;
	filein.close();
	
	vehiclePtr = new Car[MAX];
	gameOver = false;
	SetTargetFPS(60);

	//Load character
	person.radius = CIRCLE_RADIUS;
	person.position = { 75, GROUND };
	person.color = ORANGE;

	//Load vehicle
	for (int i = 0; i < MAX; i+=2)    //Places 1st car at 2000, 2nd at 4000...  
	{
		carPos[i].x = 3000 + 2000 * i;
		carPos[i].y = 530;
	}
	//load UFO
	for (int i = 1; i < MAX; i++)    //Places 1st car at 2000, 2nd at 4000...  
	{
		carPos[i].x = 3000 + 2000 * i;
		carPos[i].y = 337;
	}
	// attach the point in space to car rectangle
	for (int i = 0; i < MAX; i+=2)
	{
		vehiclePtr[i].rec.x = carPos[i].x;
		vehiclePtr[i].rec.y = carPos[i].y;
		vehiclePtr[i].rec.width = CAR_WIDTH;
		vehiclePtr[i].rec.height = CAR_HEIGHT;
		vehiclePtr[i].carSpeed = 10;   //Sets each rectangle to a unique speed
		vehiclePtr[i].active = true;
	}
    // attach the point in space to UFO rectangle
	for (int i = 1; i < MAX; i+=2)
	{
		vehiclePtr[i].rec.x = carPos[i].x;
		vehiclePtr[i].rec.y = carPos[i].y;
		vehiclePtr[i].rec.width = UFO_WIDTH;
		vehiclePtr[i].rec.height = UFO_HEIGHT;
		vehiclePtr[i].carSpeed = rand() % 20 + 15;   //Sets each rectangle to a unique speed
		vehiclePtr[i].active = true;
	}
}

void UpdateAndDraw()
{
	//Music Logic
	/*if (playingMusic)
	{
		// Update stream music buffer
		UpdateMusicStream();

		// Music loop logic
		if (GetMusicTimePlayed() >= GetMusicTimeLength())
		{
			StopMusicStream();
			PlayMusicStream(MUSIC_PATH);
		}
	}*/

	//Update For Each Game Screen
	switch (currentDisplay)
	{
	case TITLE:
	{
		TitleUpdate();
		TitleDraw();
	}break;
	case GAMEPLAY:
	{
		GameplayUpdate();
		GameplayDraw();
	}break;
	case ENDGAME:
	{
		EndUpdate();
		EndDraw();
	}break;
	default: break;
	}
}

void TitleUpdate()
{
	if (IsKeyPressed(KEY_ENTER))
		currentDisplay = GAMEPLAY;
	else if (IsKeyPressed(KEY_Q))
		quitGame = true; //Quits Game
}
void TitleDraw()
{
	BeginDrawing();

	DrawTexture(titleBackground, 0, 0, WHITE);
	DrawText("HIGHWAY HOP", 180, 130, 100, DARKGRAY); //Title
	DrawText("Press ENTER to play or Q to quit", 300, 250, 25, RED); //Play or Quit
	DrawText("Instructions:\nAvoid cars to increase your score", 1000, 50, 25, RED);
	DrawText("Use the LEFT and RIGHT arrow keys \nto move your character side to side", 1000, 150, 25, RED);
	DrawText("and press SPACE to jump over cars \nbut watch out for UFOs flying overhead", 1000, 250, 25, RED);
	EndDrawing();
}

void GameplayUpdate()
{
	if (gameOver == false)
	{
	//-------------------------------------------------------	
	//Apply UFO logic update  (speed of vehicle and make it disappear once off screen)
		for (int i = 1; i < MAX; i += 2)
		{
			if (carPos[i].x <= -170)
			{
				carPos[i].x = 2000 + 2000 * i;
				vehiclePtr[i].carSpeed = rand() % 20 + 15;
				currentScore++;
			}
		}
		if (currentScore >= 0 && currentScore <= 5) 
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 12 + 10;
					currentScore++;
				}
			}
			
		}
		else if (currentScore >= 6 && currentScore <= 10)
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 14 + 12;
					currentScore++;
				}
			}
		}
		else if (currentScore >= 11 && currentScore <= 20)
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 15 + 13;
					currentScore++;
				}
			}
		}

		else if (currentScore >= 21 && currentScore <= 30)
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 17 + 14;
					currentScore++;
				}
			}
		}

		else if (currentScore >= 31 && currentScore <= 40)
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 19 + 15;
					currentScore++;
				}
			}
		}

		else if (currentScore >= 41 && currentScore <= 50)
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 21 + 16;
					currentScore++;
				}
			}
		}

		 else if (currentScore >= 51)
		{
			//Apply car logic update  (speed of vehicle and make it disappear once off screen)
			for (int i = 0; i < MAX; i += 2)
			{
				if (carPos[i].x <= -170)
				{
					carPos[i].x = 2000 + 2000 * i;
					vehiclePtr[i].carSpeed = rand() % 23 + 17;
					currentScore++;
				}
			}
		}
		//----------------------------------------------------------------------------------------
		//Car
		for (int i = 0; i < MAX; i += 2)
		{
			carPos[i].x -= vehiclePtr[i].carSpeed; //For each frame it moves each rectangle its unique amount
			vehiclePtr[i].rec.x = carPos[i].x;     //Changes value of each rec.x for each element
		}
		
		//UFO
		for (int i = 1; i < MAX; i += 2)
		{
			carPos[i].x -= vehiclePtr[i].carSpeed;  
			vehiclePtr[i].rec.x = carPos[i].x;
		}
		
		//Jump Logic------------------------------------------------
		//Jump Up
		if (IsKeyDown(KEY_SPACE) && !isDead && (person.position.y >= GROUND)) //only allows jump process to turn on if touching or beneath ground
			jumpProcessOn = true;
		//if (IsKeyPressed(KEY_SPACE))////////////////////////////////  Play jump sound
		if ((person.position.y <= JUMP_HEIGHT) || IsKeyUp(KEY_SPACE))
			jumpProcessOn = false;    //Turns off jump process once it reaches max height or until space bar is released
		if (jumpProcessOn)
			person.position.y -= 13;
		if (!jumpProcessOn && !(person.position.y >= GROUND))
			person.position.y += 14;

		//Jump Left
		if (IsKeyDown(KEY_LEFT) && !isDead && (person.position.x >= LEFT)) //only allows jump process to turn on if touching or beneath ground
			jumpLeft = true;
		if ((person.position.x <= LEFT) || IsKeyUp(KEY_LEFT))
			jumpLeft = false;    //Turns off jump process once it reaches max height or until space bar is released
		if (jumpLeft)
			person.position.x -= 13;
		if (!jumpLeft && !(person.position.x <= LEFT))
			person.position.x += 0;

		//Jump Right
		if (IsKeyDown(KEY_RIGHT) && !isDead && (person.position.x <= RIGHT)) //only allows jump process to turn on if touching or beneath ground
			jumpRight = true;
		if ((person.position.x >= RIGHT) || IsKeyUp(KEY_RIGHT))
			jumpRight = false;    //Turns off jump process once it reaches max height or until space bar is released
		if (jumpRight)
			person.position.x += 13;
		if (!jumpRight && !(person.position.x <= RIGHT))
			person.position.x += 0;
	}
	
	//Collision
	for (int i = 0; i < MAX; i++)
	{
		if (CheckCollisionCircleRec(person.position, person.radius, vehiclePtr[i].rec))
		{
			gameOver = true;
		
			/*StopMusicStream();/////////////////////////////
			playingMusic = false;
			framesCounter = 0;*/

			// Play die sound
			//PlaySound(playerDie);	
		}
			
		else
			vehiclePtr[i].active = false;
	}

	if (gameOver == true)
		currentDisplay = ENDGAME;
}
void GameplayDraw()
{
	BeginDrawing();

	//Draw Background
	DrawTexture(gameplayBackground, 0, 0, WHITE);

	
	//Draw Player
	if (isDead == false)
		DrawCircle(person.position.x, person.position.y, person.radius, ORANGE);

	//Draw Vehicle
	for (int i = 0; i < MAX; i++)
		DrawRectangle(vehiclePtr[i].rec.x, vehiclePtr[i].rec.y, vehiclePtr[i].rec.width, vehiclePtr[i].rec.height, RED);
	for (int i = 1; i < MAX; i+=2)
		DrawRectangle(vehiclePtr[i].rec.x, vehiclePtr[i].rec.y, vehiclePtr[i].rec.width, vehiclePtr[i].rec.height, LIME);

	//Draw score (current number of jumped cars), and highscore in corner
	DrawText(FormatText("SCORE: %i", currentScore), 30, 30, 20, MAROON);
	DrawText(FormatText("HI-SCORE: %i", highScore), 30, 60, 20, RED);

	EndDrawing();
}

void EndUpdate()
{
	ofstream fileout;
	fileout.open("highscore.txt");
	if (currentScore > highScore)
		highScore = currentScore;
	fileout << highScore;
	fileout.close();

	//Play again or quit 
	if (IsKeyPressed(KEY_R))
	{
		InitializeGame();
		currentDisplay = GAMEPLAY;
	}
	else if (IsKeyPressed(KEY_Q))
		quitGame = true;
}
void EndDraw()
{
	BeginDrawing();

	//ClearBackground(BLACK);

	DrawTexture(endGameBackground, 0, 0, WHITE);
	
	DrawText(FormatText("SCORE: %i", currentScore), 660, 170, 40, DARKPURPLE);
	DrawText(FormatText("HI-SCORE: %i", highScore), 660, 200, 40, DARKPURPLE);

	DrawText("Press R to play again", 660, 240, 40, DARKBLUE);
	DrawText("Press Q to quit game", 660, 270, 40, DARKBLUE);

	EndDrawing();
}

void UnloadExtFiles()
{
	//Unload Textures
	UnloadTexture(titleBackground);
	UnloadTexture(gameplayBackground);
	UnloadTexture(endGameBackground);

	//Unload Music Streaming
	//StopMusicStream();
	//playingMusic = false;
}