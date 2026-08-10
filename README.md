# 🐍 Classic Snake Game — C++ Console Version

A modern take on the retro **Snake Game**, built entirely in **C++** using the Windows Console. The game features smooth movement, real-time keyboard input, score tracking, and persistent high scores — all inside a text-based terminal.

---

## 🎮 Features

* **Smooth Real-Time Movement** — Control the snake using `W`, `A`, `S`, and `D` keys.
* **Dynamic Growth** — The snake grows longer each time it eats food.
* **Persistent High Score** — High score is saved locally in `highscore.txt`.
* **Apple as Food 🍎** — Food randomly spawns on the board after being eaten.
* **No-Flicker Rendering** — Efficient console updates using `gotoxy()` instead of repeatedly clearing the screen.
* **Replay Option** — Choose to play again or exit after Game Over.
* **Collision Detection** — Detects both boundary collisions and self-collision.

---

## 🎯 Game Controls

| Key | Action     |
| --- | ---------- |
| `W` | Move Up    |
| `A` | Move Left  |
| `S` | Move Down  |
| `D` | Move Right |
| `1` | Play Again |
| `2` | Exit Game  |

---

## ⚙️ How It Works

The project follows a modular, object-oriented structure:

### 1. Snake Class

Responsible for managing the snake itself, including:

* Snake body
* Movement
* Direction
* Growth
* Self-collision detection

### 2. Board Class

Responsible for managing the game environment, including:

* Board rendering
* Food generation
* Score display
* Keyboard input
* Collision checks
* Game state updates

### 3. `main()` Function

Acts as the main controller of the application:

* Starts the game
* Runs the game loop
* Handles the restart/exit menu
* Maintains consistent game speed

---

## 📂 Project Structure

```text
SnakeGame/
│
├── main.cpp
├── highscore.txt
├── README.md
│
└── assets/
    └── (optional)
```

---

## 🚀 How to Run

### Requirements

* Windows OS
* C++ compiler

  * MinGW / g++
  * Visual Studio
  * Any compatible C++ compiler

### 1. Clone the Repository

```bash
git clone https://github.com/Aayushi221204/ITP_project.git
```

### 2. Navigate to the Project Directory

```bash
cd ITP_project
```

### 3. Compile the Program

Using `g++`:

```bash
g++ main.cpp -o SnakeGame
```

### 4. Run the Game

```bash
SnakeGame.exe
```

---

## 🧠 Approach Taken

The project uses a **modular, object-oriented design** to keep the game logic organized and maintainable.

The main focus areas are:

* Separation of game responsibilities
* Reusable game components
* Efficient console rendering
* Simple and maintainable game logic
* Minimal external dependencies

---

## 🗂️ Data Structures Used

### `struct Point`

Used to represent the coordinates of objects on the game board.

```cpp
struct Point {
    int x;
    int y;
};
```

### Snake Body

The snake body is represented using a collection of `Point` objects, such as:

* Static arrays
* `vector<Point>`

This allows the snake to grow dynamically as the player collects food.

---

## ⚖️ Tradeoffs Made

| Decision                       | Reason                                                     |
| ------------------------------ | ---------------------------------------------------------- |
| Console-based rendering        | Keeps the project lightweight and dependency-free          |
| Windows Console API            | Provides real-time keyboard input and cursor positioning   |
| `gotoxy()` rendering           | Reduces screen flickering compared to clearing the console |
| Local text file for high score | Simple persistent storage without requiring a database     |
| Object-oriented structure      | Makes the code easier to maintain and extend               |

---

## 🔧 Extensibility

The code is designed to be easily extended with additional features.

Possible extensions include:

* 🍎 Multiple fruit types
* ⚡ Difficulty levels
* ⏸️ Pause / Resume functionality
* 🏆 Enhanced scoreboard
* 🚧 Obstacles
* 🎨 Colored console graphics
* 🔊 Sound effects
* 👥 Two-player mode
* 📏 Configurable board size
* 🗄️ Database-based leaderboard

Each major component is designed to remain independent, making it easier to introduce new functionality without significantly modifying the existing game logic.

---

## 🔮 Future Enhancements

Planned improvements include:

1. **Colored Graphics**
   Add Windows Console color support for a more visually appealing experience.

2. **Sound Effects**
   Add sounds for eating food, achieving high scores, and Game Over.

3. **Obstacles & Levels**
   Introduce obstacles and progressively challenging levels.

4. **Two-Player Mode**
   Allow two players to compete on the same board.

5. **Configurable Game Settings**
   Allow players to customize:

   * Game speed
   * Board dimensions
   * Difficulty

6. **Database Leaderboard**
   Replace local high-score storage with a database-backed leaderboard.

---

## 👩‍💻 Author

**Aayushi Shah**

Developed using **C++ | Windows Console**

**Year:** 2025

---

## ⭐ Contributing

Feel free to fork the repository, experiment with the code, suggest improvements, or add new features.

If you enjoyed the project, consider giving it a ⭐ on GitHub!
