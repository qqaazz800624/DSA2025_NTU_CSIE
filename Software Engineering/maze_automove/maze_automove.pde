int [][] maze;
int success;
int mazeSize = 30;
int blockSize = 30;
int room = int(random(1, mazeSize - 6));
int room1 = int(random(1, mazeSize - 6));
PImage monster;

int playerX = 1;
int playerY = 1;
int moveInterval = 5;
int lastMoveFrame = 0;

ArrayList<PVector> stack = new ArrayList<PVector>();
ArrayList<PVector> doorPositions = new ArrayList<PVector>();

boolean dfsFinished = false;
boolean hasPassedDoor = false;

void setup() {
  size(900, 900, P3D);
  maze = new int[mazeSize][mazeSize];
  monster = loadImage("小怪物.png");

  initMaze();
  go(1, 1);
  stack.add(new PVector(playerX, playerY));
  noStroke();
}

void draw() {
  background(230,245,216);

  for (int x = 0; x < mazeSize; x++) {
    for (int y = 0; y < mazeSize; y++) {
      switch (maze[x][y]) {
      case 0:
        fill(230,245,216);//background
        break;
      case 1:
        fill(67, 133, 113);//wall
        break;
      case 5:
        fill(165, 229, 237);//room
        break;
      case 6:
        fill(255, 55, 55);
        break; // door
      case 8:
        fill(200);//pass
        break;
      case 9:
        fill(100, 200, 255);//return road
        break;
      default:
        fill(255);//player
        break;
      }
      rect(x * blockSize, y * blockSize, blockSize, blockSize);
    }
  }

  // DFS 移動
  if (!dfsFinished) {
    if (stack.size() == 0) {
      dfsFinished = true;
      return;
    }

    if (frameCount - lastMoveFrame >= moveInterval) {
      lastMoveFrame = frameCount;
      PVector current = stack.get(stack.size() - 1);
      int cx = int(current.x);
      int cy = int(current.y);
      playerX = cx;
      playerY = cy;

      for (PVector door : doorPositions) {
        if (playerX == int(door.x) && playerY == int(door.y)) {
          hasPassedDoor = true;
        }
      }

      if (hasPassedDoor &&
        playerX >= room && playerX < room + 6 &&
        playerY >= room1 && playerY < room1 + 6) {
        dfsFinished = true;
        stack.clear();
        return;
      }

      boolean moved = false;
      int[] dx = {0, 1, 0, -1};
      int[] dy = {-1, 0, 1, 0};
      Integer[] dirs = {0, 1, 2, 3};
      java.util.Collections.shuffle(java.util.Arrays.asList(dirs));

      for (int d = 0; d < 4; d++) {
        int i = dirs[d];
        int nx = playerX + dx[i];
        int ny = playerY + dy[i];

        if (nx >= 0 && ny >= 0 && nx < mazeSize && ny < mazeSize &&
          maze[nx][ny] != 1 && maze[nx][ny] != 8 && maze[nx][ny] != 9) {

          if (maze[nx][ny] == 5 && !hasPassedDoor) {
            continue;
          }

          stack.add(new PVector(nx, ny));
          maze[nx][ny] = 8;
          moved = true;
          break;
        }
      }

      if (!moved) {
        maze[playerX][playerY] = 9;
        stack.remove(stack.size() - 1);
      }
    }
  }

  // 畫角色
  fill(255, 0, 0);
  rect(playerX * blockSize, playerY * blockSize, blockSize, blockSize);

  // 怪物圖片
  imageMode(CENTER);
  image(monster, (room + 3) * blockSize, (room1 + 3) * blockSize, 4 * blockSize, 4 * blockSize);

  // 顯示完成提示
  if (dfsFinished) {
    fill(0);
    textSize(20);
    text("成功進入房間！", 20, 30);
  }
}

void initMaze() {
  for (int x = 0; x < mazeSize; x++) {
    for (int y = 0; y < mazeSize; y += 2) {
      maze[x][y] = 1;
    }
  }
  for (int y = 0; y < mazeSize; y++) {
    for (int x = 0; x < mazeSize; x += 2) {
      maze[x][y] = 1;
    }
  }

  for (int x = room; x < room + 6; x++) {
    for (int y = room1; y < room1 + 6; y++) {
      maze[x][y] = 5;
    }
  }
  for (int x = 1; x < mazeSize - 1; x++) {
    for (int y = 1; y < mazeSize - 1; y++) {
      if (maze[x][y] == 5 &&
          (maze[x-1][y] == 0 || maze[x+1][y] == 0 || maze[x][y-1] == 0 || maze[x][y+1] == 0)) {
        maze[x][y] = 6;
        doorPositions.add(new PVector(x, y));
      }
      if(maze[x][y] == 5 && maze[x][y] != 6 &&
          (maze[x-1][y] == 0 || maze[x+1][y] == 0 || maze[x][y-1] == 0 || maze[x][y+1] == 0))
      {
        maze[x][y] = 1;
      }
    }
  }
  
}

int go(int x, int y) {
  maze[x][y] = 4;
  int number = floor(random(4));

  if ((y > 2) && (number == 0)) {
    if (maze[x][y-2] == 0) {
      maze[x][y-1] = 3;
      go(x, y-2);
    }
  }
  if ((x < mazeSize - 2) && (number == 1)) {
    if (maze[x+2][y] == 0) {
      maze[x+1][y] = 3;
      go(x+2, y);
    }
  }
  if ((y < mazeSize - 2) && (number == 2)) {
    if (maze[x][y+2] == 0) {
      maze[x][y+1] = 3;
      go(x, y+2);
    }
  }
  if ((x > 2) && (number == 3)) {
    if (maze[x-2][y] == 0) {
      maze[x-1][y] = 3;
      go(x-2, y);
    }
  }

  if (y > 2 && maze[x][y-2] == 0) {
    maze[x][y-1] = 3;
    go(x, y-2);
  }
  if (x < mazeSize - 2 && maze[x+2][y] == 0) {
    maze[x+1][y] = 3;
    go(x+2, y);
  }
  if (y < mazeSize - 2 && maze[x][y+2] == 0) {
    maze[x][y+1] = 3;
    go(x, y+2);
  }
  if (x > 2 && maze[x-2][y] == 0) {
    maze[x-1][y] = 3;
    go(x-2, y);
  }
  return success;
}
