// Clear screen (or push old screen content up so that it is not visible)
void clear(void);
// Wait a given number of milliseconds for key from stdin.
// Returns EOF when no key was entered within the given number of milliseconds.
int wait_for_key(int milliseconds);
