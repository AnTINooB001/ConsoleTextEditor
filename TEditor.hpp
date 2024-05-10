#ifndef TEDITOR
#define TEDITOR

#include "Kernel.hpp"

class Editor
{
private:
    void enableRawMode();
    void disableRawMode();
    void insCh(chtype);
    void insRowBelow(std::string);
    void delRowBelow();
    void delCh();
    void endProg();
    void goToRowEnd();
    void goToRowStart();
    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void processKey(chtype c);
    void insTab();
    void downPage();
    void upPage();
    void bsCh();
    void addNewLine();
    int searchWordInText(std::string);
    int searchWordInRow(std::string,int = 0);
    void searchStr();
    void openFile(std::string);
    void insRowBefore(std::string);
    void goToFirstRow();
private:
    int height,menu_height;
public:
    Editor(int);
    void start(std::string);
    ~Editor();
};

#endif