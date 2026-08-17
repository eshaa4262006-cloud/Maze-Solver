/*
 * =====================================================================
 *  TERMINAL MAZE RUNNER  (portable, login + persistent scoreboard)
 * =====================================================================
 *  - Login / register with a username + password (stored in users.txt)
 *  - Generates a brand new random maze every run (recursive backtracker)
 *  - Move with W A S D + ENTER, quit with Q + ENTER
 *  - Your BEST score (fewest moves, then fastest time) is kept in
 *    scores.txt and only gets overwritten if you beat it
 *  - Leaderboard shows every player's best run
 *
 *  Compile:
 *      gcc maze_game.c -o maze_game
 *  Run:
 *      ./maze_game
 *
 *  No termios / conio / raw-terminal tricks -- plain fgets() line
 *  input, so this compiles and runs in any standard C environment,
 *  including online compilers / code-block runners.
 *
 *  NOTE: passwords are stored in plain text in users.txt for
 *  simplicity. This is a terminal game demo, not a security example --
 *  don't reuse a real password here.
 * =====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------------------------------------------------------------
 *  CONFIG  
 * --------------------------------------------------------------- */
#define ROWS 5                  /* maze cells vertically   */
#define COLS 7                  /* maze cells horizontally */
#define GRID_H (2 * ROWS + 1)   /* actual printable grid   */
#define GRID_W (2 * COLS + 1)

#define MAX_USERS  200
#define MAX_SCORES 200
#define NAME_LEN   30

#define USERS_FILE "users.txt"
#define SCORE_FILE "scores.txt"

/* ---------------------------------------------------------------
 *  STRUCTURES
 * --------------------------------------------------------------- */
typedef struct {
    int x, y;
} Point;

typedef struct {
    char grid[GRID_H][GRID_W];   /* '#' = wall, ' ' = open path */
    int  visited[ROWS][COLS];    /* used only during generation */
} Maze;

typedef struct {
    Point pos;
    int   moves;
} Player;

typedef struct {
    char username[NAME_LEN];
    char password[NAME_LEN];
} User;

typedef struct {
    char   username[NAME_LEN];
    int    bestMoves;
    double bestSeconds;
} ScoreEntry;

/* ---------------------------------------------------------------
 *  SMALL STRING HELPER
 * --------------------------------------------------------------- */
void readLine(char *buf, int size) {
    if (fgets(buf, size, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
    } else {
        buf[0] = '\0';
    }
}

/* ---------------------------------------------------------------
 *  LOGIN / REGISTER SYSTEM   (file: users.txt "username password")
 * --------------------------------------------------------------- */
int loadUsers(User list[]) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) return 0;

    int count = 0;
    while (count < MAX_USERS &&
           fscanf(fp, "%29s %29s", list[count].username,
                  list[count].password) == 2) {
        count++;
    }
    fclose(fp);
    return count;
}

int findUser(User list[], int count, const char *username) {
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(list[i].username, username) == 0) return i;
    }
    return -1;
}

int registerUser(char *outUsername) {
    User users[MAX_USERS];
    int count = loadUsers(users);

    char username[NAME_LEN], password[NAME_LEN];
    printf("Choose a username: ");
    readLine(username, NAME_LEN);

    if (findUser(users, count, username) != -1) {
        printf("That username is already taken.\n");
        return 0;
    }
    if (strlen(username) == 0) {
        printf("Username can't be empty.\n");
        return 0;
    }

    printf("Choose a password: ");
    readLine(password, NAME_LEN);

    FILE *fp = fopen(USERS_FILE, "a");
    if (!fp) {
        printf("Could not save account.\n");
        return 0;
    }
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);

    strcpy(outUsername, username);
    printf("Account created. Welcome, %s!\n", username);
    return 1;
}

int loginUser(char *outUsername) {
    User users[MAX_USERS];
    int count = loadUsers(users);

    char username[NAME_LEN], password[NAME_LEN];
    printf("Username: ");
    readLine(username, NAME_LEN);
    printf("Password: ");
    readLine(password, NAME_LEN);

    int idx = findUser(users, count, username);
    if (idx == -1 || strcmp(users[idx].password, password) != 0) {
        printf("Incorrect username or password.\n");
        return 0;
    }

    strcpy(outUsername, username);
    printf("Logged in as %s.\n", username);
    return 1;
}

void authenticate(char *outUsername) {
    int authenticated = 0;
    int attempts = 0;

    while (!authenticated && attempts < 5) {
        printf("\n--- MAZE RUNNER LOGIN ---\n");
        printf("1) Login\n2) Register\nChoice: ");
        char choice[8];
        readLine(choice, sizeof(choice));

        if (strcmp(choice, "1") == 0) {
            authenticated = loginUser(outUsername);
        } else if (strcmp(choice, "2") == 0) {
            authenticated = registerUser(outUsername);
        } else {
            printf("Please enter 1 or 2.\n");
        }
        attempts++;
    }

    if (!authenticated) {
        printf("Too many failed attempts. Goodbye.\n");
        exit(0);
    }
}

/* ---------------------------------------------------------------
 *  MAZE GENERATION  (iterative recursive-backtracker / DFS)
 * --------------------------------------------------------------- */
void initMaze(Maze *m) {
    int r, c;
    for (r = 0; r < GRID_H; r++)
        for (c = 0; c < GRID_W; c++)
            m->grid[r][c] = '#';

    for (r = 0; r < ROWS; r++)
        for (c = 0; c < COLS; c++)
            m->visited[r][c] = 0;
}

void shuffleDirections(int order[4]) {
    int i, j, tmp;
    for (i = 0; i < 4; i++) order[i] = i;
    for (i = 3; i > 0; i--) {
        j = rand() % (i + 1);
        tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }
}

void generateMaze(Maze *m) {
    int dx[4] = {0, 0, -1, 1};   /* up, down, left, right (cell space) */
    int dy[4] = {-1, 1, 0, 0};

    Point stack[ROWS * COLS];
    int top = 0;

    Point start = {0, 0};
    stack[top++] = start;
    m->visited[start.y][start.x] = 1;
    m->grid[2 * start.y + 1][2 * start.x + 1] = ' ';

    while (top > 0) {
        Point cur = stack[top - 1];
        int order[4];
        shuffleDirections(order);

        int found = 0;
        int k;
        for (k = 0; k < 4 && !found; k++) {
            int dir = order[k];
            int nx = cur.x + dx[dir];
            int ny = cur.y + dy[dir];

            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS &&
                !m->visited[ny][nx]) {

                int wallX = 2 * cur.x + 1 + dx[dir];
                int wallY = 2 * cur.y + 1 + dy[dir];
                m->grid[wallY][wallX] = ' ';
                m->grid[2 * ny + 1][2 * nx + 1] = ' ';

                m->visited[ny][nx] = 1;
                stack[top].x = nx;
                stack[top].y = ny;
                top++;
                found = 1;
            }
        }

        if (!found) top--;   /* dead end, backtrack */
    }
}

/* ---------------------------------------------------------------
 *  RENDERING
 * --------------------------------------------------------------- */
void printMaze(Maze *m, Player *p, Point goal, int elapsedSec) {
    printf("\n============================================\n");
    printf("Moves: %d   Time: %ds   |   Move: W A S D + Enter   Quit: Q\nW = up\nS = down\nA = left\nD = right\n\n", p->moves, elapsedSec);

    int r, c;
    for (r = 0; r < GRID_H; r++) {
        for (c = 0; c < GRID_W; c++) {
            if (r == p->pos.y && c == p->pos.x)
                putchar('P');
            else if (r == goal.y && c == goal.x)
                putchar('G');
            else
                putchar(m->grid[r][c]);
        }
        putchar('\n');
    }
}

/* ---------------------------------------------------------------
 *  FILE I/O  -  PERSISTENT PER-USER SCOREBOARD
 * --------------------------------------------------------------- */
int loadScores(ScoreEntry list[]) {
    FILE *fp = fopen(SCORE_FILE, "r");
    if (!fp) return 0;

    int count = 0;
    while (count < MAX_SCORES &&
           fscanf(fp, "%29s %d %lf", list[count].username,
                  &list[count].bestMoves, &list[count].bestSeconds) == 3) {
        count++;
    }
    fclose(fp);
    return count;
}

void saveAllScores(ScoreEntry list[], int count) {
    FILE *fp = fopen(SCORE_FILE, "w");   /* rewrite whole file */
    if (!fp) {
        printf("Could not save scoreboard.\n");
        return;
    }
    int i;
    for (i = 0; i < count; i++) {
        fprintf(fp, "%s %d %.2f\n", list[i].username,
                list[i].bestMoves, list[i].bestSeconds);
    }
    fclose(fp);
}

/* Update this user's record only if the new run is better
   (fewer moves wins; time is the tiebreaker). Returns 1 if
   this run became the user's new best. */
int updateScore(const char *username, int moves, double seconds) {
    ScoreEntry list[MAX_SCORES];
    int count = loadScores(list);

    int idx = -1, i;
    for (i = 0; i < count; i++) {
        if (strcmp(list[i].username, username) == 0) {
            idx = i;
            break;
        }
    }

    int isNewBest = 0;
    if (idx == -1) {
        strcpy(list[count].username, username);
        list[count].bestMoves = moves;
        list[count].bestSeconds = seconds;
        count++;
        isNewBest = 1;
    } else if (moves < list[idx].bestMoves ||
               (moves == list[idx].bestMoves && seconds < list[idx].bestSeconds)) {
        list[idx].bestMoves = moves;
        list[idx].bestSeconds = seconds;
        isNewBest = 1;
    }

    saveAllScores(list, count);
    return isNewBest;
}

void sortScores(ScoreEntry list[], int count) {
    /* selection sort: fewer moves = better rank, time is tiebreaker */
    int i, j, best;
    for (i = 0; i < count - 1; i++) {
        best = i;
        for (j = i + 1; j < count; j++) {
            if (list[j].bestMoves < list[best].bestMoves ||
                (list[j].bestMoves == list[best].bestMoves &&
                 list[j].bestSeconds < list[best].bestSeconds)) {
                best = j;
            }
        }
        if (best != i) {
            ScoreEntry tmp = list[i];
            list[i] = list[best];
            list[best] = tmp;
        }
    }
}

void showScoreboard(void) {
    ScoreEntry list[MAX_SCORES];
    int count = loadScores(list);

    printf("\n================ LEADERBOARD ================\n");
    if (count == 0) {
        printf("No scores yet. Be the first to finish a maze!\n");
    } else {
        sortScores(list, count);
        printf("%-4s %-15s %-8s %-8s\n", "#", "USERNAME", "MOVES", "TIME(s)");
        int i, limit = count < 10 ? count : 10;
        for (i = 0; i < limit; i++) {
            printf("%-4d %-15s %-8d %-8.2f\n",
                   i + 1, list[i].username, list[i].bestMoves, list[i].bestSeconds);
        }
    }
    printf("===============================================\n");
}

/* ---------------------------------------------------------------
 *  GAME LOOP
 * --------------------------------------------------------------- */
int isWalkable(Maze *m, int x, int y) {
    if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H) return 0;
    return m->grid[y][x] != '#';
}

void playRound(const char *username) {
    srand((unsigned int)time(NULL));

    Maze maze;
    initMaze(&maze);
    generateMaze(&maze);

    Player player;
    player.pos.x = 1;
    player.pos.y = 1;
    player.moves = 0;

    Point goal;
    goal.x = 2 * (COLS - 1) + 1;
    goal.y = 2 * (ROWS - 1) + 1;

    time_t startTime = time(NULL);
    int running = 1;
    int won = 0;

    while (running) {
        int elapsed = (int)difftime(time(NULL), startTime);
        printMaze(&maze, &player, goal, elapsed);

        if (player.pos.x == goal.x && player.pos.y == goal.y) {
            won = 1;
            break;
        }

        printf("Your move: ");
        fflush(stdout);

        char key = 0;
        char buf[16];
        readLine(buf, sizeof(buf));
        if (strlen(buf) > 0) key = buf[0];

        int nx = player.pos.x;
        int ny = player.pos.y;

        switch (key) {
            case 'w': case 'W': ny--; break;
            case 's': case 'S': ny++; break;
            case 'a': case 'A': nx--; break;
            case 'd': case 'D': nx++; break;
            case 'q': case 'Q': running = 0; break;
            default:
                printf("Unrecognized key. Use w/a/s/d/q.\n");
                break;
        }

        if (running && isWalkable(&maze, nx, ny)) {
            player.pos.x = nx;
            player.pos.y = ny;
            player.moves++;
        } else if (running && key != 0) {
            printf("There's a wall there!\n");
        }
    }

    if (won) {
        double totalTime = difftime(time(NULL), startTime);
        printf("\n*** YOU REACHED THE GOAL, %s! ***\n", username);
        printf("Moves: %d   Time: %.2fs\n", player.moves, totalTime);

        if (updateScore(username, player.moves, totalTime)) {
            printf("New personal best! Scoreboard updated.\n");
        } else {
            printf("Good run, but it didn't beat your personal best.\n");
        }
    } else {
        printf("\nThanks for playing, %s. Maybe next time!\n", username);
    }

    showScoreboard();
}

/* ---------------------------------------------------------------
 *  MAIN
 * --------------------------------------------------------------- */
int main(void) {
    char username[NAME_LEN];
    authenticate(username);

    int playing = 1;
    while (playing) {
        playRound(username);

        printf("\nPlay again, %s? (y/n): ", username);
        char again[8];
        readLine(again, sizeof(again));
        if (!(again[0] == 'y' || again[0] == 'Y')) {
            playing = 0;
        }
    }

    printf("Goodbye, %s!\n", username);
    return 0;
}
