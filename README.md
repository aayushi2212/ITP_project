Classic Snake Game (C++ Console Version)
A modern take on the retro Snake Game, built entirely in C++ using the Windows Console. The game features smooth movement, realtime keyboard input, score tracking, and persistent high scores — all inside a text-based terminal.
🎮 Features
•	Smooth RealTime Movement — Move the snake using W, A, S, D keys.
•	Dynamic Growth — Snake grows longer each time it eats food.
•	Persistent High Score — High score is saved locally in highscore.txt.
•	Apple as Food 🍎 — Randomly spawns on the board each time it’s eaten.
•	No Flicker Rendering — Efficient console updates using gotoxy() instead of clearing the screen.
•	Replay Option — Choose to play again or exit after game over.
•	Boundary & Self Collision Detection — Game ends on wall hit or selfbite.
🎯 Game Controls
Key	Action
W	Move Up
A	Move Left
S	Move Down
D	Move Right
1	Play Again
2	Exit Game
⚙️ How It Works
The project follows an object-oriented structure:
1.	1. Snake Class - Handles body, movement, collision, growth.
2.	2. Board Class - Manages drawing, score display, input, updates.
3.	3. main() Function - Runs game loop, restart/exit menu, consistent speed.
📂 Code Structure
SnakeGame/
├── main.cpp
├── highscore.txt
├── README.md
└── assets/ (optional)
🚀 How to Run
Requirements:
- Windows OS
- C++ compiler (e.g., g++, MinGW, or Visual Studio)
Steps:
# Clone repository
git clone https://github.com/Aayushi221204/ITP_project.git

# Navigate folder
cd ITP_project

# Compile
g++ main.cpp -o SnakeGame

# Run
SnakeGame.exe
🔮 Future Enhancements
•	Colored graphics using Windows console color codes
•	Sound effects on eating food or game over
•	Obstacles or Levels
•	Two-player mode
•	Configurable speed and board size
•	Database-based leaderboard

i. Approach Taken - Modular, object-oriented design for efficiency and maintainability.
ii. Data Structures Used - struct Point, static arrays or vectors for body parts.
iii. Tradeoffs Made - Console rendering for simplicity, minimal dependencies.
🔧 Extensibility of Code
Yes, the code is modular and receptive to new features like:
- Multiple fruit types
- Difficulty levels
- Pause/Resume
- Scoreboard enhancements
Each module is independent, allowing easy feature addition.
👩‍💻 Author
Aayushi Shah
Developed using C++ | Windows Console
Year: 2025
Feel free to fork, star, or suggest improvements!
