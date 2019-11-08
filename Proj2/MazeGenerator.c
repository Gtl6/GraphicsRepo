// A program to generate a maze
#include "MazeGenerator.h"

// Prints out the data structure underneath the maze
void print_gross_maze(int *maze, int width, int height){
  int i, j;
  for(i = 0; i < height; i++){
    for(j = 0; j < width; j++){
      printf("%d ", maze[i * width + j]);
    }
    printf("\n");
  }
}

// Prints a representation of the maze to the output console
void print_maze(int *maze, int width, int height){
  int x;
  int y;
  int wallcount = 0;

  char top[2 * width + 2];
  char mid[2 * width + 2];

  for(y = 0; y < height; y++){
    for(x = 0; x < width; x++){
      int box = maze[y * width + x];
      // build the top row
      top[2 * x] = '+';
      if(box % 2 == 0) top[2 * x + 1] = ' ';
      else {
        top[2 * x + 1] = '-';
        wallcount++;
      }

      // build the mid row
      if(box % 7 == 0) mid[2 * x] = ' ';
      else {
        mid[2 * x] = '|';
        wallcount ++;
      }

      mid[2 * x + 1] = ' ';
    }

    top[2 * width] = '+';
    top[2 * width + 1] = 0;
    mid[2 * width] = '|';
    wallcount++;
    mid[2 * width + 1] = 0;
    printf("%s\n%s\n", top, mid);
  }

  for(x = 0; x < width; x++){
    top[2*x] = '+';
    top[2*x + 1] = '-';
    wallcount++;
  }

  printf("%s\n", top);
  printf("We found %d walls\n", wallcount);
}

// Takes in a maze and coordinates, and returns the value of the node above the one specified
//  If given is y = 0, returns -1
int up(int *maze, int width, int height, int x, int y){
  if(y == 0) return -1;
  else return maze[width * (y-1) + x];
}

// Takes in a maze and coordinates, and returns the value of the node below the one specified
//  If given is the bottom, returns -1
int down(int *maze, int width, int height, int x, int y){
  if(y == (height - 1)) return -1;
  else return maze[width * (y+1) + x];
}

// Takes in a maze and coordinates, and returns the value of the node left of the one specified
//  If given is x = 0, returns -1
int left(int *maze, int width, int height, int x, int y){
  if(x == 0) return -1;
  else return maze[width * y + (x - 1)];
}

// Takes in a maze and coordinates, and returns the value of the node right of the one specified
//  If given is the right side, returns -1
int right(int *maze, int width, int height, int x, int y){
  if(x == width - 1) return -1;
  else return maze[width * y + (x + 1)];
}

// The recursive function that generates a perfect maze, using a recursive backtracking algorithm
void make_maze_r(int *maze, int width, int height, int x, int y){
  // We're at point startx, starty
  // Base case, can't move in any direction
  int upn = up(maze, width, height, x, y);
  int downn = down(maze, width, height, x, y);
  int rightn = right(maze, width, height, x, y);
  int leftn = left(maze, width, height, x, y);

  while(upn == 1 || downn == 1 || rightn == 1 || leftn == 1){
    // Else carve into a new random direction
    int dirs[4];
    int count = 0;

    // 2 is up
    if(upn == 1)
      dirs[count++] = 2;

    // 3 is down
    if(downn == 1)
      dirs[count++] = 3;

    // 5 is right
    if(rightn == 1)
      dirs[count++] = 5;

    // 7 is left
    if(leftn == 1)
      dirs[count++] = 7;

    // I used prime numbers so that the result stored in each cell tells which directions the path goes
    // 2 is up, 3 is down, 6 (2 * 3) is up and down, 10 is up and right, etc
    int rando = rand();
    int which = rando % count;
    int dir = dirs[which];
    maze[width * y + x] *= dir;

    switch(dir){
      // Go up and change the above to a down
      case 2:
        maze[width * (y - 1) + x] *= 3;
        make_maze_r(maze, width, height, x, y - 1);
        break;
      // Go down and change the below to an up
      case 3:
        maze[width * (y + 1) + x] *= 2;
        make_maze_r(maze, width, height, x, y + 1);
        break;
      // Go right and change the right one to a left
      case 5:
        maze[width * y + (x + 1)] *= 7;
        make_maze_r(maze, width, height, x + 1, y);
        break;
      // Go left
      case 7:
        maze[width * y + (x - 1)] *= 5;
        make_maze_r(maze, width, height, x - 1, y);
        break;
      // Should be unreachable
      default:
        printf("Somehow got to the unreachable part of the maze generator\n");
  }

  upn = up(maze, width, height, x, y);
  downn = down(maze, width, height, x, y);
  rightn = right(maze, width, height, x, y);
  leftn = left(maze, width, height, x, y);
  }

  return;
}

// Just the initializer for the above recursive function
void make_maze(int *maze, int width, int height){
  int i;
  // Fill our maze with 1's
  for(i = 0; i < width * height; i++) maze[i] = 1;
  srand(time(0));
  return make_maze_r(maze, width, height, 0, 0);
}
