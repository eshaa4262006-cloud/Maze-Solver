#include <stdio.h>

#define ROWS 5
#define COLS 10

char maze[ROWS][COLS + 1] = {
    "##########",
    "# P      #",
    "# ### ## #",
    "#    E   #",
    "##########"
};

int main()
{
    int playerRow = 1;
    int playerCol = 1;
    char move;

    while (1)
    {
        int i;

        // Print the maze
        printf("\n\n");
        for (i = 0; i < ROWS; i++)
        {
            printf("%s\n", maze[i]);
        }

        printf("\nMove (U L D R): ");
        scanf(" %c", &move);

        int newRow = playerRow;
        int newCol = playerCol;

        // Decide where to move
        if (move == 'u' || move == 'U')
            newRow--;
        else if (move == 'd' || move == 'D')
            newRow++;
        else if (move == 'l' || move == 'L')
            newCol--;
        else if (move == 'r' || move == 'R')
            newCol++;
        else
            continue;

        // Wall check
        if (maze[newRow][newCol] == '#')
            continue;

        // Win check
        if (maze[newRow][newCol] == 'E')
        {
            maze[playerRow][playerCol] = ' ';
            maze[newRow][newCol] = 'P';

            printf("\n");
            for (i = 0; i < ROWS; i++)
                printf("%s\n", maze[i]);

            printf("\n Congratulations! You escaped the maze! HAVE A KITKAT!\n");
            break;
        }

        // Move player
        maze[playerRow][playerCol] = ' ';
        maze[newRow][newCol] = 'P';

        playerRow = newRow;
        playerCol = newCol;
    }

    return 0;
}

