#ifndef TEDITOR
#define TEDITOR

#include "Kernel.hpp"

class Editor
{
private:
    void enableRawMode();
    void disableRawMode();
    void insertCh(char);
    void insRowBelow(std::string);
    void delCh();
    void endProg();
    void goToRowEnd();
    void goToRowStart();
    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void processKey(chtype c);
    void getKey();
public:
    Editor(int);
    void start();
    ~Editor();
};

#endif