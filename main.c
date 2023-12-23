#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define LETTER_COUNT 7
#define MAX_WORD_LENGTH 100

// Function to check if a letter is already in the array
int isLetterInArray(char letter, char *array, int length) {
    for (int i = 0; i < length; i++) {
        if (array[i] == letter) {
            return 1;
        }
    }
    return 0;
}

// Function to generate a set of unique random letters
void generateUniqueLetters(char *letters) {
    int count = 0;
    while (count < LETTER_COUNT) {
        char randomLetter = 'a' + rand() % 26;
        if (!isLetterInArray(randomLetter, letters, count)) {
            letters[count++] = randomLetter;
        }
    }
    letters[LETTER_COUNT] = '\0';
}

// Function to check if a word is in the dictionary
int isWordInDictionary(const char *word, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        return 0;
    }

    char buffer[MAX_WORD_LENGTH];
    while (fgets(buffer, MAX_WORD_LENGTH, file)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
        if (strcasecmp(word, buffer) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

// Function to check if the word is valid
int isValidWord(const char *word, const char *letters, char centralLetter, const char *filename) {
    int length = strlen(word);
    if (length < 4 || strchr(word, centralLetter) == NULL) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!strchr(letters, tolower(word[i]))) {
            return 0;
        }
    }

    return isWordInDictionary(word, filename);
}

// Function to count valid words in the dictionary for a given set of letters
int countValidWordsInDictionary(const char *letters, char centralLetter, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        return 0;
    }

    char word[MAX_WORD_LENGTH];
    int count = 0;
    while (fgets(word, MAX_WORD_LENGTH, file)) {
        word[strcspn(word, "\n")] = 0; // Remove newline character
        if (isValidWord(word, letters, centralLetter, filename)) {
            count++;
        }
    }

    fclose(file);
    return count;
}

// Helper function to compare strings for qsort
int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void sortAndConcatenateGuessedWords(const char *guessedWords, char *sortedGuessedWords) {
    char tempGuessedWords[1000]; // Ensure this is large enough
    strcpy(tempGuessedWords, guessedWords);

    char *words[100];  // Adjust size as needed
    int n = 0;
    char *word = strtok(tempGuessedWords, ", ");
    while (word != NULL) {
        words[n++] = word;
        word = strtok(NULL, ", ");
    }
    qsort(words, n, sizeof(char *), compareStrings);

    strcpy(sortedGuessedWords, "");
    for (int i = 0; i < n; i++) {
        strcat(sortedGuessedWords, words[i]);
        if (i < n - 1) {
            strcat(sortedGuessedWords, ", "); // Ensure space after comma
        }
    }
}


// Function to display the game interface
void displayGameInterface(const char *letters, char centralLetter, int score, const char *guessedWords, const int validWordCount) {
    system("clear"); // Clear the screen (use "cls" on Windows)

    char lettersWithoutCentral[LETTER_COUNT - 1];
    int idx = 0;
    for (int i = 0; i < LETTER_COUNT; ++i) {
        if (letters[i] != centralLetter) {
            lettersWithoutCentral[idx++] = letters[i];
        }
    }

    char sortedGuessedWords[1000];  // Adjust size as needed
    sortAndConcatenateGuessedWords(guessedWords, sortedGuessedWords);

    // Display the letters in a hexagon-like layout
    printf("  %c %c  \n", toupper(lettersWithoutCentral[0]), toupper(lettersWithoutCentral[1]));
    printf(" %c \033[1m%c\033[0m %c \n", toupper(lettersWithoutCentral[2]), toupper(centralLetter), toupper(lettersWithoutCentral[3]));
    printf("  %c %c  \n", toupper(lettersWithoutCentral[4]), toupper(lettersWithoutCentral[5]));
    printf("Score: %d\n", score);
    printf("Guessed Words: %s\n", sortedGuessedWords);
    printf("Total Valid Words: %d\n", validWordCount);
    printf("Enter word (or 'exit' to quit): ");
}

void showAllPossibleWords(const char *letters, char centralLetter, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        return;
    }

    char word[MAX_WORD_LENGTH];
    printf("Possible words:\n");
    while (fgets(word, MAX_WORD_LENGTH, file)) {
        word[strcspn(word, "\n")] = 0; // Remove newline character
        if (isValidWord(word, letters, centralLetter, filename)) {
            printf("%s\n", word);
        }
    }

    fclose(file);
}

int main() {
    srand(time(NULL));

    char letters[LETTER_COUNT + 1];
    char centralLetter;
    int centralLetterIndex;
    int validWordCount = 0;
    do {
        generateUniqueLetters(letters);
        centralLetterIndex = rand() % LETTER_COUNT;
        centralLetter = letters[centralLetterIndex];
        validWordCount = countValidWordsInDictionary(letters, centralLetter, "dictionary.txt");
    } while (validWordCount < 30 || validWordCount > 40);

    char guessedWords[1000] = ""; // Buffer to store guessed words
    int score = 0;

    while (1) {
        displayGameInterface(letters, centralLetter, score, guessedWords, validWordCount);

        char word[MAX_WORD_LENGTH];
        scanf("%99s", word);

        if (strcmp(word, "exit") == 0) {
            break;
        }

        if (strcmp(word, "/cheat") == 0) {
            showAllPossibleWords(letters, centralLetter, "dictionary.txt");
            break; // Exit the game after showing all possible words
        }

        if (isValidWord(word, letters, centralLetter, "dictionary.txt")) {
            int points = strlen(word);
            score += points;
            strcat(guessedWords, word);
            strcat(guessedWords, ", "); // Add the word to guessed words list
        } else {
            printf("Invalid word.\n");
            sleep(2); // Pause for a moment before refreshing the interface
        }
    }

    printf("Final Score: %d\n", score);
    return 0;
}
