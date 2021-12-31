#include "wasm4.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_PIECES 16
#define BOARD_WIDTH 6
#define BOARD_HEIGHT 6

typedef enum {H, V} orientation;
typedef enum {L, R, U, D} direction;
typedef enum {ok, err} outcome;

typedef struct {
    int8_t       x;
    int8_t       y;
} Point ;

typedef struct {
    uint8_t       x;
    uint8_t       y;
} Pixel ;

typedef struct {
    orientation    o;
    uint8_t       length;
    Point         topLeft;
} Piece;

Piece pieces[MAX_PIECES];

uint8_t spaceSize = (140 / BOARD_WIDTH) - 2;
uint8_t numPiecesInStage;
uint8_t currentPiece = 0;

uint8_t previousGamepad;

Point goal = {6, 2};

bool hasVictory = false;

const uint8_t smiley[] = {
    0b11000011,
    0b10000001,
    0b00100100,
    0b00100100,
    0b00000000,
    0b00100100,
    0b10011001,
    0b11000011,
};

outcome placePieces() {
    numPiecesInStage = 2;

    pieces[0].topLeft.x = 0;
    pieces[0].topLeft.y = 2;
    pieces[0].o = H;
    pieces[0].length = 2;

    pieces[1].topLeft.x = 3;
    pieces[1].topLeft.y = 0;
    pieces[1].o = V;
    pieces[1].length = 3;

    return ok;
}

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

  Pixel p = boardToPixel(goal.x, goal.y);
  rect(p.x, p.y, spaceSize, spaceSize);

  return ok;
}

outcome drawPieces() {
  for (int i=0; i<numPiecesInStage; i++) {
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
  }
  return ok;

}

bool isSpaceEdge(Point point) {
  if (point.x == goal.x && point.y == goal.y) {
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
  Piece player = pieces[0];
  int x = player.topLeft.x;
  int y = player.topLeft.y;
  for (int i=0; i<player.length; i++) {
    if (x == goal.x && y == goal.y) {
      return true;
    }
    if (player.o == H) { x++; }
    else { y++; }

  }
  return false;

}


void start () {
    placePieces();
    /* Point x = {3,2}; */
    /* if (isSpaceOccupied(x)) { */
    /*   tracef("%d,%d is occupied", x.x, x.y); */
    /* } else { */
    /*   tracef("%d,%d is not occupied", x.x, x.y); */
    /* } */
}

void update () {
    uint8_t gamepad = *GAMEPAD1;
    uint8_t pressedThisFrame = gamepad & (gamepad ^ previousGamepad);
    previousGamepad = gamepad;

    drawBoard();
    drawPieces();

    *DRAW_COLORS = 2;
    if (currentPiece == 0) {
      text("0", 30, 0);
    } else if (currentPiece == 1) {
      text("1", 30, 0);
    } else {
      text("Piece beyond 1 selected", 30, 0);
    }

    if (pressedThisFrame & BUTTON_RIGHT) {
      move(R);
    } else if (pressedThisFrame & BUTTON_LEFT) {
      move(L);
    } else if (pressedThisFrame & BUTTON_UP) {
      move(U);
    } else if (pressedThisFrame & BUTTON_DOWN) {
      move(D);
    } else if (pressedThisFrame & BUTTON_1) {
      currentPiece = (currentPiece + 1) % numPiecesInStage;
    }
    *DRAW_COLORS = 4;
    /* tone(260, 5, 10, TONE_PULSE1); */
    /* tracef("%d %d", pieces[0].topLeft.x, pieces[0].topLeft.y); */

    /* blit(smiley, 76, 76, 8, 8, BLIT_1BPP); */
    /* text("Press X to blink", 16, 90); */
    if (checkVictory()) {
      text("YAS QUEEN", 50, 60);
      if (!hasVictory) {
        tone(262 | (523 << 16), 60, 100, TONE_PULSE1);
      }
      hasVictory = true;     
    }
}
