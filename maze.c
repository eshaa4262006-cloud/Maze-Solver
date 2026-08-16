#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXH 31
#define MAXW 31

#define WALL '#'
#define SPACE ' '
#define PLAYER '@'
#define TRAIL '.'

typedef struct { int r, c; } Point;

int dr2[4] = {-2, 2, 0, 0};
int dc2[4] = {0, 0, -2, 2};

int in_bounds(int r, int c, int H, int W) {
    return r >= 0 && r < H && c >= 0 && c < W;
}

void shuffle4(int a[4]) {
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

void init_maze(char maze[MAXH][MAXW], int H, int W) {
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++)
            maze[r][c] = WALL;
}

void carve_maze_dfs(char maze[MAXH][MAXW], int H, int W, int sr, int sc) {
    Point stack[MAXH * MAXW];
    int top = 0;

    maze[sr][sc] = SPACE;
    stack[top++] = (Point){sr, sc};

    while (top > 0) {
        Point cur = stack[top - 1];

        int dirs[4] = {0,1,2,3};
        shuffle4(dirs);

        int carved = 0;
        for (int k = 0; k < 4; k++) {
            int d = dirs[k];
            int nr = cur.r + dr2[d];
            int nc = cur.c + dc2[d];

            if (!in_bounds(nr, nc, H, W)) continue;
            if (maze[nr][nc] != WALL) continue;

            int wr = (cur.r + nr) / 2;
            int wc = (cur.c + nc) / 2;

            maze[wr][wc] = SPACE;
            maze[nr][nc] = SPACE;

            stack[top++] = (Point){nr, nc};
            carved = 1;
            break;
        }
        if (!carved) top--;
    }
}

int can_move_to(char maze[MAXH][MAXW], int r, int c, int H, int W) {
    if (!in_bounds(r, c, H, W)) return 0;
    return maze[r][c] != WALL;
}

void print_maze(char maze[MAXH][MAXW], int H, int W, Point S, Point E, Point P) {
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            if (r == P.r && c == P.c) putchar(PLAYER);
            else if (r == S.r && c == S.c) putchar('S');
            else if (r == E.r && c == E.c) putchar('E');
            else putchar(maze[r][c]);
        }
        putchar('\n');
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    int H = 9; // odd
    int W = 11; // odd

    char maze[MAXH][MAXW];
    init_maze(maze, H, W);
    carve_maze_dfs(maze, H, W, 1, 1);

    Point S = {1, 1};
    Point E = {H - 2, W - 2};
    maze[S.r][S.c] = SPACE;
    maze[E.r][E.c] = SPACE;

    Point player = S;
    int steps = 0;

    while (1) {
        printf("\n\n=== Maze (steps=%d, player=(%d,%d)) ===\n", steps, player.r, player.c);
        print_maze(maze, H, W, S, E, player);

        if (player.r == E.r && player.c == E.c) {
            printf("\nCongratulations!!You reached the destination in %d steps!\n", steps);
            break;
        }

        printf("\nW = up\nS = down\nA = left\nD = right\nQ = quit\nMove: W/A/S/D then Enter (Q to quit): ");

        char cmd;
        if (scanf(" %c", &cmd) != 1) continue;

        int nr = player.r, nc = player.c;
        if (cmd == 'w' || cmd == 'W') nr--;
        else if (cmd == 's' || cmd == 'S') nr++;
        else if (cmd == 'a' || cmd == 'A') nc--;
        else if (cmd == 'd' || cmd == 'D') nc++;
        else if (cmd == 'q' || cmd == 'Q') break;
        else continue;

        if (can_move_to(maze, nr, nc, H, W)) {
            if (maze[player.r][player.c] == SPACE) maze[player.r][player.c] = TRAIL;
            player.r = nr; player.c = nc;
            steps++;
        } else {
            printf("Blocked by a wall.\n");
        }
    }

    return 0;
}

