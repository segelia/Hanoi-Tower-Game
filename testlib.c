#include "primlib.h"
#include <stdlib.h>
#include <stdbool.h>

#define NO_OF_PEGS 10
#define NO_OF_BLOCKS 20
#define DISTANCE_BETWEEN_PEGS gfx_screenWidth() / (NO_OF_PEGS + 1)
#define HEIGHT_OF_PEG gfx_screenHeight() / 2
#define ANIMATION_HEIGHT (gfx_screenHeight() / 4)
#define HALF_WIDTH_OF_PEG 2
#define BASE_LENGTH_OF_DISK (gfx_screenWidth() / (NO_OF_PEGS + 1)) / NO_OF_BLOCKS
#define HEIGHT_OF_DISK HEIGHT_OF_PEG / (NO_OF_BLOCKS * 2)
#define DISK_COLOR BLUE
#define PEG_COLOUR RED

#define ANIMATION_SPEED 10

int covert_sdl_keycode_to_number(int keycode)
{
	if (keycode >= SDLK_0 && keycode <= SDLK_9)
	{
		return keycode - SDLK_0;
	}
	else if (keycode == SDLK_0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void draw_pegs()
{
	int current_distance_inc;
	for (int x1 = 1; x1 <= NO_OF_PEGS; x1++)
	{
		current_distance_inc = x1 * DISTANCE_BETWEEN_PEGS;
		gfx_filledRect(current_distance_inc - HALF_WIDTH_OF_PEG, HEIGHT_OF_PEG, current_distance_inc + HALF_WIDTH_OF_PEG, gfx_screenHeight(), PEG_COLOUR);
	}
}

void drawing_blocks(int gamestate[NO_OF_BLOCKS][NO_OF_PEGS])
{
	for (int row = 0; row < NO_OF_BLOCKS; row++)
	{
		for (int col = 0; col < NO_OF_PEGS; col++)
		{
			int disk_size = gamestate[row][col];
			if (disk_size > 0)
			{
				int disk_width = disk_size * BASE_LENGTH_OF_DISK;
				int elevation = NO_OF_BLOCKS - row;
				gfx_filledRect(DISTANCE_BETWEEN_PEGS * (col + 1) - disk_width / 2, (gfx_screenHeight() + HEIGHT_OF_DISK) - (elevation * HEIGHT_OF_DISK), DISTANCE_BETWEEN_PEGS * (col + 1) + disk_width / 2, gfx_screenHeight() - (elevation * HEIGHT_OF_DISK), DISK_COLOR);
			}
		}
	}
}

void draw_view(int gamestate[NO_OF_BLOCKS][NO_OF_PEGS])
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), BLACK);
	draw_pegs();
	drawing_blocks(gamestate);
}

void init_gamestate(int gamestate[NO_OF_BLOCKS][NO_OF_PEGS])
{
	for (int i = NO_OF_BLOCKS - 1; i >= 0; i--)
	{
		gamestate[i][0] = i + 1;
	}
}

void draw_animated_block(int x, int y, int size){
	int disk_width = size * BASE_LENGTH_OF_DISK;
	gfx_filledRect(x - disk_width / 2, y + HEIGHT_OF_DISK, x + disk_width / 2, y, DISK_COLOR);
}

bool move_block(int gamestate[NO_OF_BLOCKS][NO_OF_PEGS], int source_peg, int destination_peg)
{
    int temporary_storage = 0;
	int destination_slot;
    int source_x = DISTANCE_BETWEEN_PEGS * (source_peg + 1);
    int dest_x = DISTANCE_BETWEEN_PEGS * (destination_peg + 1);

	//int source_y=gfx_screenHeight() - (elevation * HEIGHT_OF_DISK)

    // finds the block on source_peg, returns true if no blocks
	int block;
	for(block=0;block<NO_OF_BLOCKS;block++){
		if(gamestate[block][source_peg]!=0) break;
	}
	if(block>=NO_OF_BLOCKS) return true;
	// takes a block off the source peg
	temporary_storage=gamestate[block][source_peg];
	gamestate[block][source_peg]=0;

	int source_y = gfx_screenHeight()-(NO_OF_BLOCKS - block)*HEIGHT_OF_DISK;

	// finds the empty spot on the destination_peg (there should always be an empty spot)
	for(block=NO_OF_BLOCKS-1;block>=0;block--){
		if(gamestate[block][destination_peg]==0) break;
	}
	destination_slot=block;
	int destination_y = gfx_screenHeight()-(NO_OF_BLOCKS - block)*HEIGHT_OF_DISK;
	//animation here:
	int animated_x=source_x;
	int animated_y=source_y;

	while(animated_y>ANIMATION_HEIGHT){
		draw_view(gamestate);
		draw_animated_block(animated_x, animated_y, temporary_storage);
		animated_y-=ANIMATION_SPEED;
		gfx_updateScreen();
		SDL_Delay(10);
	}
	int dx=abs(dest_x-source_x)/(dest_x-source_x);
	while(abs(animated_x-dest_x)>2*ANIMATION_SPEED){
		draw_view(gamestate);
		draw_animated_block(animated_x, animated_y, temporary_storage);
		animated_x+=dx*ANIMATION_SPEED;
		gfx_updateScreen();
		SDL_Delay(10);
	}
	while(animated_y<destination_y){
		draw_view(gamestate);
		draw_animated_block(animated_x, animated_y, temporary_storage);
		animated_y+=ANIMATION_SPEED;
		gfx_updateScreen();
		SDL_Delay(10);
	}
	// puts the block in the correct slot
	gamestate[destination_slot][destination_peg]=temporary_storage;
	// check if the move was legal
	if(destination_slot<NO_OF_BLOCKS-1){
		if(gamestate[destination_slot][destination_peg]>gamestate[destination_slot+1][destination_peg]){
			return false;
		}
	}
	return true;
}

int checking_input()
{
	int keycode = gfx_getkey();
	int peg = covert_sdl_keycode_to_number(keycode);

	while (peg < 0 || peg >= NO_OF_PEGS)
	{
		printf("\nInvalid peg number. Please enter a number between 0 and %d.\n", NO_OF_PEGS - 1);
		keycode = gfx_getkey();
		peg = covert_sdl_keycode_to_number(keycode);
	}

	return peg;
}

bool is_win(int gamestate[NO_OF_BLOCKS][NO_OF_PEGS])
{
	int blocks_on_last_peg = 0;
	for (int level3 = 0; level3 < NO_OF_BLOCKS; level3++)
	{
		if (gamestate[level3][NO_OF_PEGS - 1] != 0)
		{
			blocks_on_last_peg++;
		}
	}

	// Check if all blocks are on the last peg
	if (blocks_on_last_peg == NO_OF_BLOCKS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int main(int argc, char *argv[])
{
	if (gfx_init())
	{
		exit(3);
	}

	// Setting up the initial gamestate - all blocks on first peg
	int gamestate[NO_OF_BLOCKS][NO_OF_PEGS] = {0};
	init_gamestate(gamestate);
	draw_view(gamestate);
	gfx_updateScreen();

	// Starting the game loop and input checking mech
	int playing = true;

	while (playing)
	{
		// Taking input from the player
		int source_peg = checking_input();
		int destination_peg = checking_input();

		// Losing and moving blocks mechanic (no animation)
		if (!move_block(gamestate, source_peg, destination_peg))
		{
			gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), RED);
			gfx_updateScreen();
			playing = false;
			break;
		}

		// Updating the display after a block has been moved
		draw_view(gamestate);
		gfx_updateScreen();

		// Winning
		if (is_win(gamestate))
		{
			gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), GREEN);
			gfx_updateScreen();
			playing = false;
			break;
		}
	}
	SDL_Delay(3000);
	return 0;
}
