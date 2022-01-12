#include <stdio.h>
#include <string.h>
#include "parking.h"

outcome levelA(Level *level) {
    level->numPiecesInStage = 3;

    level->pieces[0].topLeft.x = 0;
    level->pieces[0].topLeft.y = 2;
    level->pieces[0].o = H;
    level->pieces[0].length = 2;

    level->pieces[1].topLeft.x = 0;
    level->pieces[1].topLeft.y = 3;
    level->pieces[1].o = H;
    level->pieces[1].length = 2;

    level->pieces[2].topLeft.x = 0;
    level->pieces[2].topLeft.y = 1;
    level->pieces[2].o = H;
    level->pieces[2].length = 3;


    level->goal.x = 6;
    level->goal.y = 2;
    
    strcpy(level->victoryPhrase, "Done\n shopping");
    strcpy(level->topPhrase, "Leave the lot");

    return ok;
 
}


outcome level0(Level *level) {
    level->numPiecesInStage = 2;

    level->pieces[0].topLeft.x = 0;
    level->pieces[0].topLeft.y = 2;
    level->pieces[0].o = H;
    level->pieces[0].length = 2;

    level->pieces[1].topLeft.x = 2;
    level->pieces[1].topLeft.y = 1;
    level->pieces[1].o = V;
    level->pieces[1].length = 3;

    level->goal.x = 6;
    level->goal.y = 2;
    
    strcpy(level->victoryPhrase, "Are you free?");
    strcpy(level->topPhrase, "Learn you a game");

    return ok;
 
}

outcome level1(Level *level) {
    level->numPiecesInStage = 4;

    level->pieces[0].topLeft.x = 0;
    level->pieces[0].topLeft.y = 2;
    level->pieces[0].o = H;
    level->pieces[0].length = 2;

    level->pieces[1].topLeft.x = 3;
    level->pieces[1].topLeft.y = 0;
    level->pieces[1].o = V;
    level->pieces[1].length = 3;

    level->pieces[2].topLeft.x = 3;
    level->pieces[2].topLeft.y = 4;
    level->pieces[2].o = H;
    level->pieces[2].length = 3;

    level->pieces[3].topLeft.x = 0;
    level->pieces[3].topLeft.y = 3;
    level->pieces[3].o = V;
    level->pieces[3].length = 2;

    level->numPiecesInStage = 4;

    level->goal.x = 6;
    level->goal.y = 2;
    
    strcpy(level->victoryPhrase, "YAS QUEEN");
    strcpy(level->topPhrase, "Escape Parking Hell");

    return ok;
}

outcome level2(Level *level) {
    level->numPiecesInStage = 4;

    level->pieces[0].topLeft.x = 0;
    level->pieces[0].topLeft.y = 2;
    level->pieces[0].o = H;
    level->pieces[0].length = 2;

    level->pieces[1].topLeft.x = 5;
    level->pieces[1].topLeft.y = 0;
    level->pieces[1].o = V;
    level->pieces[1].length = 3;

    level->pieces[2].topLeft.x = 3;
    level->pieces[2].topLeft.y = 5;
    level->pieces[2].o = H;
    level->pieces[2].length = 3;

    level->pieces[3].topLeft.x = 2;
    level->pieces[3].topLeft.y = 4;
    level->pieces[3].o = V;
    level->pieces[3].length = 2;

    level->numPiecesInStage = 4;

    level->goal.x = 6;
    level->goal.y = 2;
    
    strcpy(level->victoryPhrase, "GET IT");
    strcpy(level->topPhrase, "Keep Going");

    return ok;
}
