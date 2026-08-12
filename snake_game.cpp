#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

using namespace std;

#define MAX_LENGTH 1000

const char DIR_UP = 'U';
const char DIR_DOWN = 'D';
const char DIR_LEFT = 'L';
const char DIR_RIGHT = 'R';

const int GAME_WIDTH = 50;
const int GAME_HEIGHT = 20;
const int GAME_LEFT = 5;
const int GAME_TOP = 3;

enum MoveResult {
    DEAD,
    MOVED,
    ATE
};

struct Point {
    int xCoord;
    int yCoord;

    Point() {}

    Point(int x, int y) {
        xCoord = x;
        yCoord = y;
    }
};

#ifndef _WIN32

termios originalTerminal;

void setupTerminal() {
    tcgetattr(STDIN_FILENO, &originalTerminal);

    termios newTerminal = originalTerminal;

    newTerminal.c_lflag &= ~(ICANON | ECHO);
    newTerminal.c_cc[VMIN] = 0;
    newTerminal.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal);
}

void restoreTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminal);
}

#endif

void hideCursor() {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);

    cursorInfo.bVisible = false;

    SetConsoleCursorInfo(out, &cursorInfo);
#else
    cout << "\033[?25l";
    cout.flush();
#endif
}

void showCursor() {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);

    cursorInfo.bVisible = true;

    SetConsoleCursorInfo(out, &cursorInfo);
#else
    cout << "\033[?25h";
    cout.flush();
#endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32

    COORD coord;

    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;

    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        coord
    );

#else

    printf("\033[%d;%dH", y, x);
    fflush(stdout);

#endif
}

#ifdef _WIN32

bool kbhitWrapper() {
    return _kbhit();
}

int getchWrapper() {
    return _getch();
}

#else

bool kbhitWrapper() {
    int bytesWaiting = 0;

    ioctl(
        STDIN_FILENO,
        FIONREAD,
        &bytesWaiting
    );

    return bytesWaiting > 0;
}

int getchWrapper() {
    char ch;

    if (read(STDIN_FILENO, &ch, 1) == 1)
        return ch;

    return -1;
}

#endif

class Snake {

    int length;
    char direction;

public:

    Point body[MAX_LENGTH];

    Snake(int x, int y, char initialDirection) {

        length = 1;

        body[0] = Point(x, y);

        direction = initialDirection;
    }

    int getLength() {
        return length;
    }

    void changeDirection(char newDirection) {

        if (newDirection == DIR_UP &&
            direction != DIR_DOWN) {

            direction = newDirection;
        }

        else if (newDirection == DIR_DOWN &&
                 direction != DIR_UP) {

            direction = newDirection;
        }

        else if (newDirection == DIR_LEFT &&
                 direction != DIR_RIGHT) {

            direction = newDirection;
        }

        else if (newDirection == DIR_RIGHT &&
                 direction != DIR_LEFT) {

            direction = newDirection;
        }
    }

    MoveResult move(Point food, Point &oldTail) {

        oldTail = body[length - 1];

        for (int i = length - 1; i > 0; i--) {
            body[i] = body[i - 1];
        }

        switch (direction) {

            case DIR_UP:
                body[0].yCoord--;
                break;

            case DIR_DOWN:
                body[0].yCoord++;
                break;

            case DIR_LEFT:
                body[0].xCoord--;
                break;

            case DIR_RIGHT:
                body[0].xCoord++;
                break;
        }

        for (int i = 1; i < length; i++) {

            if (body[0].xCoord == body[i].xCoord &&
                body[0].yCoord == body[i].yCoord) {

                return DEAD;
            }
        }

        if (food.xCoord == body[0].xCoord &&
            food.yCoord == body[0].yCoord) {

            if (length < MAX_LENGTH) {
                body[length] = oldTail;
                length++;
            }

            return ATE;
        }

        return MOVED;
    }
};

class Board {

    Snake *snake1;
    Snake *snake2;

    const char SNAKE1_BODY = 'O';
    const char SNAKE2_BODY = 'X';

    Point food;

    int score1;
    int score2;

    int highScore;

    char fruitChar;

    vector<char> fruits = {
        '@',
        '*',
        '$',
        '&',
        '%',
        '+'
    };

    int loser;

public:

    Board() {

        snake1 = new Snake(
            GAME_LEFT + GAME_WIDTH / 4,
            GAME_TOP + GAME_HEIGHT / 2,
            DIR_RIGHT
        );

        snake2 = new Snake(
            GAME_LEFT + (3 * GAME_WIDTH) / 4,
            GAME_TOP + GAME_HEIGHT / 2,
            DIR_LEFT
        );

        score1 = 0;
        score2 = 0;

        highScore = 0;

        loser = 0;

        loadHighScore();

        hideCursor();

        drawBorder();

        spawnFood();
    }

    ~Board() {

        delete snake1;
        delete snake2;
    }

    void loadHighScore() {

        ifstream file("highscore.txt");

        if (file.is_open()) {
            file >> highScore;
            file.close();
        }
    }

    void saveHighScore() {

        int currentBest = max(score1, score2);

        if (currentBest > highScore) {

            highScore = currentBest;

            ofstream file("highscore.txt");

            if (file.is_open()) {

                file << highScore;

                file.close();
            }
        }
    }

    bool samePoint(Point a, Point b) {

        return a.xCoord == b.xCoord &&
               a.yCoord == b.yCoord;
    }

    bool pointOnSnake(Point point, Snake *snake) {

        for (int i = 0;
             i < snake->getLength();
             i++) {

            if (samePoint(point, snake->body[i]))
                return true;
        }

        return false;
    }

    void drawBorder() {

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        for (int x = GAME_LEFT;
             x <= GAME_LEFT + GAME_WIDTH;
             x++) {

            gotoxy(x, GAME_TOP);
            cout << "#";

            gotoxy(
                x,
                GAME_TOP + GAME_HEIGHT
            );

            cout << "#";
        }

        for (int y = GAME_TOP;
             y <= GAME_TOP + GAME_HEIGHT;
             y++) {

            gotoxy(GAME_LEFT, y);
            cout << "#";

            gotoxy(
                GAME_LEFT + GAME_WIDTH,
                y
            );

            cout << "#";
        }

        displayCurrentScore();
    }

    void spawnFood() {

        while (true) {

            int x =
                GAME_LEFT + 1 +
                rand() % (GAME_WIDTH - 2);

            int y =
                GAME_TOP + 1 +
                rand() % (GAME_HEIGHT - 2);

            Point newFood(x, y);

            if (!pointOnSnake(newFood, snake1) &&
                !pointOnSnake(newFood, snake2)) {

                food = newFood;

                fruitChar =
                    fruits[rand() % fruits.size()];

                gotoxy(
                    food.xCoord,
                    food.yCoord
                );

                cout << fruitChar;

                cout.flush();

                break;
            }
        }
    }

    void displayCurrentScore() {

        gotoxy(
            GAME_LEFT,
            GAME_TOP - 1
        );

        cout << "Player 1: "
             << score1
             << "    Player 2: "
             << score2
             << "    High Score: "
             << highScore
             << "       ";

        cout.flush();
    }

    void drawInitialSnake() {

        gotoxy(
            snake1->body[0].xCoord,
            snake1->body[0].yCoord
        );

        cout << SNAKE1_BODY;

        gotoxy(
            snake2->body[0].xCoord,
            snake2->body[0].yCoord
        );

        cout << SNAKE2_BODY;

        displayCurrentScore();

        cout.flush();
    }

    bool checkWallCollision(Point head) {

        if (head.xCoord <= GAME_LEFT)
            return true;

        if (head.xCoord >= GAME_LEFT + GAME_WIDTH)
            return true;

        if (head.yCoord <= GAME_TOP)
            return true;

        if (head.yCoord >= GAME_TOP + GAME_HEIGHT)
            return true;

        return false;
    }

    bool checkSnakeCollision(
        Snake *snake,
        Point head
    ) {

        for (int i = 0;
             i < snake->getLength();
             i++) {

            if (samePoint(head, snake->body[i]))
                return true;
        }

        return false;
    }

    bool update() {

    Point oldTail1;
    Point oldTail2;

    MoveResult result1 = snake1->move(food, oldTail1);
    MoveResult result2 = snake2->move(food, oldTail2);

    Point head1 = snake1->body[0];
    Point head2 = snake2->body[0];

    if (checkWallCollision(head1)) {
        loser = 1;
        return false;
    }

    if (checkWallCollision(head2)) {
        loser = 2;
        return false;
    }

    if (result1 == DEAD && result2 == DEAD) {
        loser = 0;
        return false;
    }

    if (result1 == DEAD) {
        loser = 1;
        return false;
    }

    if (result2 == DEAD) {
        loser = 2;
        return false;
    }

    if (samePoint(head1, head2)) {
        loser = 0;
        return false;
    }

    for (int i = 1; i < snake2->getLength(); i++) {
        if (samePoint(head1, snake2->body[i])) {
            loser = 1;
            return false;
        }
    }

    for (int i = 1; i < snake1->getLength(); i++) {
        if (samePoint(head2, snake1->body[i])) {
            loser = 2;
            return false;
        }
    }

    if (result1 == ATE) {
        score1++;
        displayCurrentScore();
        spawnFood();
    }

    if (result2 == ATE) {
        score2++;
        displayCurrentScore();
        spawnFood();
    }

    if (result1 == MOVED) {
        gotoxy(oldTail1.xCoord, oldTail1.yCoord);
        cout << ' ';
    }

    if (result2 == MOVED) {
        gotoxy(oldTail2.xCoord, oldTail2.yCoord);
        cout << ' ';
    }

    gotoxy(head1.xCoord, head1.yCoord);
    cout << SNAKE1_BODY;

    gotoxy(head2.xCoord, head2.yCoord);
    cout << SNAKE2_BODY;

    cout.flush();

    return true;
}

    void getInput() {

        while (kbhitWrapper()) {

            int key = getchWrapper();

            if (key == -1)
                continue;

            if (key == 'w' ||
                key == 'W') {

                snake2->changeDirection(DIR_UP);
            }

            else if (key == 's' ||
                     key == 'S') {

                snake2->changeDirection(DIR_DOWN);
            }

            else if (key == 'a' ||
                     key == 'A') {

                snake2->changeDirection(DIR_LEFT);
            }

            else if (key == 'd' ||
                     key == 'D') {

                snake2->changeDirection(DIR_RIGHT);
            }

#ifdef _WIN32

            else if (key == 224 ||
                     key == 0) {

                if (!kbhitWrapper())
                    continue;

                int arrowKey = getchWrapper();

                if (arrowKey == 72)
                    snake1->changeDirection(DIR_UP);

                else if (arrowKey == 80)
                    snake1->changeDirection(DIR_DOWN);

                else if (arrowKey == 75)
                    snake1->changeDirection(DIR_LEFT);

                else if (arrowKey == 77)
                    snake1->changeDirection(DIR_RIGHT);
            }

#else

            else if (key == '\033') {

                if (!kbhitWrapper())
                    continue;

                int key2 = getchWrapper();

                if (key2 != '[')
                    continue;

                if (!kbhitWrapper())
                    continue;

                int key3 = getchWrapper();

                if (key3 == 'A')
                    snake1->changeDirection(DIR_UP);

                else if (key3 == 'B')
                    snake1->changeDirection(DIR_DOWN);

                else if (key3 == 'C')
                    snake1->changeDirection(DIR_RIGHT);

                else if (key3 == 'D')
                    snake1->changeDirection(DIR_LEFT);
            }

#endif
        }
    }

    void endGameScreen() {

        saveHighScore();

        showCursor();

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        cout << "\n";
        cout << "==============================\n";
        cout << "         GAME OVER\n";
        cout << "==============================\n\n";

        if (loser == 1) {

            cout << "Player 1 Lost!\n";
            cout << "Player 2 Wins!\n";
        }

        else if (loser == 2) {

            cout << "Player 2 Lost!\n";
            cout << "Player 1 Wins!\n";
        }

        else {

            cout << "Both Players Collided!\n";
        }

        cout << "\n";

        cout << "Player 1 Score: "
             << score1
             << "\n";

        cout << "Player 2 Score: "
             << score2
             << "\n";

        cout << "High Score: "
             << highScore
             << "\n";

        cout << "\n";
        cout << "1. Play Again\n";
        cout << "2. Exit\n";

        cout << "\nEnter choice: ";

        cout.flush();
    }
};

int main() {

    srand(
        static_cast<unsigned int>(
            time(0)
        )
    );

#ifndef _WIN32
    setupTerminal();
#endif

    const int UPDATE_INTERVAL = 200;

    while (true) {

        Board *board = new Board();

        board->drawInitialSnake();

        chrono::steady_clock::time_point start =
            chrono::steady_clock::now();

        while (true) {

            board->getInput();

            if (!board->update())
                break;

            chrono::steady_clock::time_point end =
                chrono::steady_clock::now();

            auto elapsedMs =
                chrono::duration_cast<
                    chrono::milliseconds
                >(end - start).count();

#ifdef _WIN32

            if (elapsedMs < UPDATE_INTERVAL) {

                Sleep(
                    UPDATE_INTERVAL -
                    static_cast<DWORD>(elapsedMs)
                );
            }

#else

            if (elapsedMs < UPDATE_INTERVAL) {

                usleep(
                    (UPDATE_INTERVAL - elapsedMs)
                    * 1000
                );
            }

#endif

            start = chrono::steady_clock::now();
        }

        board->endGameScreen();

#ifndef _WIN32
        restoreTerminal();
#endif

        int choice;

        cin >> choice;

#ifndef _WIN32
        setupTerminal();
#endif

        delete board;

        if (choice != 1)
            break;
    }

#ifndef _WIN32
    restoreTerminal();
#endif

    showCursor();

#ifdef _WIN32
    system("pause");
#endif

    return 0;
}