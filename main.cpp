#include "Editor.hpp"

Editor E;
int extern height, width;

void goToRowEnd()
{
    int pos_x = getcurx(stdscr) + E.xoff;

    if(pos_x > E.strSz()) {
        while(pos_x > E.strSz()) {
            moveLeft();
            pos_x = getcurx(stdscr) + E.xoff;
        }
    }
    else {
        while(pos_x < E.strSz()) {
            moveRight();
            pos_x = getcurx(stdscr) + E.xoff;
        }
    }
}

void goToRowStart()
{
    int pos_x = getcurx(stdscr) + E.xoff;

    while(pos_x > 0) {
        moveLeft();
        pos_x = getcurx(stdscr) + E.xoff;
    }
}

void goToFirstRow()
{
    int pos_y = getcury(stdscr) + E.yoff;

    while(pos_y > 0) {
        moveUp();
        pos_y = getcury(stdscr) + E.yoff;
    }
}

void goToLastRow()
{
    int pos_y = getcury(stdscr) + E.yoff;

    while(pos_y < E.rowsCount()) {
        moveDown();
        pos_y = getcury(stdscr) + E.yoff;
    }
}

chtype getKey()
{
    chtype c;
    while((c = getch()) == ERR) {};
    return c;
}

void moveLeft()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_x+E.xoff == 0) {     //if should go throught left edge, its go to end of prev row
        if(pos_y+E.yoff == 0)  //leave if its first line
            return;
        moveUp();
        goToRowEnd();
        return;
    }

    if(pos_x > 0) {
        move(pos_y,pos_x-1);
    }
    else {
        if(E.xoff > 0) 
            E.xoff--;
    }
}

void moveRight()
{
    
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_y == E.rowsCount()) {     //check if row not init '~'
        move(pos_y,0);
    }
    else if(pos_x+E.xoff == E.strSz()) {   //if should go throught size of row, its go to start of next row
        moveDown();
        goToRowStart();
    }
    else if(pos_x+E.xoff < E.strSz()) {    // move cursor or add offset
        if(pos_x == width-1) 
            E.xoff++;
        else 
            move(pos_y,pos_x+1);
    }
}

void moveDown()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_y+E.yoff < E.rowsCount()) {
        if(pos_y < height-1-MENU_HEIGHT) 
            move(pos_y+1,pos_x);
        else
            E.yoff++;
    }

    if(pos_y+E.yoff >= E.rowsCount()) {
        pos_y = getcury(stdscr);
        move(pos_y,0);
    }
    else if(pos_x+E.xoff > E.strSz())
        goToRowEnd();
}

void moveUp()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_y > 0) {
        move(pos_y-1,pos_x);
    }
    else {
        if(E.yoff > 0) {
            E.yoff--;
        }
    }

    if(pos_x+E.xoff > E.strSz())
        goToRowEnd();
}

void insertRowBelow(std::string str)
{
    int pos_y = getcury(stdscr);

    moveDown();
    E.insertNewRow(str);

    pos_y = getcury(stdscr);
    move(pos_y,0);
    E.xoff = 0;
}

void insertRowBefore(std::string str)
{
    E.setModif(true);
    E.insertNewRow(str);
    moveDown();
}

void backSpaceCh()
{
    moveLeft();
    deleteSym();
}

void addChar(char c)
{
    int pos_y = getcury(stdscr) + E.yoff;

    if(pos_y == E.rowsCount()) {     // if cursor located on not init row '~'
        insertRowBelow("");
        return;
    }

    E.insertChar((char)c);         
    moveRight();
}

void searchStr()
{
    std::string str;
    int count;
    chtype c;

    while (1) {
        E.eraseMessage();
        E.addMessage("ESC to cancel | ");
        E.addMessage(str);
        E.showScreen();

        c = getKey();
        if(c == KEY_ESC) {
            E.eraseMessage();
            E.showScreen();
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
        else if(isalnum(c)){
            str.push_back(c);
        }
    }
    count = E.searchWordInText(str);
    if(count > 0)
        while (count--)
            moveRight();

    E.eraseMessage();
    E.showScreen();
}

void upPage()
{
   int count = height-MENU_HEIGHT;     
   int pos_y = getcury(stdscr);
    while(pos_y != 0) {         //move to up edge of screen
        moveUp();
        pos_y = getcury(stdscr);
    }
    while(count--)                  //move on 1 screen Width upper
        moveUp();
}

void downPage()
{
    int count = height-MENU_HEIGHT-1;   
    int pos_y = getcury(stdscr);

    while(pos_y != count) {         //move to down edge of screen
        if(pos_y == E.rowsCount())
            return;
        moveDown();
        pos_y = getcury(stdscr);
    }
    while(count--)                  //move on 1 screen Width below
        moveDown();
}

void addNewLine()
{
    E.setModif(true);
    int pos_y = getcury(stdscr)+E.yoff;

    if(pos_y == E.rowsCount()) {
        insertRowBelow("");
        return;
    }

    int pos_x = getcurx(stdscr)+E.xoff;
    std::string str = &E.getRowStr()[pos_x];
    int count = str.length();

    goToRowEnd();
    while(count--)
        backSpaceCh();

    insertRowBelow(str);
}

void insTab()
{
    int count = TABSIZE;
    while(count--)
        addChar(' ');
}

int insChar(chtype& c)
{
    if(!isgraph(c) && c != ' ')
        return 0;
    addChar(c);
    return 1;
}

void deleteSym()
{
    int pos_x = getcurx(stdscr)+E.xoff;

    if(pos_x == E.strSz())
        E.delRowBelow();
    else
        E.delChar();
}


void endProg(const int a)
{
    noraw();
    echo();
    clear();
    keypad(stdscr,FALSE);
    refresh();
    endwin();
    exit(a);
}

void openFile(std::string name)
{
    E.setFileName(name);
    std::ifstream file(name);
    if(!file.is_open())
        return;

    std::string buff;
    while(1) {
        std::getline(file,buff);
        insertRowBefore(buff);
        if(file.eof())
            break;
    }
    
    file.close();
    goToRowStart();
    goToFirstRow();
    E.setModif(false);
}

void keyProcess()
{
    chtype c = getKey();
    std::string str;
    switch (c) {
        case CTRL_Q:
            if(E.getModif() == false) {
                endProg(0);
            }
            else {
                E.addMessage("All changes not will be saved! You are sure? y or n ");
                E.showScreen();
                while(1) {
                    c = getKey();
                    if(tolower(c) == 'y')
                        endProg(0);
                    if(tolower(c) == 'n') {
                        E.eraseMessage();
                        E.showScreen();
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

void initEditor()
{
    E.init();
    halfdelay(0);
    move(0,0);
    raw();
    noecho();
    keypad(stdscr,TRUE);
}

int main(int argc, char* argv[]) {
    initscr();
    initEditor();

    if(argv[1] != nullptr)
        openFile(argv[1]);
    else
        openFile("");

    while (1) {
        getmaxyx(stdscr,height,width);
        E.showScreen();  
        keyProcess();
    }
    endProg(0);
}