#ifndef KEYBOARDPOLLER_H
#define KEYBOARDPOLLER_H

#include <functional>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <cstdio>

class KeyboardPoller {
public:
    // Signal called when a key is pressed
    std::function<void(char)> keyboardHitSignal = nullptr;

    // Status of the poller execution
    bool running = false;

    // Default frequency in Hz
    int frequency = 100;

    // Default constructor
    KeyboardPoller();

    // Destructor
    ~KeyboardPoller();

    // Start the poller
    void start();

    // Stop the poller
    void stop();

private:
    std::thread pollerThread; // Define pollerThread here

    // Internal function to monitor the keyboard
    void pollKeyboard();

    // Check if a key is pressed
    int kbhit();
};

#endif // KEYBOARDPOLLER_H
