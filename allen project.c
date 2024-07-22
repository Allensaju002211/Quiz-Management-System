#include <stdio.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_QUESTIONS 10
#define MAX_TOPICS 5

typedef struct {
    char question[256];
    char options[4][100];
    char correctOption;
} Question;

typedef struct {
    char topic[100];
    Question questions[MAX_QUESTIONS];
    int questionCount;
} Topic;

typedef struct {
    char username[100];
    int scores[MAX_TOPICS];
} User;

User users[MAX_USERS];
Topic topics[MAX_TOPICS];
int userCount = 0;
int topicCount = 0;

void loadTopicsFromFile() {
    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("Error opening questions file.\n");
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') continue; // Skip empty lines
        
        strcpy(topics[topicCount].topic, strtok(line, "\n"));
        
        int qCount = 0;
        while (fgets(line, sizeof(line), file) && line[0] != '\n') {
            strcpy(topics[topicCount].questions[qCount].question, strtok(line, ","));
            for (int i = 0; i < 4; i++) {
                strcpy(topics[topicCount].questions[qCount].options[i], strtok(NULL, ","));
            }
            topics[topicCount].questions[qCount].correctOption = strtok(NULL, "\n")[0];
            qCount++;
        }
        topics[topicCount].questionCount = qCount;
        topicCount++;
    }
    fclose(file);
}

void loadUsersFromFile() {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Error opening users file.\n");
        return;
    }
    
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        strtok(line, "\n");
        strcpy(users[userCount].username, line);
        for (int i = 0; i < MAX_TOPICS; i++) {
            users[userCount].scores[i] = -1; // -1 indicates no score yet
        }
        userCount++;
    }
    fclose(file);
}

void saveUserScoresToFile() {
    FILE *file = fopen("users.txt", "w");
    if (file == NULL) {
        printf("Error opening users file for writing.\n");
        return;
    }
    
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s\n", users[i].username);
        for (int j = 0; j < topicCount; j++) {
            fprintf(file, "%d ", users[i].scores[j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("Maximum user limit reached.\n");
        return;
    }
    printf("Enter username: ");
    scanf("%s", users[userCount].username);
    for (int i = 0; i < MAX_TOPICS; i++) {
        users[userCount].scores[i] = -1; // -1 indicates no score yet
    }
    userCount++;
    printf("User registered successfully!\n");
    saveUserScoresToFile();
}

int loginUser() {
    char username[100];
    printf("Enter username: ");
    scanf("%s", username);
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1; // User not found
}

void displayTopics() {
    printf("Available topics:\n");
    for (int i = 0; i < topicCount; i++) {
        printf("%d. %s\n", i + 1, topics[i].topic);
    }
}

void takeQuiz(int userId) {
    int topicIndex;
    displayTopics();
    printf("Choose a topic by entering the corresponding number: ");
    scanf("%d", &topicIndex);
    topicIndex--; // Adjust for array index

    if (topicIndex < 0 || topicIndex >= topicCount) {
        printf("Invalid topic selection.\n");
        return;
    }

    int score = 0;
    for (int i = 0; i < topics[topicIndex].questionCount; i++) {
        Question q = topics[topicIndex].questions[i];
        printf("%s\n", q.question);
        for (int j = 0; j < 4; j++) {
            printf("%c. %s\n", 'A' + j, q.options[j]);
        }
        char answer;
        printf("Enter your answer: ");
        scanf(" %c", &answer);
        if (answer == q.correctOption) {
            score++;
        }
    }
    users[userId].scores[topicIndex] = score;
    printf("You scored %d out of %d.\n", score, topics[topicIndex].questionCount);
    saveUserScoresToFile();
}

void displayScores(int userId) {
    printf("Scores for %s:\n", users[userId].username);
    for (int i = 0; i < topicCount; i++) {
        if (users[userId].scores[i] == -1) {
            printf("%s: Not attempted\n", topics[i].topic);
        } else {
            printf("%s: %d\n", topics[i].topic, users[userId].scores[i]);
        }
    }
}

int main() {
    loadTopicsFromFile();
    loadUsersFromFile();
    int choice;
    int loggedInUserId = -1;

    while (1) {
        printf("1. Register\n2. Login\n3. Take Quiz\n4. View Scores\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loggedInUserId = loginUser();
                if (loggedInUserId == -1) {
                    printf("Login failed. User not found.\n");
                } else {
                    printf("Login successful.\n");
                }
                break;
            case 3:
                if (loggedInUserId == -1) {
                    printf("Please login first.\n");
                } else {
                    takeQuiz(loggedInUserId);
                }
                break;
            case 4:
                if (loggedInUserId == -1) {
                    printf("Please login first.\n");
                } else {
                    displayScores(loggedInUserId);
                }
                break;
            case 5:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}