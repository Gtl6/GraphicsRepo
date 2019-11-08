#include "MazeGenerator.h"

int main(){
  // Defining our maze object
  int width = 3;
  int height = 3;
  int maze[width * height];
  int i;
  // Fill our maze with 1's
  for(i = 0; i < width * height; i++) maze[i] = 1;

  make_maze(maze, width, height);
  print_maze(maze, width, height);

  return 0;
}
