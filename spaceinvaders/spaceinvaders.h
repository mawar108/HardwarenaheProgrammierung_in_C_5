// Deklarieren Sie hier Ihre Funktionen, struct(s) und enum(s).
#include <stdio.h>
#include <stdlib.h>

typedef enum Einheiten
{
    space = 0,
    alien = 1,
    alien_Boss = 2,
    player = 3,
    enemy_laser = 4,
    player_laser = 5,
    explosion = 6,
    wall = 6
} Einheiten;

typedef struct Map
{
    char movement; //0 = right or 1 = left
    int downCounter;
    size_t column, lines;
    size_t totalenemies;
    // Raumschiff **field;
    Einheiten **einheit;
    size_t playerXPosition;
    size_t playerYPosition;
    int palyerlifes;
    int Bosslifes;
} Map;

void put_raumschiff(Map *map, char c, size_t counter_colums, size_t counter_lines);
FILE* checkInput(char *path, Map *map);
void loadMap(FILE *fp, Map *map);
void getFileAndDiff(int argc, char *argv[], char **filename, char **difficulty);
void updateDirection(Map *map);
void moveLeftorRight(Map *map, size_t i, size_t j, size_t x);
void moveEnemies(Map *map);
void printEinheit(Einheiten einheit);
void printMap(Map *map);
int checkforSpace(Map *map, size_t x, size_t y);
void enemyshoot(Map *map);
void setLives(Map *map, char *difficulty);
void playershoot(Map *map);
void movePlayer(Map *map, char c);
void deleteMap(Map *map);



// typedef struct Raumschiff
// {
//     int lifes;
//     Einheiten einheit;
//     char test;
//     int movement; //0 = right or 1 = left
//     int down;     // goes down or not 1/0

// } Raumschiff;


