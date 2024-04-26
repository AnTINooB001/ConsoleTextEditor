#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <time.h>

#define KEY_ESC 27
#define MENU_HEIGHT 2
#define ctrl(x) ((x)&0x1f)
#define CTRL_S ctrl('s')

int width,height,sx,sy;
void moveLeft();
void moveRight();
void moveDown();
void moveUp();

class editor
{
private:
    std::vector<std::string> rows;
    std::string message;
    time_t curr_time;
public:
    std::string file_name;
    int xoff,yoff;
    bool modif;
    editor(): xoff{0},yoff{0} { rows.push_back(""); modif = false; curr_time = 0; };

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

    void printMenu()
    {
        int txtsz = 0;
        move(height-MENU_HEIGHT,0);
        if(curr_time == 0) {
            printw("| ESC to exit |");
            txtsz += 16;

            if(modif) {
                printw(" Modified |");
                txtsz += 12;
            }
            else {
                printw(" Saved |");
                txtsz += 9;
            }
            printw(" %s |",file_name.c_str());
        }
        else{
            int diff = time(NULL)-curr_time;
            if(time(NULL)-curr_time < 5)
                printw("%s",message.c_str());
            else
                curr_time = 0;
        }
    }

    void addMessage(std::string str)
    {
        message.append(str);
        curr_time = time(NULL);
    }

    void printRows()
    {
        sx = getcurx(stdscr);
        sy = getcury(stdscr);
        int ty = 0;
        clear();
        move(0,0);
        for(int y = 0; y < height-MENU_HEIGHT; y++) {
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
        printMenu();
        move(sy,sx);
        refresh();
    }

    void delChar()
    {
        int temp = getcurx(stdscr)+xoff;
        int y = getcury(stdscr)+yoff;
        
        auto it = rows[y].begin();
        while(temp--)
            it++;
        if(it < rows[y].end())
            rows[y].erase(it);

    }

    void delRow()               //delete row (not backspace), erase row below
    {
        sx = getcurx(stdscr);
        sy = getcury(stdscr);

        moveDown();
        int y = getcury(stdscr)+yoff;
        
        auto it = rows.begin();
        while(y--)
            it++;

         auto prev_it = it-1;
         if(it < rows.end())
             prev_it->append(it->c_str());

        if(it < rows.end())
            rows.erase(it);

        move(sy,sx);
        
    }

    int rowsCount() { return rows.size(); }
    int strLen() 
    { 
        int y = getcury(stdscr)+yoff;
        return (y >= rows.size())? 0 :rows[y].size(); 
    }

    void writeInFile()
    {
        std::ofstream file(file_name);
        if(!file.is_open())
            return;
        auto e = rows.end();

        for (auto it = rows.begin(); it < e; it++)
        {
            if(it == e-1)
                file << *it;
            else
                file << *it + "\n";
        }
    
        file.close();
        modif = false;
    }
};
editor E;

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

void goToRowStart()
{
    int pos_x = getcurx(stdscr);
    while(pos_x+E.xoff != 0) {
        moveLeft();
        pos_x = getcurx(stdscr);
    }
}

void goToFirstRow()
{
    int pos_y = getcury(stdscr);

    while(pos_y+E.yoff > 0) {
        moveUp();
        pos_y = getcury(stdscr);
    }
}

void goToLastRow()
{
    int pos_y = getcury(stdscr);

    while(pos_y+E.yoff < E.rowsCount()) {
        moveDown();
        pos_y = getcury(stdscr);
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

    if(pos_x+E.xoff == 0) {                        //if should go throught left edge, its go to end of prev row
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

    if(pos_y+E.yoff == E.rowsCount()) {     //check if row not init '~'
        move(pos_y,0);
    }
    else if(pos_x+E.xoff == E.strLen()) {   //if should go throught size of row, its go to start of next row
        moveDown();
        pos_x = getcurx(stdscr);
        goToRowStart();
    }
    else if(pos_x+E.xoff < E.strLen()) {    // move cursor or add offset
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
    int len = E.strLen();
    if(pos_y+E.yoff >= E.rowsCount()) {
        pos_y = getcury(stdscr);
        move(pos_y,0);
    }
    else if(pos_x+E.xoff > E.strLen())
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

void insertRowBack(std::string str)
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    moveDown();
    E.insertNewRow(str);

    pos_y = getcury(stdscr);
    move(pos_y,0);
    E.xoff = 0;
}

void insertRowFront(std::string str)
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    E.insertNewRow(str);
    E.delRow();
    moveDown();
    pos_y = getcury(stdscr);
    move(pos_y,0);
    E.xoff = 0;
}

void addChar(char c)
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_y+E.yoff == E.rowsCount()) {     // if cursor located on not init row '~'
        insertRowBack("");
    }

    if(pos_x >= width-1) {                  // if string longer then screen
        E.insertChar((char)c);
        E.xoff++;
    } 

    else {                                  // just add symbol
        E.insertChar((char)c);              
        moveRight();
    }
}

void endProg(const int a)
{
    echo();
    exit(a);
}

void openFile(std::string name)
{
    E.file_name = name;
    std::ifstream file(name);
    if(!file.is_open())
        return;
    std::string buff;
    while(1) {
        std::getline(file,buff);
        insertRowFront(buff);
        if(file.eof())
            break;
    }
    file.close();

    goToRowStart();
    goToFirstRow();
}

void keyProcess()
{
    static bool sexit = false;
    
    chtype c = getKey();
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);
    int count;
    switch (c) {
        case KEY_ESC:
            if(sexit == true || E.modif == false) {
                E.writeInFile();
                endProg(0);
            }
            else {
                sexit = true;
                E.addMessage("All changes not will be saved!");
            }
            break;
        
        case CTRL_S:
            E.writeInFile();
            break;

        case '\t':
            E.modif = true;
            count = TABSIZE;
            while(count--)
                addChar(' ');
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
            count = height-MENU_HEIGHT;      
            while(pos_y != count) {         //move to down edge of screen
                moveDown();
                pos_y = getcury(stdscr);
            }
            while(count--)                  //move on 1 screen Width below
                moveDown();
            break;

        case KEY_PPAGE:
            count = height-MENU_HEIGHT;     
            while(pos_y != 0) {         //move to up edge of screen
                moveUp();
                pos_y = getcury(stdscr);
            }
            while(count--)                  //move on 1 screen Width upper
                moveUp();
            break;
        
        case KEY_HOME:
            goToRowStart();
            break;

        case KEY_END:
            goToRowEnd();
            break;

        case '\n':
        case KEY_ENTER:
            E.modif = true;
            insertRowBack("");
            break;

        case KEY_DC: 
            E.modif = true;
            if(pos_x+E.xoff == E.strLen())
                E.delRow();
            else
                E.delChar();
            break;

        case KEY_BACKSPACE:  
            E.modif = true;                   
            if(pos_x+E.xoff == 0 && pos_y+E.yoff == 0)
                break;
            moveLeft();
            pos_x = getcurx(stdscr);
            if(pos_x+E.xoff == E.strLen())
                E.delRow();
            else
                E.delChar();
            break;

        default:
            E.modif = true;
            addChar(c);
            break;
    }
}

int main(int argc, char* argv[]) {
    initscr();

    getmaxyx(stdscr,height,width);

     if(argv[1] != nullptr)
        openFile(argv[1]);
    
    
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