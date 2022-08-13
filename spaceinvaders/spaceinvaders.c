#include "spaceinvaders.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>

void put_raumschiff(Map *map, char c, size_t counter_colums, size_t counter_lines){
    if(c == 'O'){
        map->einheit[counter_lines][counter_colums] = alien_Boss;
    }else if(c == 'M'){
        map->einheit[counter_lines][counter_colums] = alien;
        map->totalenemies += 1;
     } else if(c == 'A' ){
        map->einheit[counter_lines][counter_colums] = player;
        map->playerXPosition = counter_lines;
        map->playerYPosition = counter_colums;
     }else if(c == '#'){
        map->einheit[counter_lines][counter_colums] = wall;
    }else{
        map->einheit[counter_lines][counter_colums] = space;
    }

}

FILE* checkInput(char *path, Map *map){
    FILE *fp = NULL;
    fp = fopen(path, "r");
    

    if (fp == NULL){
        fprintf(stderr, "Die Datei %s konnt nicht geöffnet werden!  2 \n", path);
        exit(2);
    }

    char errortag = getc(fp);

    if (ferror(fp)){
        fclose(fp);
        fprintf(stderr, "Datei konnte nicht gelesen werden %s %c 3 \n", path, errortag);
        //clearerr(fp);
        exit(3);
    } else{
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);

        if (0 == size){
            fclose(fp);
            fprintf(stderr, "Die Datei %s ist leer! 4 \n", path);
            exit(4);
        }
    }
    //clearerr(fp);
    fseek(fp, 0, SEEK_SET);

    size_t counter_colums = 0;
    size_t counter_lines = 0;

    char c;
    while ((c = fgetc(fp)) != EOF){
        counter_colums++;
        if (c == '\n'){
            counter_lines++;
        }
    }
    map->column = (counter_colums / counter_lines);
    map->lines = counter_lines;

    fseek(fp, 0, SEEK_SET);
    
    return fp;
}

void loadMap(FILE *fp, Map *map){
    size_t counterX = 0;
    size_t counterY = 0;
    char b;
    while ((b = fgetc(fp)) != EOF){
        put_raumschiff(map, b, counterY, counterX);
        counterY++;
        if (b == '\n'){
            counterX++;
            counterY = 0;
        }
    }
    fclose(fp);
}


void getFileAndDiff(int argc, char *argv[], char **filename, char **difficulty){
    int c;
    int checkFile = 0;
    int checkDiff = 0;

    while (1){
        struct option long_options[] =
            {
                {"level", required_argument, 0, 'a'},
                {"difficulty", required_argument, 0, 'b'},
                {0, 0, 0, 0}};

        int option_index = 0;

        c = getopt_long_only(argc, argv, "a:b:",
                             long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c){
        case 'a':
            if(checkFile == 0){
                *filename = optarg;
                checkFile = 1;
            } else{
                fprintf(stderr, "Zu oft --level eingeben");
                exit(1);
            }
            break;
        case 'b':
            if(checkDiff == 0){
                *difficulty = optarg;
                checkDiff = 1;
            }else{
                fprintf(stderr, "Zu oft --difficulty eingeben");
                exit(1);
            }
            break;
        case '?':
            fprintf(stderr, "Unbekannter Befehl! 1 \n");
            exit(1);
            break;

        default:
            fprintf(stderr, "Irgendwas ist falsch :(  1\n");
            exit(1);
            break;
        }
    }
}

void updateDirection(Map *map){
    for(size_t x = 1; x < map->lines; x++){
        if(map->einheit[x][1] == alien || map->einheit[x][1] == alien_Boss){
            map->movement= 'r';
            map->downCounter = map->downCounter +1;
            break;
        }
        else if (map->einheit[x][map->column-3] == alien 
        || map->einheit[x][map->column-3] == alien_Boss){
            map->movement= 'l';
            break;
        }
    }
}


void moveLeftorRight(Map *map, size_t i, size_t j , size_t x){
    if (map->einheit[i][j + x ] == alien){
        map->einheit[i][j + x] = space;
        if (map->einheit[i][j] == player_laser){
            map->einheit[i][j] = space;
            map->totalenemies -= 1;
        } else if (map->einheit[i][j] == player) {
            map->totalenemies -= 1;
            map->palyerlifes -=1;
        } else{
            map->einheit[i][j] = alien;
        }
    }
    else if (map->einheit[i][j + x] == alien_Boss){
        map->einheit[i][j + x] = space;
        if (map->einheit[i][j] == player_laser){
            map->einheit[i][j] = alien_Boss;
            map->einheit[i][j + x] = space;
            map->Bosslifes -= 1;
        } else if (map->einheit[i][j] == player){
            map->totalenemies -= 1;
            map->palyerlifes -= 1;
        }else{
            map->einheit[i][j] = alien_Boss;
        }
    }
}




void moveEnemies(Map *map){
    if(map->movement == 'r'){
        for (size_t i = map->lines - 2; i > 0 ; i--){
            for(size_t j = map->column - 3; j > 0; j--){ 
                if ((map->downCounter % 3) == 0 && map->einheit[i][j] != wall 
                && map->einheit[i][j] != alien){
                    if (map->einheit[i-1][j] == alien){
                        if (map->einheit[i][j] == player){
                            map->einheit[i - 1][j] = space;
                            map->palyerlifes -= 1;
                        }else if (map->einheit[i][j] == player_laser){ 
                            map->einheit[i - 1][j] = space;
                        } else{
                            map->einheit[i][j] = map->einheit[i - 1][j];
                            map->einheit[i - 1][j] = space;
                        }      
                    }
                } else if((map->downCounter % 3) == 0 && map->einheit[i+1][j] == wall 
                && map->einheit[i][j] == alien){
                    map->palyerlifes = 0;
                }else{
                    moveLeftorRight(map,i, j, -1);
                }  
            }
        }
    } else {
        for (size_t i = 1; i < map->lines-1; i++){
            for (size_t j = 1; j < map->column-1; j++){
                moveLeftorRight(map,i , j,  1);
            }
        }
    }
}

void printEinheit(Einheiten einheit){
    if(einheit == alien_Boss){
        printf("O");
    }
    if (einheit == player){
        printf("A");
    }
    if (einheit == space){
        printf(" ");
    }
    if (einheit == alien){
        printf("M");
    }
    if (einheit == wall){
        printf("#");
    }
    if (einheit == enemy_laser){
        printf("U");
    }
    if (einheit == player_laser){
        printf("^");
    }
}

void printMap(Map *map){
    for (size_t x = 0; x < map->lines; x++){
        for (size_t y = 0; y < map->column; y++){
            printEinheit(map->einheit[x][y]);
        }
        printf("\n");
    }
}

int checkforSpace(Map *map, size_t x, size_t y){
    int rw = 1;
    for(size_t i = x+1; i < map->lines; i++){
        if(map->einheit[i][y] == alien && rw == 1){
            rw = 0;
        } 
    }
    return rw;
}

void playershoot(Map *map){
    for (size_t i = 1; i < map->lines - 1; i++){
        for (size_t j = 1; j < map->column - 1; j++){
            if (map->einheit[i][j] == player_laser) {
                map->einheit[i][j] = space;
                if (map->einheit[i - 1][j] == space) {
                    map->einheit[i - 1][j] = player_laser;
                } else if (map->einheit[i - 1][j] == alien){
                      map->einheit[i - 1][j] = space;
                      map->totalenemies -= 1;
                  } else if(map->einheit[i - 1][j] == alien_Boss){
                      map->Bosslifes -= 1;
                      if(map->Bosslifes <= 0){
                          for (size_t k = map->column - 1; k > 0; k--){
                              if (map->einheit [i-1][k] == alien_Boss){
                                  map->einheit[i - 1][k] = space;
                              }
                          }
                      }
                  } else if (map->einheit[i - 1][j] == enemy_laser){
                      map->einheit[i - 1][j] = space;
                  }
            }
        }    
    }
}



void enemyshoot(Map *map){
    srand(time(NULL));
    for (size_t i = map->lines - 1; i > 0 ; i--){
        for (size_t j = map->column - 1; j > 0; j--){
            if ((map->einheit[i][j] == alien || map->einheit[i][j] == alien_Boss) 
            && (rand() % 15) > 13 && checkforSpace(map, i,j) == 1 && map->einheit[i+1][j]!= wall ){
                if (map->einheit[i + 1][j] == player){
                    map->palyerlifes -= 1;
                    map->totalenemies -= 1; 
                }else{
                    map->einheit[i + 1][j] = enemy_laser;
                }
            }else if (map->einheit[i][j] == enemy_laser){
                map->einheit[i][j] = space;
                if (map->einheit[i + 1][j] != wall && map->einheit[i + 1][j] != player 
                && map->einheit[i + 1][j] != alien){
                    map->einheit[i + 1][j] = enemy_laser;
                }
                else if (map->einheit[i + 1][j] == player)
                {
                    map->palyerlifes -=1;
                }
            }
        }    
    }
}    

void setLives(Map *map, char *difficulty){
    if (strcmp(difficulty, "easy") == 0){
        map->Bosslifes = 2;
        map->palyerlifes = 3;
    }else
    if (strcmp(difficulty, "normal") == 0){
        map->Bosslifes = 3;
        map->palyerlifes = 2;
    }else
    if (strcmp(difficulty, "hard") == 0){
        map->Bosslifes = 4;
        map->palyerlifes = 1;
    }else{
        fprintf(stderr, "Falsche Difficulty %s\n", difficulty);
        exit(1);
    }
}


void movePlayer(Map *map, char c){
    if (c == 'a'){
        if (map->einheit[map->playerXPosition][map->playerYPosition - 1] != wall){
            if (map->einheit[map->playerXPosition][map->playerYPosition - 1] == enemy_laser){
                map->palyerlifes -=1;
            }
            map->einheit[map->playerXPosition][map->playerYPosition] = space;
            map->playerYPosition -= 1;
            map->einheit[map->playerXPosition][map->playerYPosition] = player;

        }
    } else if (c == 'd'){
        if (map->einheit[map->playerXPosition][map->playerYPosition + 1] != wall){
            if (map->einheit[map->playerXPosition][map->playerYPosition + 1] == enemy_laser){
                map->palyerlifes -=1;
            }
            map->einheit[map->playerXPosition][map->playerYPosition] = space;
            map->playerYPosition += 1;
            map->einheit[map->playerXPosition][map->playerYPosition] = player;
        }
    } else if (c == 'w'){
        size_t x = map->playerXPosition;
        size_t y = map->playerYPosition;
        if (map->einheit[x - 1][y] == alien){
            map->einheit[x - 1][y] = space;
            map->totalenemies -= 1;
        }else if (map->einheit[x - 1][y] == space ){
            map->einheit[x - 1][y] = player_laser;
        }else if (map->einheit[x - 1][y] == enemy_laser){
            map->einheit[x - 1][y] = space;
        }
    } else if (c == 's'){
        //bleibt an der positon stehen
    } else {
        deleteMap(map);
        fprintf(stderr, "Falscher Befehl %c 1 \n", c);
        exit(1);
    }
}

void deleteMap(Map *map){
    for(size_t x= 0; x < map->lines; x++ ){
        free(map->einheit[x]);
    }
    free(map->einheit);
}



int main(int argc, char *argv[]) {
    Map map;
    char *filename = "level/1.txt";
    char *difficulty= "easy";
    getFileAndDiff(argc, argv, &filename, &difficulty);
    setLives(&map, difficulty);
    FILE *fp= checkInput(filename, &map);
    
    map.totalenemies = 0;
    map.downCounter = 1;
    map.movement = 'r';

    map.einheit = malloc(sizeof(Einheiten *) * map.lines);
    for (size_t i = 0; i < map.lines; i++){
        map.einheit[i] = malloc(sizeof(Einheiten) * (map.column));
    }

    loadMap(fp, &map);
    //map = loadmap(filename);
    
    char c;
    printMap(&map);
    
    while (1){
        c = getc(stdin);
        if(c == 'q'){
            deleteMap(&map);
            fprintf(stderr, "Spiel beendet \n");
            exit(0);
        }else if(c != '\n'){
            updateDirection(&map);
            moveEnemies(&map);
            playershoot(&map);
            movePlayer(&map, c);
            enemyshoot(&map);
            printMap(&map);
            
        }
        if(map.palyerlifes == 0){
            deleteMap(&map);
            fprintf(stderr, "Verloren :( 0 \n");
            exit(0);
        } else if(map.Bosslifes == 0 && map.totalenemies == 0){
            deleteMap(&map);
            fprintf(stderr, "Gewonnen yay  0 \n");
            exit(0);
        }   
    }
    return 0;
}
