#ifndef TEKERNEL
#define TEKERNEL
#include <ncurses.h>
#include <vector>
#include <string>

class Kernel
{
private:
    static inline std::vector<std::string> rows{};
    static inline std::string message{};
    static inline int xoff{},yoff{},msg_pos{},menu_height{};
private:
    static void printRows();
    static void printMenu();
public:
    Kernel() = delete;
    static void setMenuHeight(int);
    static int getX();
    static int getY();
    static int strLen();
    static int rowsCount();
    static void moveUp();
    static void moveDown();
    static void moveRight();
    static void moveLeft();
    static void insCh(char);
    static void insStr()
    static void insRow(std::string);
    static void showScreen();
    static void addMsg(std::string);
    static void eraseMsg();
    static void eraseCh();
    static std::string eraseRow();
};

#endif