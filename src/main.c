#include "wasm4.h"
#include "splash.h"
#include "parking.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_PIECES 16
#define BOARD_WIDTH 6
#define BOARD_HEIGHT 6
#define ARROW_FLASH_DURATION 6
#define NUMBER_OF_LEVELS 4

const uint8_t arrow[] = {
    0b10000000,
    0b11000000,
    0b11100000,
    0b11110000,
    0b11111000,
    0b11110000,
    0b11100000,
    0b11000000,
    0b10000000,
};

Level level;  // The current level

outcome (*levels[NUMBER_OF_LEVELS]) (Level *level);

// Framebuffer is 160x160, we reserve 10 pixels margin 
// so the board itself is 140 pixels across
uint8_t spaceSize = (140 / BOARD_WIDTH) - 2;
uint8_t currentPiece = 0;
uint8_t previousGamepad;
uint8_t gamepad;
uint8_t pressedThisFrame;
bool hasVictory = false;
bool inLevelSelect = true;

int leftPressed = 0;
int rightPressed = 0;
int levelSelectCounter = 0;

Pixel boardToPixel(int x, int y) {
  Pixel pixel;
  pixel.x = 10+(2*(x+1))+(x*spaceSize);
  pixel.y = 10+(2*(y+1))+(y*spaceSize);
  return pixel;
}

outcome drawBoard() { 
  *DRAW_COLORS = 2;
  rect(10, 10, 140, 140);
  
  *DRAW_COLORS = 3;
  for (int i=0; i<BOARD_WIDTH; i++) {
     for (int j=0; j<BOARD_HEIGHT; j++) {
       Pixel p = boardToPixel(i, j);
       rect(p.x, p.y, spaceSize, spaceSize);
     }
  }

  Pixel p = boardToPixel(level.goal.x, 
                         level.goal.y);
  rect(p.x, p.y, spaceSize, spaceSize);

  return ok;
}

outcome drawPieces() {
  Piece *pieces = level.pieces;

  bool selected;
  for (int i=0; i<level.numPiecesInStage; i++) {
    selected = (i == currentPiece);
    Piece piece = pieces[i];
    if (i == 0) {
      *DRAW_COLORS = 4;
    } else {
      *DRAW_COLORS = 1;
    }
    Pixel p = boardToPixel(piece.topLeft.x, piece.topLeft.y);
    if (piece.o == H) {
      rect(p.x + 1, p.y + 1, 
           spaceSize * piece.length - 2 , spaceSize - 2);
    } else {
      rect(p.x + 1, p.y + 1, 
           spaceSize - 2 , spaceSize * piece.length - 2);
    }

    if (selected) {
      *DRAW_COLORS = 0x20;
      int arrowLen = spaceSize * piece.length - 10;
      if (piece.o == H) {
        /* line(p.x + 5, p.y + spaceSize/2, p.x + arrowLen, p.y + spaceSize/2); */
        rect(p.x + 3, 
             p.y + 3, 
             spaceSize * piece.length - 6,
             spaceSize - 6); 
        rect(p.x + 5, 
             p.y + 5, 
             spaceSize * piece.length - 10,
             spaceSize - 10); 
        rect(p.x + 7, 
             p.y + 7, 
             spaceSize * piece.length - 14,
             spaceSize - 14); 
      } else {
        /* line(p.x + spaceSize/2, p.y + 5, p.x + spaceSize/2, p.y + arrowLen); */
        rect(p.x + 3, 
             p.y + 3, 
             spaceSize - 6, 
             spaceSize * piece.length - 6); 
        rect(p.x + 5, 
             p.y + 5, 
             spaceSize - 10, 
             spaceSize * piece.length - 10); 
        rect(p.x + 7, 
             p.y + 7, 
             spaceSize - 14, 
             spaceSize * piece.length - 14); 
      }
    }
  }
  return ok;

}

bool isSpaceEdge(Point point) {
  if (point.x == level.goal.x && point.y == level.goal.y) {
    return false;
  }
  if (point.x < 0 || point.y < 0) {
    return true;
  } else if (point.x >= BOARD_WIDTH || point.y >= BOARD_HEIGHT) {
    return true;
  } else {
    return false;
  }
}

bool isSpaceOccupied(Point point) {
  Piece *pieces = level.pieces;

  for (int i=0; i<MAX_PIECES; i++) {
    Piece piece = pieces[i];
    int x, y;
    x = piece.topLeft.x;
    y = piece.topLeft.y;
    for (int j=0; j<piece.length; j++) {
      if (x == point.x && y == point.y) {
        tracef("%d, %d is occupied", x, y);
        return true;
      }   
      if (piece.o == H) { x++; }
      else { y++; }
    }
  }
  return false;
}

bool isLegalMove(direction d) {
  Piece *pieces = level.pieces;

  // Check that it's not a strafe
  Piece movePiece = pieces[currentPiece];
  if ((d == L || d == R) && movePiece.o == V) {
    return false;
  } else if ((d == D || d == U) && movePiece.o == H) {
    return false;
  }

  Point p;
  if (d == R) {
    p.x = movePiece.topLeft.x + movePiece.length;
    p.y = movePiece.topLeft.y;
  } else if (d == L) {
    p.x = movePiece.topLeft.x - 1;
    p.y = movePiece.topLeft.y;
  } else if (d == D) {
    p.x = movePiece.topLeft.x;
    p.y = movePiece.topLeft.y + movePiece.length;
  } else {   // else if d == U
    p.x = movePiece.topLeft.x;
    p.y = movePiece.topLeft.y - 1;
  } 
  return !isSpaceOccupied(p) && !isSpaceEdge(p);

}

outcome move(direction d) {
  Piece *pieces = level.pieces;

  if (!isLegalMove(d)) {
    tone(100, 5, 10, TONE_PULSE1);
    return err;
  }
  if (d == U) {
    pieces[currentPiece].topLeft.y--;
  } else if (d == D) { 
    pieces[currentPiece].topLeft.y++;
  } else if (d == R) { 
    pieces[currentPiece].topLeft.x++;
  } else if (d == L) { 
    pieces[currentPiece].topLeft.x--;
  }

  tone(260, 5, 10, TONE_PULSE1);

  return ok;
}

bool checkVictory() {
  Piece *pieces = level.pieces;

  Piece player = pieces[0];
  int x = player.topLeft.x;
  int y = player.topLeft.y;
  for (int i=0; i<player.length; i++) {
    if (x == level.goal.x && y == level.goal.y) {
      return true;
    }
    if (player.o == H) { x++; }
    else { y++; }

  }
  return false;

}

int framesToDate = 1;


void splashScreen() {
  blit(splash, 0, 0, 160, 160, BLIT_2BPP);
  text("Parking Puzzle", 10, 10);
  text("Ada & \nMax 2022", 90, 110);
  if (framesToDate == 10) {
    tone(200 | (500 << 16), 60, 60, TONE_NOISE);
  }
}

void start () {
    levels[0] = levelA;
    levels[1] = level0;
    levels[2] = level1;
    levels[3] = level2;
    (*levels[0])(&level);
}

void game() {
     PALETTE[0] = 0xe0f8cf;
     PALETTE[1] = 0x86c06c;
     PALETTE[2] = 0x306850; 
     PALETTE[3] = 0x71821;
      
    /* uint8_t gamepad = *GAMEPAD1; */
    /* uint8_t pressedThisFrame = gamepad & (gamepad ^ previousGamepad); */
    /* previousGamepad = gamepad; */

    drawBoard();
    drawPieces();

    *DRAW_COLORS = 2;
    text(level.topPhrase, 5, 2);

    if (pressedThisFrame & BUTTON_RIGHT) {
      move(R);
    } else if (pressedThisFrame & BUTTON_LEFT) {
      move(L);
    } else if (pressedThisFrame & BUTTON_UP) {
      move(U);
    } else if (pressedThisFrame & BUTTON_DOWN) {
      move(D);
    } else if (pressedThisFrame & BUTTON_1) {
      currentPiece = (currentPiece + 1) % level.numPiecesInStage;
    } else if (pressedThisFrame & BUTTON_2) {
      if (currentPiece == 0) {
        currentPiece = level.numPiecesInStage - 1;
      } else {
        currentPiece = currentPiece - 1;
      }
    }
    *DRAW_COLORS = 4;

    if (checkVictory()) {
      text(level.victoryPhrase, 50, 60);
      if (!hasVictory) {
        tone(262 | (523 << 16), 60, 100, TONE_PULSE1);
      }
      hasVictory = true;     
    }

}

void levelSelect() {
    // Experiment with using a paler color palette in level select
    PALETTE[0] = 0xF1FFE6;
    PALETTE[1] = 0xBBDAAE;
    PALETTE[2] = 0x6D8078;
    PALETTE[3] = 0x2F373B;
    
    drawBoard();
    drawPieces();

    if (pressedThisFrame & BUTTON_RIGHT) {
      rightPressed = ARROW_FLASH_DURATION;
      levelSelectCounter = (levelSelectCounter + 1) % NUMBER_OF_LEVELS;
      (*levels[levelSelectCounter]) (&level);
    } else if (pressedThisFrame & BUTTON_LEFT) {
      leftPressed = ARROW_FLASH_DURATION;
      if (levelSelectCounter == 0) {
        levelSelectCounter = NUMBER_OF_LEVELS - 1;
      } else {
        levelSelectCounter--;
      }
      (*levels[levelSelectCounter]) (&level);
    } else if (pressedThisFrame & BUTTON_1) {
      inLevelSelect = false;
    }

  
    uint16_t unpressed = 0x0040;
    uint16_t pressed = 0x4000;

    if (rightPressed > 0) {
      *DRAW_COLORS = pressed;
      rightPressed--;
    } else {
      *DRAW_COLORS = unpressed;
    }
    blit(arrow, 152, 75, 8, 9, BLIT_1BPP);

    char levelSelectText[80]; 
    levelSelectText[0]= 0x30 + levelSelectCounter + 1;
    levelSelectText[1]= 0x0;
    
    *DRAW_COLORS = 0x0004;
    text(levelSelectText, 12, 2);
    text("Select Level", 52, 2);
    text("(Arrow pick, x go)", 12, 152);

    if (leftPressed > 0) {
      *DRAW_COLORS = pressed;
      leftPressed--;
    } else {
      *DRAW_COLORS = unpressed;
    }
    blit(arrow, 0, 75, 8, 9, BLIT_1BPP | BLIT_FLIP_X);
}

void update () {
  gamepad = *GAMEPAD1;
  pressedThisFrame = gamepad & (gamepad ^ previousGamepad);
  previousGamepad = gamepad;
  if (framesToDate >=0 && framesToDate < 180) {
    splashScreen();
    framesToDate++;
  } else if (inLevelSelect) {
    framesToDate = -1;
    levelSelect();
  } else {
    game();
  }
}
