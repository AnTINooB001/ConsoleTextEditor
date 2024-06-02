#ifndef TEDITOR
#define TEDITOR

#include "Kernel.hpp"

class Editor
{
private:
    void enableRawMode();
    void disableRawMode();
    void insCh(chtype);
    void delCh();
    void bsCh();
    void insRowBelow(std::string&&);
    void delRowBelow();
    void insTab();
    void addNewLine();
    void insRowBefore(std::string&&);
    void goToRowEnd();
    void goToRowStart();
    void goToFirstRow();
    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void downPage();
    void upPage();
    int  searchWordInText(std::string&&);
    int  searchWordInRow(std::string,int = 0);
    void searchStr();
    void ReadFromFile(std::string);
    int  processKey(chtype c);
private:
    int height,menu_height;
public:
    Editor(int);
    void start(std::string);
    ~Editor();
};

#endif