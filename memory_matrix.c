#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MATRIX_SIZE 4
#define MAX_ATTEMPTS 8

typedef struct {
    int value;
    int* address_ref;
    int revealed;
} Cell;

typedef struct {
    Cell** matrix;
    int size;
    int score;
    int attempts;
} GameBoard;

void clearScreen() {
    printf("\033[H\033[J");
}

void printMatrix(GameBoard* board, int showAddresses) {
    printf("\n   ");
    for (int col = 0; col < board->size; col++) {
        printf("   %d  ", col);
    }
    printf("\n");
    
    for (int row = 0; row < board->size; row++) {
        printf(" %d ", row);
        for (int col = 0; col < board->size; col++) {
            Cell* cell = &board->matrix[row][col];
            
            if (cell->revealed) {
                printf(" [%2d] ", cell->value);
            } else if (showAddresses) {
                printf(" %p ", (void*)cell->address_ref);
            } else {
                printf(" [??] ");
            }
        }
        printf("\n");
    }
}

void initializeGame(GameBoard* board) {
    board->size = MATRIX_SIZE;
    board->score = 0;
    board->attempts = MAX_ATTEMPTS;
    
    // Allocate matrix
    board->matrix = malloc(board->size * sizeof(Cell*));
    for (int i = 0; i < board->size; i++) {
        board->matrix[i] = malloc(board->size * sizeof(Cell));
    }
    
    // Initialize cells with random values and pointer references
    srand(time(NULL));
    int values[MATRIX_SIZE * MATRIX_SIZE];
    
    // Create pairs of values
    for (int i = 0; i < (board->size * board->size) / 2; i++) {
        values[2 * i] = 10 + rand() % 90;
        values[2 * i + 1] = values[2 * i];
    }
    
    // Shuffle values
    for (int i = 0; i < board->size * board->size; i++) {
        int j = rand() % (board->size * board->size);
        int temp = values[i];
        values[i] = values[j];
        values[j] = temp;
    }
    
    // Assign values and addresses
    int index = 0;
    for (int row = 0; row < board->size; row++) {
        for (int col = 0; col < board->size; col++) {
            board->matrix[row][col].value = values[index++];
            board->matrix[row][col].address_ref = &board->matrix[row][col].value;
            board->matrix[row][col].revealed = 0;
        }
    }
}

int makeGuess(GameBoard* board, int row1, int col1, int row2, int col2) {
    if (row1 < 0 || row1 >= board->size || col1 < 0 || col1 >= board->size ||
        row2 < 0 || row2 >= board->size || col2 < 0 || col2 >= board->size) {
        printf("Invalid coordinates! Try again.\n");
        return 0;
    }
    
    if (row1 == row2 && col1 == col2) {
        printf("Cannot select the same cell twice!\n");
        return 0;
    }
    
    Cell* cell1 = &board->matrix[row1][col1];
    Cell* cell2 = &board->matrix[row2][col2];
    
    // Temporarily reveal the cells
    cell1->revealed = 1;
    cell2->revealed = 1;
    
    clearScreen();
    printf("\n=== MEMORY MATRIX CHALLENGE ===\n");
    printMatrix(board, 0);
    
    printf("\nCell 1 [%d,%d]: Value = %d, Address = %p\n", 
           row1, col1, cell1->value, (void*)cell1->address_ref);
    printf("Cell 2 [%d,%d]: Value = %d, Address = %p\n", 
           row2, col2, cell2->value, (void*)cell2->address_ref);
    
    // Check if the pointer addresses match the values
    if (cell1->value == cell2->value) {
        printf("\n✅ MATCH FOUND! Values are equal.\n");
        
        // Extra points if addresses are also related
        if (cell1->address_ref == &cell1->value && cell2->address_ref == &cell2->value) {
            printf("✅ Pointer addresses correctly reference their own values!\n");
            board->score += 20;
        } else {
            board->score += 10;
        }
        
        // Keep cells revealed
        cell1->revealed = 1;
        cell2->revealed = 1;
        return 1;
    } else {
        printf("\n❌ NO MATCH. Values are different.\n");
        
        // Hide cells again after delay
        printf("Hiding cells in 3 seconds...\n");
        sleep(3);
        cell1->revealed = 0;
        cell2->revealed = 0;
        
        board->attempts--;
        return 0;
    }
}

int checkGameOver(GameBoard* board) {
    // Check if all cells are revealed
    for (int row = 0; row < board->size; row++) {
        for (int col = 0; col < board->size; col++) {
            if (!board->matrix[row][col].revealed) {
                return 0;
            }
        }
    }
    return 1;
}

void freeGameBoard(GameBoard* board) {
    for (int i = 0; i < board->size; i++) {
        free(board->matrix[i]);
    }
    free(board->matrix);
}

void printInstructions() {
    clearScreen();
    printf("=== MEMORY MATRIX CHALLENGE ===\n");
    printf("\nHOW TO PLAY:\n");
    printf("1. Match pairs of cells with the same value\n");
    printf("2. Each cell shows a pointer address when hidden\n");
    printf("3. Enter coordinates of two cells to reveal them\n");
    printf("4. If values match, cells stay revealed\n");
    printf("5. Bonus points for correct pointer references\n");
    printf("\nSCORING:\n");
    printf("- Match found: +10 points\n");
    printf.- Correct pointer reference: +10 bonus points\n");
    printf("- Max attempts: %d\n", MAX_ATTEMPTS);
    printf("\nGOOD LUCK!\n");
    printf("\nPress Enter to continue...");
    getchar();
}

int main() {
    GameBoard game;
    int row1, col1, row2, col2;
    
    printInstructions();
    
    initializeGame(&game);
    
    while (game.attempts > 0 && !checkGameOver(&game)) {
        clearScreen();
        printf("\n=== MEMORY MATRIX CHALLENGE ===\n");
        printf("Score: %d | Attempts remaining: %d\n", game.score, game.attempts);
        
        printf("\nCurrent Board (hidden values):\n");
        printMatrix(&game, 1);  // Show addresses
        
        printf("\nEnter first cell coordinates (row col): ");
        scanf("%d %d", &row1, &col1);
        
        printf("Enter second cell coordinates (row col): ");
        scanf("%d %d", &row2, &col2);
        
        makeGuess(&game, row1, col1, row2, col2);
        
        if (!checkGameOver(&game) && game.attempts > 0) {
            printf("\nPress Enter to continue...");
            getchar();
            getchar();
        }
    }
    
    clearScreen();
    printf("\n=== GAME OVER ===\n");
    
    if (checkGameOver(&game)) {
        printf("\n🎉 CONGRATULATIONS! You've revealed all cells!\n");
    } else {
        printf("\n😔 Out of attempts! Better luck next time.\n");
    }
    
    printf("\nFinal Score: %d\n", game.score);
    printf("\nFinal Board:\n");
    printMatrix(&game, 0);
    
    // Show pointer analysis
    printf("\n=== POINTER ANALYSIS ===\n");
    for (int row = 0; row < game.size; row++) {
        for (int col = 0; col < game.size; col++) {
            Cell* cell = &game.matrix[row][col];
            printf("Cell [%d,%d]: Value=%d, Addr=%p, Ref=%p %s\n",
                   row, col, cell->value, (void*)&cell->value, 
                   (void*)cell->address_ref,
                   (cell->address_ref == &cell->value) ? "(Correct)" : "");
        }
    }
    
    freeGameBoard(&game);
    
    printf("\nThanks for playing Memory Matrix Challenge!\n");
    return 0;
}