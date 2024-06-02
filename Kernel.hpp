#ifndef TEKERNEL
#define TEKERNEL

#include "macros.hpp"
#include <ncurses.h>
#include <vector>
#include <string>
#include <fstream>

class Kernel
{
private:
    static inline std::vector<std::string>  rows{};
    static inline std::string               displaying_name{},file_name{},message{};
    static inline int                       xoff{},yoff{},menu_height{},width{},height{};
    static inline bool                      modif{false};
private:
    static void         printRows();
    static void         printMenu();
public:
    Kernel() = delete;
    static void         init();
    static void         setMenuHeight(int);
    static int          getX();
    static int          getY();
    static int          getWinH();
    static bool         getModif();
    static chtype       getKey();
    static std::string& getStr();
    static int          getAllSymCount();
    static std::string  getFileName();
    static int          strLen();
    static int          strSz();
    static int          rowsCount();
    static void         setModif(bool);
    static void         setFileName(std::string);
    static void         moveUp();
    static void         moveDown();
    static void         moveRight();
    static void         moveLeft();
    static void         insCh(char);
    static void         eraseCh();
    static void         insStr(std::string&& str);
    static void         insRow(std::string);
    static std::string  eraseRow();
    static void         addMsg(std::string&&);
    static void         eraseMsg();
    static int          writeInFile();
    static void         showScreen();
};

#endif