/**************************************************************
*    Pacman C++ Allegro5                                     *
*    (c) 2015-ComputerMethods3     *
**************************************************************/
// Allegro library
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <time.h>
#include <string>



//Screen resolution
#define SCREEN_W 800
#define SCREEN_H 600
#define TILE_SIZE 32

// Game fps
#define FPS 60

// Direction
#define STAND 0
#define LEFT -1
#define RIGHT 1
#define UP -2
#define DOWN 2

// Game input keys
enum Keyboard { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ESCAPE };
bool keyboard[5] = { false, false, false, false, false };

// Types of objects in the game
enum ObjectType { FOOD, BLOCK };


// Character Object Class
class CharacterObject
{
public:
	float posX; // X coordinate of the Character Object
	float posY; // Y coordinate of the Character Object
	float speed; // Speed of the Character Object
	float angle; // Angle of rotation of Character Object
	int direction; // Direction [0 -> STAND, -1 -> LEFT, 1 -> RIGHT, -2 -> UP, 2 -> DOWN]

	int spritesheetWidth; // Width of the sprite sheet
	int spritesheetHeight; // Height of the sprite sheet
	int totalFrame; // Total number of frames
	int currentFrame; // Current frame counter
	int frameWidth; // Width of the selected region from the sprite sheet
	int frameHeight; // Height of the selected region from the sprite sheet
	int frameDelay; // Frame delay in between animation sequence
	int frameDelayCounter; // Frame delay counter
	ALLEGRO_BITMAP *image; // ALLEGRO_BITMAP to hold the sprite sheet

	// Class constructor
	CharacterObject()
	{
		posX = 0;
		posY = 0;
		speed = 0;
		direction = 0;
		angle = 0;
		spritesheetWidth = 0;
		spritesheetHeight = 0;
		totalFrame = 0;
		currentFrame = 0;
		frameWidth = 0;
		frameHeight = 0;
		frameDelay = 0;
		frameDelayCounter = 0;
		image = NULL;
	}

	// Method To Initialize The CharacterObject
	void InitObject()
	{
		// Returns the width of the sprite sheet
		spritesheetWidth = al_get_bitmap_width(image);
		// Returns the height of the sprite sheet
		spritesheetHeight = al_get_bitmap_height(image);
		// Calculation to find out individual frame width
		frameWidth = spritesheetWidth / totalFrame;
		// Calculation to find out individual frame height
		frameHeight = spritesheetHeight;
		// Makes the background of the sprite sheet transparent
		al_convert_mask_to_alpha(image, al_map_rgb(255, 0, 255));
	}

	// Method to handle drawing logic of the Character Object
	void DrawObject(ALLEGRO_DISPLAY *screen)
	{
		// X coordinate calculation of selected frame/region
		int currentFrameX = currentFrame * frameWidth;
		// Y coordinate of selected frame/region
		int currentFrameY = 0;
		// Empty bitmap to store rotated image
		ALLEGRO_BITMAP *rotated = NULL;
		// Creating the new bitmap with width and height same as the selected frame/region
		rotated = al_create_bitmap(frameWidth, frameHeight);
		// New bitmap selected for drawing instead of backbuffer
		al_set_target_bitmap(rotated);
		// Particular region/frame is selected and drawn on to the new bitmap
		al_draw_bitmap_region(image, currentFrameX, currentFrameY, frameWidth, frameHeight, 0, 0, 0);
		// Drawing target changed back to backbuffer of the screen
		al_set_target_bitmap(al_get_backbuffer(screen));


		/* Logic to handle the rotation of the sprite as per direction
		0 degree is 0 radian
		90 degree is pi / 2
		180 degree is pi
		270 degree is 1.5 * pi
		360 degree is 2 * pi
		Check radian and degree conversion chart
		*/
		switch (direction)
		{
		case STAND:
			angle = angle; // No rotation
			break;
		case LEFT:
			angle = ALLEGRO_PI; // 180 degree rotation
			break;
		case RIGHT:
			angle = 0; // 0 degree rotation
			break;
		case UP:
			angle = ALLEGRO_PI * 1.5; // 270 degree rotation
			break;
		case DOWN:
			angle = ALLEGRO_PI / 2; // 90 degree rotation
			break;
		}
		// Rotate the new bitmap 'rotated' holding our selected region of the sprite sheet
		// Draw it on to the backbuffer
		al_draw_rotated_bitmap(rotated, frameWidth / 2, frameHeight / 2, posX, posY, angle, 0);
		// Free resoruce after using ALLEGRO_BITMAP 'rotated'
		al_destroy_bitmap(rotated);
		// Logic to handle frame delay in animation sequence
		if (++frameDelayCounter >= frameDelay)
		{
			// Frame animation sequence logic
			// Increment frame & reset frame counter to 0 after traversing through all frames
			if (++currentFrame >= totalFrame)currentFrame = 0;
			// If Character Object is standing, only draw the first frame of the sprite sheet
			if (direction == STAND){ currentFrame = 0; frameDelayCounter = frameDelay; }
			// Reset frame delay counter to 0, after it has completed its delay cycle
			frameDelayCounter = 0;
		}
	}

	// Method to handle movement, direction and collision logic of Character Object
	void UpdateLogic()
	{
		//Update object direction as per keyPress
		if (keyboard[KEY_UP])direction = -2;
		if (keyboard[KEY_DOWN])direction = 2;
		if (keyboard[KEY_LEFT])direction = -1;
		if (keyboard[KEY_RIGHT])direction = 1;

		//Update object speed as per direction
		switch (direction)
		{
		case LEFT:
			posX = posX - speed; // Move negative to x-axis
			break;
		case RIGHT:
			posX = posX + speed; // Move along the x-axis
			break;
		case UP:
			posY = posY - speed; // Move negative to y-axis
			break;
		case DOWN:
			posY = posY + speed; // Move along the y-axis
			break;
		case STAND: // No update in position
			break;
		}

		//Check for collision [Object moving out of the screen]
		if (posX >= SCREEN_W)posX = 10; // If Character Object position outside right of the screen
		if (posY >= SCREEN_H)posY = 10; // If Character Object position outside bottom of the screen
		if (posX <= 0)posX = SCREEN_W;  // If Character Object position outside the left of screen
		if (posY <= 0)posY = SCREEN_H;  // If Character Object position outsided the top of the
	}

	//Method to handle collision
	template <class T>
	void CollisionLogic(T &object)
	{
		if (this->posX + 10 > (object.posX - object.boundedBoxWidth / 2) &&
			this->posX - 10 < (object.posX + object.boundedBoxWidth + object.boundedBoxWidth / 2) &&
			this->posY + 10 > (object.posY - object.boundedBoxHeight / 2) &&
			this->posY - 10 < (object.posY + object.boundedBoxHeight + object.boundedBoxHeight / 2))
		{
			switch (object.type)
			{
			case BLOCK:
			{
				switch (direction)
				{
				case LEFT:
					posX = posX + 2;
					break;
				case RIGHT:
					posX = posX - 2;
					break;
				case UP:
					posY = posY + 2;
					break;
				case DOWN:
					posY = posY - 2;
					break;
				}
				direction = STAND; // Collision occurs, change direction to stand
				break;
			}
			case FOOD:
			{
				object.alive = false;
				break;
			}

			}

		}

	}

};

class GameObject
{
public:
	float posX; // X coordinate of the Game Object
	float posY; // Y coordinate of the Game Object
	int type; // Type of Game Object
	bool alive; // Boolean flag to check if Game Object is alive
	int boundedBoxWidth; // Bounded box width for collision
	int boundedBoxHeight; // Bounded box height for collision
	ALLEGRO_BITMAP *image; // ALLEGRO_BITMAP to hold the image

	GameObject()
	{
		posX = 0;
		posY = 0;
		alive = true;
		boundedBoxWidth = 0;
		boundedBoxHeight = 0;
		image = NULL;
	}

	// Method To Initialize The GameObject
	void InitObject(ALLEGRO_BITMAP *image, float posX, float posY, int type)
	{
		this->image = image;
		this->posX = posX;
		this->posY = posY;
		this->type = type;
		al_convert_mask_to_alpha(this->image, al_map_rgb(255, 0, 255));
		boundedBoxWidth = al_get_bitmap_width(image);
		boundedBoxHeight = al_get_bitmap_height(image);
	}

	// Method to draw the GameObject
	void DrawObject()
	{
		if (alive)
			al_draw_bitmap(image, posX, posY, 0);
	}


};

// GameManager class
class GameManager
{
public:
	ALLEGRO_FILE *levelData; // ALLEGRO_FILE to operate external file
	ALLEGRO_BITMAP *foodSprite, *blockSprite; // ALLEGRO_BITMAP to hold food sprite, level block sprite
	CharacterObject Pacman; // Pacman object creation of class CharacteObject
	GameObject *gameObject; // Pointer to GameObject

	int gameObjectCount; // Variable to hold total count of Game Object's in a level
	int levelGrid[SCREEN_H / TILE_SIZE][SCREEN_W / TILE_SIZE]; // 2 dimensional array to hold level data from a external file

	// Method to Initialize Level
	void InitLevel()
	{
		Pacman.image = al_load_bitmap("Pacman_Sprite_Sheet.PNG"); // Loading pacman sprite sheet
		Pacman.posX = SCREEN_W / 2; // Starting x-coordinate of pacman on the screen
		Pacman.posY = SCREEN_H / 2; // Starting y-coordinate of pacman on the screen
		Pacman.speed = 2.0; // Movement speed of pacman
		Pacman.totalFrame = 2; // Total number of pacman animation frames in the sprite sheet
		Pacman.frameDelay = 7; // Speed of pacman animation
		Pacman.InitObject(); // Initialize pacman

		gameObject = new GameObject[(SCREEN_W / TILE_SIZE) * (SCREEN_H / TILE_SIZE)];
		gameObjectCount = 0;

		foodSprite = al_load_bitmap("food.png");
		blockSprite = al_load_bitmap("block.png");

		levelData = al_fopen("level_1.data.txt", "r");

		// Loading and initializing Level & Game Objects
		for (int row = 0; row < SCREEN_H / TILE_SIZE; row++)
		{
			for (int column = 0; column < SCREEN_W / TILE_SIZE; column++)
			{
				levelGrid[row][column] = al_fgetc(levelData);
				switch (levelGrid[row][column])
				{
				case 10:
					column--;
					break;
				case 49:
					gameObject[gameObjectCount].InitObject(blockSprite, column * TILE_SIZE, row * TILE_SIZE, BLOCK);
					gameObjectCount++;
					break;
				case 48:
					gameObject[gameObjectCount].InitObject(foodSprite, column * TILE_SIZE, row * TILE_SIZE, FOOD);
					gameObjectCount++;
					break;
				}
			}
		}

	}

	void DrawLevel(ALLEGRO_DISPLAY *screen)
	{
		// Draw level objects
		for (int loopCounter = 0; loopCounter < gameObjectCount; loopCounter++)
		{
			gameObject[loopCounter].DrawObject();
		}

		// Draw pacman Character Object
		Pacman.DrawObject(screen);

	}

	void UpdateLogic()
	{
		// Update pacman Character Object logic
		Pacman.UpdateLogic();
		// Update pacman collision check
		for (int loopCounter = 0; loopCounter < gameObjectCount; loopCounter++)
		{
			Pacman.CollisionLogic(gameObject[loopCounter]);
		}
	}

	// Method to update keypress events
	void KeyboardEventUpdate(ALLEGRO_EVENT &event, bool flag)
	{
		switch (event.keyboard.keycode)
		{
		case ALLEGRO_KEY_UP:
			keyboard[KEY_UP] = flag;
			break;
		case ALLEGRO_KEY_DOWN:
			keyboard[KEY_DOWN] = flag;
			break;
		case ALLEGRO_KEY_LEFT:
			keyboard[KEY_LEFT] = flag;
			break;
		case ALLEGRO_KEY_RIGHT:
			keyboard[KEY_RIGHT] = flag;
			break;
		case ALLEGRO_KEY_ESCAPE:
			keyboard[KEY_ESCAPE] = flag;
			break;
		}
	}

};

int main()
{

	int count = 0;
	int t = 70;
	int clk = 0;


	al_init(); // Start Allegro
	ALLEGRO_DISPLAY *screen = NULL; // Main display
	ALLEGRO_EVENT_QUEUE *event_queue = NULL; // Event queue to maintain events
	ALLEGRO_TIMER *timer = NULL; // Timer to control game fps

	// Initializing screen, event queue and timer
	screen = al_create_display(SCREEN_W, SCREEN_H);
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS); // Timer set to tick at 60 frames per second

	al_init_font_addon();	//Font addon initialized
	al_init_ttf_addon();	//TrueType Font initialized
	ALLEGRO_FONT *font24 = al_load_font("arial.ttf", 24, 0); // Set font to Arial, of size 24
	ALLEGRO_FONT *font36 = al_load_font("arial.ttf", 36, 0); // Set font to Arial, of size 24

	al_init_image_addon(); // Initialize Allegro image addon
	al_install_keyboard(); // Initialize Allegro keyboard
	al_start_timer(timer); // Start timer

	// Register event sources to be handled in the event queue
	al_register_event_source(event_queue, al_get_display_event_source(screen));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	GameManager gameManager; // Object creation
	gameManager.InitLevel(); // Function call

	//Main game loop
	while (!keyboard[KEY_ESCAPE]) // Run while escape key is not pressed
	{
		ALLEGRO_EVENT event; // ALLEGRO_EVENT to hold events
		al_wait_for_event(event_queue, &event); // Wait for events to occur

		// Display handler
		// Draw if timer counter has incremented and there are no events to handle
		// and event queue is empty
		if (event.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(event_queue))
		{
			gameManager.DrawLevel(screen);
			al_flip_display(); // Display backbuffer on to the main screen
			al_clear_to_color(al_map_rgb(0, 0, 0)); // Clear display to black
		}


		//Event handlers
		if (event.type == ALLEGRO_EVENT_TIMER)
		{
			count++;
			clk++;

			gameManager.UpdateLogic(); // Function call

		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			count++;
			clk++;

			break; // End game loop on closing display window
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			count++;
			clk++;

			gameManager.KeyboardEventUpdate(event, true); // Update routine on key press
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP)
		{
			count++;
			clk++;

			gameManager.KeyboardEventUpdate(event, false); // Update routine on key release
		}

		if (clk >= 58) // Updates time every second
		{
			t--;
			std::string str = "Time:  " + std::to_string(t);
			char * cstr = new char[str.length() + 1];
			std::strcpy(cstr, str.c_str());
			al_draw_text(font24, al_map_rgb(255, 255, 255), 660, 520, 0, cstr);
			clk = 0;

		}
		else // Displays Timer
		{
			std::string str = "Time:  " + std::to_string(t);
			char * cstr = new char[str.length() + 1];
			std::strcpy(cstr, str.c_str());
			al_draw_text(font24, al_map_rgb(255, 255, 255), 660, 520, 0, cstr);

		}


		if (count >= 4118) // Runs the game until 70 seconds
		{
			exit(0);

		}

	} // End of Game Loop

	al_destroy_display(screen); // Destroy display
	return 0;
}
