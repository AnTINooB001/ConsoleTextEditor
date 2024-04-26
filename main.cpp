#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
//#include <map>

int width,height,sx,sy;

class editor
{
private:
    std::vector<std::string> rows;
public:
    int xoff,yoff;
    editor(): xoff{0},yoff{0} { rows.push_back(""); };

    void insertChar(char c)
    {
        int temp = getcurx(stdscr)+xoff;
        int y = getcury(stdscr)+yoff;
        
        auto it = rows[y].begin();
        while(temp--)
            it++;
        rows[y].insert(it,c);
    }

    void insertNewRow(std::string str)
    {
        int y = getcury(stdscr);
        int temp = y+yoff;
        auto it = rows.begin();
        while(temp--)
            it++;
        rows.insert(it,str);
    }

    void printRows()
    {
        sx = getcurx(stdscr);
        sy = getcury(stdscr);
        int ty = 0;
        clear();
        move(0,0);
        for(int y = 0; y < height; y++) {
            if(y+yoff < rows.size()) {
                int sz = rows[y+yoff].size();
                if(sz-xoff > 0) {
                    insnstr(&rows[y+yoff].c_str()[xoff],rows[y+yoff].size()-xoff);
                }
            }
            else
                addch('~');
            ty++;
            move(ty,0);
        }
        move(sy,sx);
    }

    void delChar()
    {
        int temp = getcurx(stdscr)+xoff;
        int y = getcury(stdscr)+yoff;
        
        auto it = rows[y].begin();
        while(--temp)
            it++;
        rows[y].erase(it);

    }

    int rowsCount() { return rows.size(); }
    int strLen() { return rows[getcury(stdscr)+yoff].size(); }

    void writeInFile(std::string name)
{
    std::ofstream file(name);
    for (auto& el : rows)
    {
        file << el + "\n\0";
    }
    
    file.close();
}
};
editor E;

void moveLeft();
void moveRight();

void goToRowEnd()
{
    int pos_x = getcurx(stdscr);

    if(pos_x+E.xoff > E.strLen()) {
        while(pos_x+E.xoff != E.strLen()) {
            moveLeft();
            pos_x = getcurx(stdscr);
        }
    }
    else {
        while(pos_x+E.xoff != E.strLen()) {
            moveRight();
            pos_x = getcurx(stdscr);
        }
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

    if(pos_x+E.xoff < E.strLen()) {
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
        if(pos_y >= height-1) 
            E.yoff++;
        else
            move(pos_y+1,pos_x);
    }
    if(pos_x+E.xoff > E.strLen())
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
    if(pos_x+E.xoff > E.strLen())
        goToRowEnd();
}

void insertRowBack()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    moveDown();
    E.insertNewRow("");

    pos_y = getcury(stdscr);
    move(pos_y,0);
    E.xoff = 0;
}

void insertRowFront()
{
    insertRowBack();

    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    move(pos_y-1,pos_x);
}

void endProg(const int a)
{
    echo();
    exit(a);
}

void keyProcess()
{
    chtype c = getKey();
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);
    switch (c) {
        case 'q':
            E.writeInFile("name.txt");
            endProg(0);

        case KEY_LEFT:
            if(pos_x+E.xoff == 0) {                        //if should go throught left edge, its go to end of prev row
                if(pos_y == 0)
                    break;
                moveUp();
                goToRowEnd();
            }
            else
                moveLeft();
            break;

            case KEY_RIGHT:
                if(pos_y+E.yoff == E.rowsCount()) {     //check if row not init '~'
                    move(pos_y,0);
                }
                else if(pos_x+E.xoff == E.strLen()) {   //if should go throught size of row, its go to start of next row
                    moveDown();
                    pos_y = getcury(stdscr);
                    move(pos_y,0);
                }
                else
                    moveRight();
            break;

            case KEY_DOWN:
                moveDown();
                pos_y = getcury(stdscr);
                if(pos_y+E.yoff == E.rowsCount()) {
                    move(pos_y,0);
                }
            break;

            case KEY_UP:
                moveUp();
            break;

        case '\n':
        case KEY_ENTER:
            insertRowBack();
            break;

        case KEY_IC: //
        case KEY_BACKSPACE:
            if(pos_x+E.xoff > 0) {
                E.delChar();
                moveLeft();
            }
            break;

        default:
            if(pos_y+E.yoff == E.rowsCount()) {     // if cursor located on not init row '~'
                insertRowBack();
            }
            if(pos_x >= width-1) {                  // if string longer then screen
                E.insertChar((char)c);
                E.xoff++;
            } 
            else {                                  // just add symbol
                E.insertChar((char)c);              
                moveRight();
            }
            break;
    }
    
}

int main( ) {
    initscr();

    getmaxyx(stdscr,height,width);
    
    halfdelay(1);
    move(0,0);
    noecho();
    keypad(stdscr,TRUE);

    while (1) {
        E.printRows();  
        keyProcess();
    }
    clear();
    endwin();
}