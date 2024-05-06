#include "TEditor.hpp"

void Editor::enableRawMode()
{
    halfdelay(0);
    move(0,0);
    raw();
    noecho();
    keypad(stdscr,TRUE);
}

void Editor::disableRawMode()
{
    noraw();
    echo();
    keypad(stdscr,FALSE);
}

void Editor::endProg()
{
    disableRawMode();
    clear();
    refresh();
}

Editor::Editor(int h)
{
    Kernel::setMenuHeight(h);
    enableRawMode();
}

Editor::~Editor()
{
    endProg();
}

void Editor::insertCh(char c)
{
    if(!isgraph(c))
        return;
    if(Kernel::getY() == Kernel::rowsCount()) 
        Kernel::insRow("");

    Kernel::insCh(c);
    moveRight();
}

void Editor::insRowBelow(std::string str)
{
    moveDown();
    Kernel::insRow(str);
    moveUp();
}

void Editor::delCh()
{
    
    if(Kernel::getX() == Kernel::strLen()) {
        Kernel::moveDown();
        std::string buff = Kernel::eraseRow();
        Kernel::moveUp();

    }
}

void Editor::goToRowEnd()
{
    int pos_x = Kernel::getX();

    if(pos_x > E.strSz()) {
        while(pos_x > E.strSz()) {
            moveLeft();
            pos_x = Kernel::getX();
        }
    }
    else {
        while(pos_x < E.strSz()) {
            moveRight();
            pos_x = Kernel::getX();
        }
    }
}

void Editor::goToRowStart()
{
    int pos_x = Kernel::getX();

    while(pos_x > 0) {
        moveLeft();
        pos_x = Kernel::getX();
    }
}

void Editor::moveUp()
{
    Kernel::moveUp();
    if(Kernel::getX() >= Kernel::strLen())
        goToRowEnd();
}

void Editor::moveDown()
{
    Kernel::moveDown();
    if(Kernel::getX() > Kernel::strLen())
        goToRowEnd();
}

void Editor::moveRight()
{
    if(Kernel::getX() >= Kernel::strLen()) {
        Kernel::moveDown();
        goToRowStart();
        return;
    }
    Kernel::moveRight();
}

void Editor::moveLeft()
{
    if(Kernel::getX() <= 0) {
        Kernel::moveUp();
        goToRowEnd();
        return;
    }
    Kernel::moveLeft();
}

chtype getKey()
{
    chtype c;
    while((c = getch()) == ERR) {};
    return c;
}

void Editor::processKey(chtype c)
{
    switch (c) {
        case CTRL_Q:
            if(E.getModif() == false) {
                endProg(0);
            }
            else {
                E.addMessage("All changes not will be saved! You are sure? y or n ");
                showScreen();
                while(1) {
                    c = getKey();
                    if(tolower(c) == 'y')
                        endProg(0);
                    if(tolower(c) == 'n') {
                        E.eraseMessage();
                        showScreen();
                        return;
                    }
                }
            }
            break;
        case CTRL_A:
            insertRowBefore("");
            break;
        case CTRL_S:           
            E.writeInFile();
            break;

        case CTRL_D:
            searchStr();
            break;

        case HOR_TAB:
            insTab();
            E.setModif(true);
            break;

        case KEY_LEFT:
            moveLeft();
            break;

        case KEY_RIGHT:
            moveRight();
            break;

        case KEY_DOWN:
            moveDown();
            break;

        case KEY_UP:
            moveUp();
            break;

        case KEY_NPAGE:
            downPage();
            break;

        case KEY_PPAGE:
            upPage();
            break;
        
        case KEY_HOME:
            goToRowStart();
            break;

        case KEY_END:
            goToRowEnd();
            break;

        case KEY_IL:
        case LINE_FEED:
        case KEY_ENTER:
            addNewLine();
            break;

        case KEY_DC: 
            deleteSym();
            E.setModif(true);
            break;

            case KEY_DL:
                E.delRowBelow();
            break;

        case KEY_BACKSPACE:  
            backSpaceCh();
            break;

        default:
            if(insChar(c))
                E.setModif(true);
            break;
    }
}

void Editor::start()
{
    while(1) {
        Kernel::showScreen();
        processKey(getKey());
    }
}