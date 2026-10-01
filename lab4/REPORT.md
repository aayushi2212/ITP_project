# Lab 4 - Group A

| | |
|---|---|
| Repository | aayushi2212/ITP_project (your own snake game) |
| Base tag | `lab4-base` at commit 9f3e214a0aff948736e64264901f830d3efaf06f |
| Pull request | #1 |

---

## 1. Five rules - [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | The snake ignores opposite direction inputs (e.g., when moving right, inputting left is ignored). |
| 2 | The snake dies when its head collides with any of its own body segments. |
| 3 | Eating a fruit increments the current score by 1 and prompts a new fruit to spawn. |
| 4 | Spawning food generates coordinates strictly on empty tiles unoccupied by the snake. |
| 5 | Keyboard inputs ('w', 'a', 's', 'd') update the snake's movement direction during the game loop. |

---

## 2. What you could test, and what stopped you - [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Snake ignores opposite direction inputs | Yes | None (pure deterministic logic in `Snake::changeDirection` and `Snake::move`) |
| 2 | Snake dies on self-collision | Yes | None (pure deterministic collision check in `Snake::move`) |
| 3 | Eating fruit increases score by 1 and spawns food | No | `snake_game.cpp:250` - `score` is a private variable with no accessor; `update()` calls `displayCurrentScore()` and `spawnFood()` which execute `gotoxy()` console rendering (`snake_game.cpp:219-220`, `snake_game.cpp:211-212`) directly coupled to terminal stdout. |
| 4 | Food spawn never occupies snake tiles | No | `snake_game.cpp:197-198` - `Board::spawnFood()` calls non-deterministic unseeded `rand()`, directly prints to the console with `gotoxy()`/`cout` (`snake_game.cpp:211-212`), and stores `food` in private field `snake_game.cpp:143` with no injection point. |
| 5 | Keyboard inputs ('w','a','s','d') change direction | No | `snake_game.cpp:261-262` - `Board::getInput()` calls `kbhitWrapper()` and `getchWrapper()`, directly polling hardware keyboard input via `kbhit()` and `getch()` with no input abstraction or seam to supply synthetic key events. |

> **Rules testable without modifying the source: 2 / 5**

---

## 3. Coverage, and what it missed - [6]

| | |
|---|---|
| Line coverage | 19.54 % |
| Branch coverage | 22.29 % |
| Command used | `g++ -std=c++11 --coverage -O0 -g -I. tests/test_part_b.cpp -o run_tests.exe && .\run_tests.exe && gcov -b test_part_b.gcda` |

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | Snake body length increases by 1 when eating fruit |
| Line that runs | `snake_game.cpp:132` (`length++;`) |
| The assertion that is missing | `assert(s2.getLength() == 2);` after `s2.move(f1, oldTail)` (the test calls `move()` to grow the snake for self-collision setup, executing the length increment, but asserts only the final death state rather than verifying growth). |

---

## 4. The seam - [10]

| | |
|---|---|
| Rule made testable | Keyboard inputs ('w', 'a', 's', 'd') update the snake's movement direction during the game loop (Rule 5) |
| Commit 1 (seam) | 1639d6e |
| Commit 2 (test) | 49ae13b |
| Seam kind | object |
| Enabling point | `Board::Board(InputSource *input, bool headless)` constructor parameter and `Board::getInput()` where `inputSource` is injected and dispatched polymorphically |
| What production code gave up | Direct binding to `<conio.h>` global console polling (`kbhit()` / `getch()`). In exchange for testability, production code introduced dynamic polymorphism (an abstract base class `InputSource`, virtual function calls with vtable lookup, and ownership management for the input strategy). |

---

## 5. The double - [4]

| | |
|---|---|
| What you passed through the seam | stub |
| The method under test | `Board::getInput()` |

The collaborator was asked questions by production code (`hasKey()` asks "is a key ready?" and `getKey()` asks "what key was pressed?"). Because the collaborator returns values to answer queries rather than being commanded to perform an action with side effects, the test double is a stub providing predefined answers.

---

## 6. Two smells in your own tests - [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Assertion roulette | `tests/test_part_b.cpp:21` | Add descriptive failure messages to assertions (e.g. `assert((r1 == MOVED) && "Expected snake to continue moving forward");`) or use named assertions. |
| 2 | Eager test | `tests/test_part_b.cpp:24` | Split the multi-step test into separate, single-purpose unit test functions (one testing snake growth across multiple directions, and one specifically testing self-collision death). |

Fixing them is optional. Finding them is not.
