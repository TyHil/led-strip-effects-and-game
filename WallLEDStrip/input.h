/*
  Input library header for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#define Y_PIN A3
#define X_PIN A2
#define SW_PIN 4

class Input {
  public:
    Input();
    bool up(bool sensitive = false);
    bool down(bool sensitive = false);
    bool left(bool sensitive = false);
    bool right(bool sensitive = false);
    bool click();
};
