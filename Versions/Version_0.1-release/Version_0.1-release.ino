//A Tetris like Game for the Arduino Uno
//;Core code written by Yves
//
//THIS SKETCH FALLS UNDER THE GNU GENERAL PUBLIC LICENCE V3 AND WILL BE OPEN SOURCED WHEN FINISHED
//ANY MODIFICATION OF THIS SKETCH HAS TO BE OPEN SOURCED ASWELL
//
//
//Github: https://github.com/TechniCraft1/Tetris_like_game_UNO
//
//Display: I2C OLED 128x64
//
//IF PARTS OF THE CODE BELOW ARE USED THE ABOVE SECTION MUST BE INCLUDED 
//;
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define SCREEN_WIDTH 128 //Resolution parameters
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Grid dimensions (each cell is 4x4 pixels)
#define CELL_SIZE 4 //6
#define GRID_WIDTH 11 //14
#define GRID_HEIGHT 16 //11
#define SHAPE_SIZE 4 //4

//Speed at which the block moves 1 sqare
#define G_delay 500 //in ms

//Line position
#define line_pos 45

//define Bit array
//NOTE: THIS IS NO MEMORY EFFICIENT AND IS A WORK_IN_PROGRESS
bool grid[GRID_WIDTH][GRID_HEIGHT] = {false};

bool In[4] = {false};



uint8_t lines = 0;

// Current shape
byte currentShape[SHAPE_SIZE][SHAPE_SIZE];
int shapeX = GRID_WIDTH / 2 - 2;
int shapeY = 0;

// Block definitions
const byte shapes[7][SHAPE_SIZE][SHAPE_SIZE] = {
  // SHAPES
  {
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  // 
  {
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
  },
  // 
  {
    {0, 0, 0, 0},
    {1, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
  },
  // 
  {
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0}
  },
  // 
  {
    {0, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
  },
  // 
  {
    {0, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0}
  },
  // 
  {
    {0, 0, 0, 0},
    {0, 0, 1, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0}
  }
};

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  Serial.begin(9600);
  //Init. Text
  display.setTextColor(SSD1306_WHITE); // or BLACK
  display.setTextSize(1); // Default size


  pinMode(2, INPUT_PULLUP); // Left
  pinMode(3, INPUT_PULLUP); // Right
  pinMode(4, INPUT_PULLUP); // Down
  pinMode(5, INPUT_PULLUP); // Rotate

  spawnNewShape();
}

void drawBlock(int x, int y) {
  display.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);
}

void drawBorder() { //Draws the End of the grid on the right
display.drawLine(line_pos, 0, line_pos, 64, SSD1306_WHITE);
}

void drawText() {
  display.setTextSize(1);
  display.setCursor(90, 0);
  display.print("Lines: ");
  int IntWidth = strlen(lines) * 6;
  int x = 120 - IntWidth;
  display.setCursor(x, 20);
  display.print(lines);
}

void drawCurrentShape() {
  for (int y = 0; y < SHAPE_SIZE; y++) {
    for (int x = 0; x < SHAPE_SIZE; x++) {
      if (currentShape[x][y]) {
        drawBlock(shapeX + x, shapeY + y);
      }
    }
  }
}

void drawGrid() {
  display.clearDisplay();
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      if (grid[x][y]) drawBlock(x, y);
    }
  }
  drawCurrentShape();
  drawText();
  drawBorder();
  display.display();
}

bool canMove(int dx, int dy) {
  for (int y = 0; y < SHAPE_SIZE; y++) {
    for (int x = 0; x < SHAPE_SIZE; x++) {
      if (currentShape[x][y]) {
        int newX = shapeX + x + dx;
        int newY = shapeY + y + dy;
        if (newX < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT || grid[newX][newY]) {
          return false;
        }
      }
    }
  }
  return true;
}

void rotateShape() {
  byte temp[SHAPE_SIZE][SHAPE_SIZE];
  for (int y = 0; y < SHAPE_SIZE; y++) {
    for (int x = 0; x < SHAPE_SIZE; x++) {
      temp[x][SHAPE_SIZE - 1 - y] = currentShape[y][x];
    }
  }
  // Check if rotation is valid
  for (int y = 0; y < SHAPE_SIZE; y++) {
    for (int x = 0; x < SHAPE_SIZE; x++) {
      if (temp[x][y]) {
        int newX = shapeX + x;
        int newY = shapeY + y;
        if (newX < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT || grid[newX][newY]) {
          return; // Invalid rotation
        }
      }
    }
  }
  memcpy(currentShape, temp, sizeof(temp));
}

void lockBlock() {
  for (int y = 0; y < SHAPE_SIZE; y++) {
    for (int x = 0; x < SHAPE_SIZE; x++) {
      if (currentShape[x][y]) {
        grid[shapeX + x][shapeY + y] = true;
      }
    }
  }
}

void spawnNewShape() {
  int r = random(0, 70) / 10;
  memcpy(currentShape, shapes[r], sizeof(currentShape));
  shapeX = GRID_WIDTH / 2 - 2; //
  shapeY = 0; //

  // Game over detection
  for (int y = 0; y < SHAPE_SIZE; y++) {  //Scanning grid
    for (int x = 0; x < SHAPE_SIZE; x++) {
      if (currentShape[x][y]) {
        int gx = shapeX + x; 
        int gy = shapeY + y; 
        if (gx < 0 || gx >= GRID_WIDTH || gy >= GRID_HEIGHT || grid[gx][gy]) { //Checking if any Game Over condition is true
      display.setTextSize(2);      // Game over Text size
      display.clearDisplay();
      int textWidth = strlen("game over!") * 6; //Compensating for Text size
      int x = 64 - textWidth;
      display.setCursor(x, 0); // Top-right corner
      display.print(F("GAME OVER!"));
      display.setCursor(x, 20); 
      display.print(F("Score: "));
      display.print(lines * 10);
      display.display();
      Serial.println("game over!");
      for(;;);
        }
      }
    }
  }
}

bool check_line(int y) {
  for (int x = 0; x < GRID_WIDTH; x++) {
    if (grid[x][y] == false) return false;
  }
  return true;
}

void clear_line(int y) {
  for (int row = y; row > 0; row--) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[x][row] = grid[x][row - 1];
    }
  }
  for (int x = 0; x < GRID_WIDTH; x++) {
    grid[x][0] = false;
  }
}

void loop() {
  //Detect Inputs
  for(int tm = 0; tm < G_delay / 10 + 1;){
  
  if (digitalRead(2) == LOW) {In[0] = true;}
  if (digitalRead(3) == LOW) {In[1] = true;}
  if (digitalRead(4) == LOW) {In[2] = true;}
  if (digitalRead(5) == LOW) {In[3] = true;}


  //if (digitalRead(2) == LOW && canMove(-1, 0)) shapeX--;
  //if (digitalRead(3) == LOW && canMove(1, 0)) shapeX++;
  //if (digitalRead(4) == LOW && canMove(0, 1)) shapeY++;
  //if (digitalRead(5) == LOW) 
  //{
  //  rotateShape(); 
  //}
  delay(10);
  tm++;
  }
  //Check if Input flags were activated
  if (In[0] == true && canMove(-1, 0)){ 
  In[0] = false; //reset flag
  shapeX--; 
  //Serial.println("Button 1 pressed");
  }

  if (In[1] == true && canMove(1, 0)){ 
  In[1] = false; //reset flag
  shapeX++; 
  //Serial.println("Button 2 pressed");
  }

  if (In[2] == true && canMove(0, 1)){ 
  In[2] = false; //reset flag
  shapeY++; 
  //Serial.println("Button 3 pressed");
  }

  if (In[3] == true) {
  In[3] = false; //reset flag
  rotateShape(); 
  //Serial.println("Button 4 pressed");
  }
  //Reset Input flags
  //In[0] = false;
  //In[1] = false;
  //In[2] = false;
  //In[3] = false;

  //delay(G_delay); // Gravity Delay (Difficulty)
  if (canMove(0, 1)) {
    shapeY++;
  } else {
    lockBlock();
    spawnNewShape();
  }

  for (int y = 0; y < GRID_HEIGHT; y++) {
    if (check_line(y)) {
      clear_line(y);
      lines++;
    }
  }

  drawGrid();
}

