#include "MazeGenerator.h"

int main(){
  // Defining our maze object
  int width = 8;
  int height = 8;
  int maze[width * height];
  int i;

  make_maze(maze, width, height);
  print_maze(maze, width, height);

  return 0;
}
