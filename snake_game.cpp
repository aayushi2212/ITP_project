#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
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

enum MoveResult { DEAD, MOVED, ATE };

struct Point {
    int xCoord, yCoord;
    Point() {}
    Point(int x, int y) : xCoord(x), yCoord(y) {}
};

void hideCursor() {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);
#else
    cout << "\033[?25l";
#endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord{(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
#endif
}

#ifdef _WIN32
bool kbhitWrapper() {
    return kbhit();
}
int getchWrapper() {
    return getch();
}
#else
bool kbhitWrapper() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term2);

    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    return bytesWaiting > 0;
}

int getchWrapper() {
    return getchar();
}
#endif

// --- Input Seam Interface ---
class InputSource {
public:
    virtual ~InputSource() {}
    virtual bool hasKey() = 0;
    virtual int getKey() = 0;
};

class KeyboardInput : public InputSource {
public:
    bool hasKey() override {
        return kbhitWrapper();
    }
    int getKey() override {
        return getchWrapper();
    }
};

class Snake {
    int length;
    char direction;

public:
    Point body[MAX_LENGTH];
    Snake(int x, int y) {
        length = 1;
        body[0] = Point(x, y);
        direction = DIR_RIGHT;
    }

    int getLength() { return length; }
    char getDirection() { return direction; }

    void changeDirection(char newDirection) {
        if (newDirection == DIR_UP && direction != DIR_DOWN)
            direction = newDirection;
        else if (newDirection == DIR_DOWN && direction != DIR_UP)
            direction = newDirection;
        else if (newDirection == DIR_LEFT && direction != DIR_RIGHT)
            direction = newDirection;
        else if (newDirection == DIR_RIGHT && direction != DIR_LEFT)
            direction = newDirection;
    }

    MoveResult move(Point food, Point &oldTail) {
        oldTail = body[length - 1];
        for (int i = length - 1; i > 0; i--)
            body[i] = body[i - 1];

        switch (direction) {
            case DIR_UP: body[0].yCoord--; break;
            case DIR_DOWN: body[0].yCoord++; break;
            case DIR_RIGHT: body[0].xCoord++; break;
            case DIR_LEFT: body[0].xCoord--; break;
        }

        for (int i = 1; i < length; i++)
            if (body[0].xCoord == body[i].xCoord && body[0].yCoord == body[i].yCoord)
                return DEAD;

        if (food.xCoord == body[0].xCoord && food.yCoord == body[0].yCoord) {
            body[length] = oldTail;
            length++;
            return ATE;
        }

        return MOVED;
    }
};

class Board {
    Snake *snake;
    InputSource *inputSource;
    bool ownsInputSource;
    const char SNAKE_BODY = 'O';
    Point food;
    int score;
    int highScore = 0;
    char fruitChar;
    vector<char> fruits = {'@', '*', '$', '&', '%', '+'};

public:
    Board(InputSource *input = nullptr, bool headless = false) {
        if (input != nullptr) {
            inputSource = input;
            ownsInputSource = false;
        } else {
            inputSource = new KeyboardInput();
            ownsInputSource = true;
        }
        snake = new Snake(GAME_LEFT + GAME_WIDTH / 2, GAME_TOP + GAME_HEIGHT / 2);
        score = 0;
        if (!headless) {
            loadHighScore();
            hideCursor();
            drawBorder();
            spawnFood();
        }
    }

    ~Board() {
        delete snake;
        if (ownsInputSource) {
            delete inputSource;
        }
    }

    Snake* getSnake() { return snake; }
    int getScore() { return score; }
    Point getFood() { return food; }
    void setFood(Point p) { food = p; }

    void loadHighScore() {
        ifstream file("highscore.txt");
        if (file.is_open()) file >> highScore;
        file.close();
    }

    void saveHighScore() {
        if (score > highScore) {
            ofstream file("highscore.txt");
            file << score;
            file.close();
        }
    }

    void drawBorder() {
        system("clear");
#ifdef _WIN32
        system("cls");
#endif
        for (int x = GAME_LEFT; x <= GAME_LEFT + GAME_WIDTH; x++) {
            gotoxy(x, GAME_TOP);
            cout << "#";
            gotoxy(x, GAME_TOP + GAME_HEIGHT);
            cout << "#";
        }
        for (int y = GAME_TOP; y <= GAME_TOP + GAME_HEIGHT; y++) {
            gotoxy(GAME_LEFT, y);
            cout << "#";
            gotoxy(GAME_LEFT + GAME_WIDTH, y);
            cout << "#";
        }
        displayCurrentScore();
    }

    void spawnFood() {
        while (true) {
            int x = GAME_LEFT + 1 + rand() % (GAME_WIDTH - 2);
            int y = GAME_TOP + 1 + rand() % (GAME_HEIGHT - 2);
            bool onSnake = false;

            for (int i = 0; i < snake->getLength(); i++) {
                if (snake->body[i].xCoord == x && snake->body[i].yCoord == y) {
                    onSnake = true;
                    break;
                }
            }

            if (!onSnake) {
                food = Point(x, y);
                fruitChar = fruits[rand() % fruits.size()];
                gotoxy(food.xCoord, food.yCoord);
                cout << fruitChar;
                break;
            }
        }
    }

    void displayCurrentScore() {
        gotoxy(GAME_LEFT, GAME_TOP - 1);
        cout << "Score: " << score << "   High Score: " << highScore << "   ";
    }

    void drawInitialSnake() {
        for (int i = 0; i < snake->getLength(); i++) {
            gotoxy(snake->body[i].xCoord, snake->body[i].yCoord);
            cout << SNAKE_BODY;
        }
        displayCurrentScore();
    }

    bool update() {
        Point oldTail;
        MoveResult r = snake->move(food, oldTail);

        int headX = snake->body[0].xCoord;
        int headY = snake->body[0].yCoord;

        if (headX <= GAME_LEFT || headX >= GAME_LEFT + GAME_WIDTH ||
            headY <= GAME_TOP || headY >= GAME_TOP + GAME_HEIGHT)
            return false;

        if (r == DEAD)
            return false;

        if (r == MOVED) {
            gotoxy(oldTail.xCoord, oldTail.yCoord);
            cout << ' ';
        } else if (r == ATE) {
            score++;
            displayCurrentScore();
            spawnFood();
        }

        gotoxy(headX, headY);
        cout << SNAKE_BODY;

        return true;
    }

    void getInput() {
        if (inputSource && inputSource->hasKey()) {
            int key = inputSource->getKey();
            if (key == 'w' || key == 'W')
                snake->changeDirection(DIR_UP);
            else if (key == 'a' || key == 'A')
                snake->changeDirection(DIR_LEFT);
            else if (key == 's' || key == 'S')
                snake->changeDirection(DIR_DOWN);
            else if (key == 'd' || key == 'D')
                snake->changeDirection(DIR_RIGHT);
        }
    }

    void endGameScreen() {
        saveHighScore();
        system("clear");
#ifdef _WIN32
        system("cls");
#endif
        cout << "\n===== GAME OVER =====\n";
        cout << "Final Score: " << score << "\n";
        cout << "High Score: " << max(score, highScore) << "\n";
        cout << "\n1. Play Again\n2. Exit\n";
        cout << "\nEnter choice: ";
    }
};

int main() {
    srand(time(0));
    const int UPDATE_INTERVAL = 200;

    while (true) {
        Board *board = new Board();
        board->drawInitialSnake();

        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        while (board->update()) {
            board->getInput();

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

#ifdef _WIN32
            if (elapsedMs < UPDATE_INTERVAL)
                Sleep(UPDATE_INTERVAL - elapsedMs);
#else
            if (elapsedMs < UPDATE_INTERVAL)
                usleep((UPDATE_INTERVAL - elapsedMs) * 1000);
#endif

            start = std::chrono::steady_clock::now();
        }

        board->endGameScreen();
        int choice;
        std::cin >> choice;
        if (choice != 1) {
            delete board;
            break;
        }
        delete board;
    }

#ifdef _WIN32
    system("pause");
#else
    std::cout << "\033[?25h"; // Show cursor again
#endif

    return 0;
}
