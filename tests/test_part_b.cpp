#include <cassert>
#include <iostream>

#define main disabled_game_main
#include "snake_game.cpp"
#undef main

int main() {
    std::cout << "Running test suite..." << std::endl;
    
    // Test 1 (Rule 1): Opposite direction rejection
    // Snake starts moving DIR_RIGHT by default.
    // Changing direction to DIR_LEFT should be ignored.
    Snake s1(10, 10);
    s1.changeDirection(DIR_LEFT);
    Point food1(50, 50);
    Point oldTail1;
    MoveResult r1 = s1.move(food1, oldTail1);
    assert(r1 == MOVED);
    assert(s1.body[0].xCoord == 11);
    assert(s1.body[0].yCoord == 10);

    // Test 2 (Rule 2): Self-collision causes death
    // Grow snake to length 5 around a 2x2 loop and collide head into body[4]
    Snake s2(10, 10);
    Point oldTail2;
    // Step 1: eat at (11, 10) -> length 2, moving right
    Point f1(11, 10);
    assert(s2.move(f1, oldTail2) == ATE);
    // Step 2: turn DOWN, eat at (11, 11) -> length 3, moving down
    s2.changeDirection(DIR_DOWN);
    Point f2(11, 11);
    assert(s2.move(f2, oldTail2) == ATE);
    // Step 3: turn LEFT, eat at (10, 11) -> length 4, moving left
    s2.changeDirection(DIR_LEFT);
    Point f3(10, 11);
    assert(s2.move(f3, oldTail2) == ATE);
    // Step 4: turn UP, eat at (10, 10) -> length 5, moving up
    s2.changeDirection(DIR_UP);
    Point f4(10, 10);
    assert(s2.move(f4, oldTail2) == ATE);
    // Step 5: turn RIGHT into (11, 10) which is body[4] -> collision!
    s2.changeDirection(DIR_RIGHT);
    Point f5(99, 99);
    MoveResult r2 = s2.move(f5, oldTail2);
    assert(r2 == DEAD);

    std::cout << "All Part B tests passed successfully!" << std::endl;
    return 0;
}
