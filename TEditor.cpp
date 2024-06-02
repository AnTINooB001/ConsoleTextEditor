#include "TEditor.hpp"

Editor::Editor(int h) : menu_height{h}
{   
    Kernel::init();
    Kernel::setMenuHeight(h);
    height = Kernel::getWinH();
    enableRawMode();
}

Editor::~Editor()
{
    disableRawMode();
    clear();
    refresh();
}

void Editor::enableRawMode()
{
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

void Editor::insCh(chtype c)
{
    if(!isgraph(c) && !isspace(c))
        return;
    if(Kernel::getY() == Kernel::rowsCount())
        Kernel::insRow("");
    
    Kernel::insCh(c);
    moveRight();
}

void Editor::delCh()
{
    
    if(Kernel::getX() == Kernel::strLen()) {
        Kernel::moveDown();
        std::string buff = Kernel::eraseRow();
        Kernel::moveUp();
        Kernel::insStr(std::move(buff));
        int count = buff.size();
        while(count--)
            moveLeft();
    }
    else {
        Kernel::eraseCh();
    }
}

void Editor::bsCh()
{
    moveLeft();
    delCh();
}

void Editor::insRowBelow(std::string&& str)
{
    moveDown();
    Kernel::insRow(str);
    moveUp();
}

void Editor::delRowBelow()
{
    int pos_x = Kernel::getX();

    goToRowStart();
    moveDown();
    Kernel::eraseRow();
    moveUp();

    while(pos_x--)
        moveRight();
}

void Editor::insTab()
{
    int count = TABSIZE;
    while(count--)
        insCh(' ');
}

void Editor::addNewLine()
{
    std::string buff = &Kernel::getStr()[Kernel::getX()];
    for (int i = 0; i < buff.size(); i++)
    {
        delCh();
    }
    moveDown();
    Kernel::insRow(buff);
}

void Editor::insRowBefore(std::string&& str)
{
    Kernel::insRow(str);
    moveDown();
}

void Editor::goToRowEnd()
{
    int pos_x = Kernel::getX();

    if(pos_x > Kernel::strSz()) {
        while(pos_x > Kernel::strSz()) {
            moveLeft();
            pos_x = Kernel::getX();
        }
    }
    else {
        while(pos_x < Kernel::strSz()) {
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

void Editor::goToFirstRow()
{
    int pos_y = Kernel::getY();

    while(pos_y > 0) {
        moveUp();
        pos_y = Kernel::getX();
    }
}

void Editor::moveUp()
{
    Kernel::moveUp();

    if(Kernel::getX() >= Kernel::strSz())
        goToRowEnd();

    if(Kernel::getY() == Kernel::rowsCount())
    goToRowStart();
}

void Editor::moveDown()
{
    Kernel::moveDown();

    if(Kernel::getX() > Kernel::strSz())
        goToRowEnd();

    if(Kernel::getY() == Kernel::rowsCount())
        goToRowStart();
}

void Editor::moveRight()
{
    if(Kernel::getY() == Kernel::rowsCount())
        return;

    if(Kernel::getX() >= Kernel::strSz()) {
        Kernel::moveDown();
        goToRowStart();
        return;
    }
    Kernel::moveRight();
}

void Editor::moveLeft()
{
    int x_pos = Kernel::getX();
    int y_pos = Kernel::getY();

    if(x_pos <= 0 && y_pos <=0)
        return;
    if(x_pos <= 0) {
        Kernel::moveUp();
        goToRowEnd();
        return;
    }
    Kernel::moveLeft();
}

void Editor::downPage()
{
    int count = height-menu_height-1;   
    int pos_y = getcury(stdscr);

    while(pos_y != count) {         //move to down edge of screen
        if(pos_y == Kernel::rowsCount())
            return;
        moveDown();
        pos_y = getcury(stdscr);
    }
    while(count--)                  //move on 1 screen Width below
        moveDown();
}

void Editor::upPage()
{
   int count = height-menu_height;     
   int pos_y = getcury(stdscr);
    while(pos_y != 0) {         //move to up edge of screen
        moveUp();
        pos_y = getcury(stdscr);
    }
    while(count--)                  //move on 1 screen Width upper
        moveUp();
}

int Editor::searchWordInText(std::string&& str)
{
    int pos_y = Kernel::getY();
    int pos_x = Kernel::getX();
    int pos = searchWordInRow(str,pos_x);

    if(pos >= 0)
        return pos;

    goToRowStart();
    while (pos_y != Kernel::rowsCount())
    {
        moveDown();
        if((pos = searchWordInRow(str)) >= 0)
            return pos;
        pos_y = Kernel::getY();
    }
    goToRowEnd();
    return -1;
}

int Editor::searchWordInRow(std::string str,int i)
{
    int pos;
    int y = Kernel::getY();

    if(y == Kernel::rowsCount())
       return -1;

    if((pos = Kernel::getStr().find(str,i)) != std::string::npos)
        return pos-i;

    return -1;
}

void Editor::searchStr()
{
    std::string str;
    int count;
    chtype c;

    while (1) {
        Kernel::eraseMsg();
        Kernel::addMsg("ESC to cancel | ");
        Kernel::addMsg(std::move(str));
        Kernel::showScreen();

        c = Kernel::getKey();
        if(c == KEY_ESC) {
            Kernel::eraseMsg();
            Kernel::showScreen();
            return;
        }
        if(c == '\n') {
            break;
        }
        if( c == KEY_BACKSPACE || c == KEY_DL) {
            auto it = str.end()-1;
            if(it >= str.begin())
                str.erase(it);
        }
        else if(isgraph(c)){
            str.push_back(c);
        }
    }
    count = searchWordInText(std::move(str));
    if(count > 0)
        while (count--)
            moveRight();

    Kernel::eraseMsg();
    Kernel::showScreen();
}

void Editor::ReadFromFile(std::string name)
{
    Kernel::setFileName(name);
    std::ifstream file(name);
    if(!file.is_open())
        return;

    std::string buff;
    while(1) {
        std::getline(file,buff);
        insRowBefore(std::move(buff));
        if(file.eof())
            break;
    }
    
    file.close();
    goToRowStart();
    goToFirstRow();
    Kernel::setModif(false);
}

int Editor::processKey(chtype c)
{
    switch (c) {
        case CTRL_Q:
            if(Kernel::getModif() == false) {
                return EXIT_MODE;
            }
            else {
                Kernel::addMsg("All changes not will be saved! You are sure? y or n ");
                Kernel::showScreen();
                while(1) {
                    c = Kernel::getKey();
                    if(tolower(c) == 'y')
                        return EXIT_MODE;
                    if(tolower(c) == 'n') {
                        Kernel::eraseMsg();
                        Kernel::showScreen();
                        return CONTINUE_MODE;
                    }
                }
            }
            break;
        case CTRL_A:
            insRowBelow("");
            break;
        case CTRL_S:           
            if(Kernel::writeInFile() == FILE_WRITTEN)
                Kernel::setModif(false);
            else
                Kernel::setModif(true);
            break;

        case CTRL_D:
            searchStr();
            break;

        case HOR_TAB:
            insTab();
            Kernel::setModif(true);
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
            delCh();
            Kernel::setModif(true);
            break;

            case KEY_DL:
                delRowBelow();
            break;

        case KEY_BACKSPACE:  
            bsCh();
            break;

        default:
            insCh(c);
            Kernel::setModif(true);
            break;
    }
    return CONTINUE_MODE;
}

void Editor::start(std::string path)
{
    ReadFromFile(path);

    while(1) {
        Kernel::showScreen();
        if(processKey(getch()) == EXIT_MODE)
            return;
    }
}