#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <map>

#define MENU_HEIGHT 2
#define CTRL_A 0x01
#define CTRL_D 0x04
#define CTRL_I 0x09
#define CTRL_J 0x0A
#define CTRL_Q 0x11
#define CTRL_S 0x13
#define KEY_ESC 0x1B
#define HOR_TAB CTRL_I
#define LINE_FEED CTRL_J

// defined in main.cpp
void moveLeft();
void moveRight();
void moveDown();
void moveUp();
void goToRowEnd();
void goToRowStart();
void goToLastRow();
void goToFirstRow();
void deleteSym();
//end

class Editor
{
private:
    std::vector<std::string> rows;
    std::string message;
    std::string displaying_name;
    std::string file_name;
    bool modif;
private:
    void printRows();
    void printMenu();
public:
    int xoff,yoff;
    Editor() = default;
    void init();
    std::string getRowStr();
    void setFileName(std::string);
    void insertChar(char);
    void insertNewRow(std::string);
    int getAllSymCount();
    void addMessage(std::string);
    void eraseMessage();
    void delChar();
    void delRowBelow();
    int rowsCount();
    int strSz();
    std::string getFileName();
    void writeInFile();
    int searchWordInText(std::string);
    int searchWordInRow(std::string,int = 0);
    void setModif(bool);
    bool getModif();
    void showScreen();
};
