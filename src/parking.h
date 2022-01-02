
typedef enum {H, V} orientation;
typedef enum {L, R, U, D} direction;
typedef enum {ok, err} outcome;

typedef struct {
    int8_t       x;
    int8_t       y;
} Point;

typedef struct {
    uint8_t       x;
    uint8_t       y;
} Pixel;

typedef struct {
    orientation    o;
    uint8_t       length;
    Point         topLeft;
} Piece;

typedef struct {
    Piece         pieces[16];
    uint8_t       numPiecesInStage;
    Point         goal;
    char          victoryPhrase[64];
    char          topPhrase[64];
} Level;

outcome level1(Level *);
