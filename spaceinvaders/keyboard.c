#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>

double sec(void);

double sec(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

void clear(void){
    printf("\033[H\033[J");
}

int wait_for_key(int milliseconds){
    // Save copy of terminal state to restore lalter
    struct termios backup_terminal_state;
    tcgetattr(0, &backup_terminal_state);

    struct termios terminal_state = backup_terminal_state;

    // Enable terminal raw input
    cfmakeraw(&terminal_state);
    tcsetattr(0, TCSANOW, &terminal_state);

    // Initialize file descriptor for reading from standard input
    fd_set file_descriptor;
    FD_ZERO(&file_descriptor);
    FD_SET(STDIN_FILENO, &file_descriptor);

    int key = EOF;

    double start_time = sec();

    // Check if input is available
    struct timeval wait_duration = {0, milliseconds * 1000};

    int select_result = select(1, &file_descriptor, NULL, NULL, &wait_duration);

    if (select_result > 0){
        // Read chars from stdin
        char buf[1024];
        ssize_t result = read(0, buf, sizeof(buf));

        if (result != -1){
            // Only return first char
            key = (unsigned char)buf[0];
        }
    }

    // Reset terminal state
    tcsetattr(0, TCSANOW, &backup_terminal_state);

    double elapsed_time = sec() - start_time;

    double wait_time = milliseconds * 0.001;

    // Wait out remaining time if select returned too early.
    if (elapsed_time < wait_time){
        usleep((wait_time - elapsed_time) * 1e6);
    }

    return key;
}
