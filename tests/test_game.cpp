#include <cassert>
#include <iostream>
#include <vector>

#define main disabled_game_main
#include "snake_game.cpp"
#undef main

// Stub / Fake Input Source for testing the Input Seam
class StubInputSource : public InputSource {
private:
    std::vector<int> keys;
    size_t index;

public:
    StubInputSource() : index(0) {}
    explicit StubInputSource(int singleKey) : index(0) {
        keys.push_back(singleKey);
    }
    explicit StubInputSource(const std::vector<int>& keySequence) : keys(keySequence), index(0) {}

    void queueKey(int key) {
        keys.push_back(key);
    }

    bool hasKey() override {
        return index < keys.size();
    }

    int getKey() override {
        if (index < keys.size()) {
            return keys[index++];
        }
        return 0;
    }
};

void test_rule1_opposite_direction_rejection() {
    std::cout << "[RUN] Testing Rule 1: Opposite direction rejection..." << std::endl;
    Snake s(10, 10); // default moves DIR_RIGHT
    assert(s.getDirection() == DIR_RIGHT);

    // Attempt to turn directly opposite (DIR_LEFT)
    s.changeDirection(DIR_LEFT);
    assert(s.getDirection() == DIR_RIGHT);

    // Verify movement continues in DIR_RIGHT
    Point food(99, 99);
    Point oldTail;
    MoveResult res = s.move(food, oldTail);
    assert(res == MOVED);
    assert(s.body[0].xCoord == 11);
    assert(s.body[0].yCoord == 10);
    std::cout << "[PASS] Rule 1 verified." << std::endl;
}

void test_rule2_self_collision_death() {
    std::cout << "[RUN] Testing Rule 2: Self-collision causes death..." << std::endl;
    Snake s(10, 10);
    Point oldTail;

    // Grow snake to length 5 around a loop
    // (10,10) -> (11,10) [Right]
    assert(s.move(Point(11, 10), oldTail) == ATE);
    assert(s.getLength() == 2);

    // (11,10) -> (11,11) [Down]
    s.changeDirection(DIR_DOWN);
    assert(s.move(Point(11, 11), oldTail) == ATE);
    assert(s.getLength() == 3);

    // (11,11) -> (10,11) [Left]
    s.changeDirection(DIR_LEFT);
    assert(s.move(Point(10, 11), oldTail) == ATE);
    assert(s.getLength() == 4);

    // (10,11) -> (10,10) [Up]
    s.changeDirection(DIR_UP);
    assert(s.move(Point(10, 10), oldTail) == ATE);
    assert(s.getLength() == 5);

    // Turn Right into body segment at (11, 10)
    s.changeDirection(DIR_RIGHT);
    MoveResult res = s.move(Point(99, 99), oldTail);
    assert(res == DEAD);
    std::cout << "[PASS] Rule 2 verified." << std::endl;
}

void test_rule5_keyboard_direction_input_via_seam() {
    std::cout << "[RUN] Testing Rule 5: Keyboard inputs update direction via InputSource seam..." << std::endl;

    // Test 'w' changes direction to DIR_UP
    {
        StubInputSource stub('w');
        Board board(&stub, true);
        assert(board.getSnake()->getDirection() == DIR_RIGHT);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_UP);
    }

    // Test 'a' changes direction to DIR_LEFT (when moving UP)
    {
        StubInputSource stub('a');
        Board board(&stub, true);
        board.getSnake()->changeDirection(DIR_UP);
        assert(board.getSnake()->getDirection() == DIR_UP);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_LEFT);
    }

    // Test 's' changes direction to DIR_DOWN (when moving RIGHT)
    {
        StubInputSource stub('s');
        Board board(&stub, true);
        assert(board.getSnake()->getDirection() == DIR_RIGHT);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_DOWN);
    }

    // Test 'd' changes direction to DIR_RIGHT (when moving DOWN)
    {
        StubInputSource stub('d');
        Board board(&stub, true);
        board.getSnake()->changeDirection(DIR_DOWN);
        assert(board.getSnake()->getDirection() == DIR_DOWN);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_RIGHT);
    }

    // Test uppercase keys ('W', 'A', 'S', 'D')
    {
        StubInputSource stub('W');
        Board board(&stub, true);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_UP);
    }

    // Test invalid keys do not alter direction
    {
        StubInputSource stub('z');
        Board board(&stub, true);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_RIGHT);
    }

    // Test opposite direction input via seam is rejected
    {
        StubInputSource stub('a'); // left when moving right
        Board board(&stub, true);
        assert(board.getSnake()->getDirection() == DIR_RIGHT);
        board.getInput();
        assert(board.getSnake()->getDirection() == DIR_RIGHT);
    }

    std::cout << "[PASS] Rule 5 verified via InputSource seam." << std::endl;
}

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Running Snake Game Unit Test Suite" << std::endl;
    std::cout << "=========================================" << std::endl;

    test_rule1_opposite_direction_rejection();
    test_rule2_self_collision_death();
    test_rule5_keyboard_direction_input_via_seam();

    std::cout << "=========================================" << std::endl;
    std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    std::cout << "=========================================" << std::endl;
    return 0;
}
