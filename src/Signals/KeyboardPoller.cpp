#include "KeyboardPoller.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>

// Default constructor
KeyboardPoller::KeyboardPoller() {
    running.store(false);
}

// Destructor
KeyboardPoller::~KeyboardPoller() {
    stop();
}

// Start the poller
void KeyboardPoller::start() {
    running.store(true);
    pollerThread = std::thread(&KeyboardPoller::pollKeyboard, this);
}

// Stop the poller
void KeyboardPoller::stop() {
    running.store(false);
    if (pollerThread.joinable()) {
        pollerThread.join();
    }
}


// Internal function to monitor the keyboard
void KeyboardPoller::pollKeyboard() {
    while (running.load()) {
        if (kbhit()) {
            char c = getchar();
            if (keyboardHitSignal) {
                keyboardHitSignal(c);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / frequency));
    }
}

// Check if a key is pressed
int KeyboardPoller::kbhit() {
    //it works like that under linux! Why ? The fuck why... it works like that.
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
