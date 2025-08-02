#include <Wire.h>

// Screen size
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define SCREEN_SIZE     (SCREEN_WIDTH*SCREEN_HEIGHT/8)

// Screen image
uint8_t screen[SCREEN_SIZE];

// Program init
void setup() {
  // Init serial for debug messages
  Serial.begin(115200);
  // Init display
  Display_init();
}

// Main loop
void loop() {
  uint32_t inicio = millis();
  Display_update();
  uint32_t fim = millis();
  if (generate() == 0) {
    // No more alive cells, Draw again initial display
    blinker(100, 10);
    glider_gun(2,2);
  }
  Serial.print("Update: ");
  Serial.print(fim-inicio);
  Serial.print(" Generate: ");
  Serial.println(millis()-fim);
}

//----------------------------------
// Game of Life

// Auxiliary screen image
uint8_t aux[SCREEN_SIZE];

#define WIDTH SCREEN_WIDTH
#define HEIGHT SCREEN_HEIGHT

// Mark cell as alive
inline void set_cell(unsigned char *mat, int x, int y) {
	mat[x+y/8*WIDTH] |= (1 << (y & 7));
}

// Mark cell as dead
inline void clear_cell(unsigned char *mat, int x, int y) {
	mat[x+y/8*WIDTH] &= ~(1 << (y & 7));
}

// Check if cell is alive
inline unsigned char get_cell(unsigned char *mat, int x, int y) {
	if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT)) {
		return 0;
	}
	return (mat[x+y/8*WIDTH] & (1 << (y & 7))) == 0? 0 : 1;
}

// Draw a blinker pattern on the screen
void blinker(int x, int y) {
	set_cell(screen, x+1, y);
	set_cell(screen, x+1, y+1);
	set_cell(screen, x+1, y+2);
}

// Draw a glider pattern on the screen
void glider(int x, int y) {
	set_cell(screen, x+1, y);
	set_cell(screen, x+2, y+1);
	set_cell(screen, x, y+2);
	set_cell(screen, x+1, y+2);
	set_cell(screen, x+2, y+2);
}

const char gun[9][37] = {
  "........................X...........",
  "......................X.X...........",
  "............XX......XX............XX",
  "...........X...X....XX............XX",
  "XX........X.....X...XX..............",
  "XX........X...X.XX....X.X...........",
  "..........X.....X.......X...........",
  "...........X...X....................",
  "............XX....................."
};

void glider_gun(int x, int y) {
  for (int dy = 0; dy < 9; dy++) {
    for (int dx = 0; dx < 36; dx++) {
      if (gun[dy][dx]=='X') {
        set_cell(screen, x+dx, y+dy);
      }
    }
  }
}

// Offsets to cell neighboors
const int dir[][2] = {
	{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
};

// Generate next generation
// Returns the total of alive cells in previous generation
int generate() {
	memcpy (aux, screen, SCREEN_SIZE);
  int total_alive = 0;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int live = 0;
			for (int i = 0; i < 8; i++) {
				live += get_cell(aux, x+dir[i][0], y+dir[i][1]);
			}
      int alive = get_cell(aux, x, y);
			if (alive && ((live < 2) || (live > 3))) {
				clear_cell(screen, x, y);
			} else if (!alive && (live == 3)) {
				set_cell(screen, x, y);
			}
      total_alive += alive;
		}
	}
  return total_alive;
}
