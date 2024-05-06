#include "Kernel.hpp"

int extern height,width;

void Kernel::printRows()
{
    int sx = getcurx(stdscr);
    int sy = getcury(stdscr);
    int ty = 0;

    clear();
    move(0,0);
    for(int y = yoff; y < height-menu_height+yoff; y++) {
        if(y < rows.size()) {
            int sz = rows[y].size();
            if(sz-xoff > 0) {
                insnstr(&rows[y].c_str()[xoff],rows[y].size()-xoff);
            }
        }
        else
            addch('~');
        ty++;
        move(ty,0);
    }
    move(sy,sx);
}

void Kernel::printMenu()
{

    int sx = getcurx(stdscr);
    int sy = getcury(stdscr);

    move(height-menu_height,0);
    if(message.empty()) {
        std::string to_print;
        to_print.append("| CTRL+Q to exit |");

        if(modif) {
            to_print.append(" Modified |");
        }
        else {
                to_print.append(" Saved |");
        }
            to_print.push_back(' ');
        int sz = width-to_print.length()-1;
        if(sz < displaying_name.length()) {
            std::string dots = "...";
            sz -= dots.length();
            for (int i = 0; i < sz; i++)
            {
                    to_print.push_back(displaying_name[i]);
            }
            to_print.append(dots);
            to_print.push_back('|');
            printw(to_print.c_str());
        }
        else
            printw("%s %s |",to_print.c_str(),displaying_name.c_str());
        move(height-MENU_HEIGHT+1,0);
        printw("| CTRL+S to save |");
        printw(" CTRL+D to search |");
        printw("row %d|",sy+yoff+1);
        printw("col %d|",sx+xoff+1);
        printw("rows %ld|",rows.size());
        printw("cols %d|",getAllSymCount());
    }
    else{
        printw("%s",message.c_str());
    }
    move(sy,sx);
}

void Kernel::showScreen()
{
    printRows();
    printMenu();
    refresh();
}

void Kernel::setMenuHeight(int h)
{
    menu_height = h;
}

int Kernel::getX()
{
    return getcurx(stdscr)+xoff;
}

int Kernel::getY()
{
    return getcury(stdscr) + yoff;
}

int Kernel::strLen()
{
    return rows[getY()].length();
}

int Kernel::rowsCount()
{
    return rows.size();
}

void Kernel::printRows()
{
    int sx = getcurx(stdscr);
    int sy = getcury(stdscr);
    int ty = 0;

    clear();
    move(0,0);
    for(int y = yoff; y < height-menu_height+yoff; y++) {
        if(y < rows.size()) {
            int sz = rows[y].size();
            if(sz-xoff > 0) {
                insnstr(&rows[y].c_str()[xoff],rows[y].size()-xoff);
            }
        }
        else
            addch('~');
        ty++;
        move(ty,0);
    }
    move(sy,sx);
}

void Kernel::addMsg(std::string str)
{
    message.append(str);
}

void Kernel::eraseMsg()
{
    message.clear();
}

void Kernel::moveUp()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_y > 0) {
        move(pos_y-1,pos_x);
    }
    else {
        if(yoff > 0) {
            yoff--;
        }
    }
}

void Kernel::moveDown()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_y+yoff < rows.size()) {
        if(pos_y < height-1-menu_height) 
            move(pos_y+1,pos_x);
        else
            yoff++;
    }

    if(pos_y+yoff >= rows.size()) {
        pos_y = getcury(stdscr);
        move(pos_y,0);
    }
}

void Kernel::moveRight()
{
    
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr)+yoff;

    if(pos_y == rows.size()) {     //check if row not init '~'
        move(pos_y,0);
    }
    else if(pos_x+xoff < rows[pos_y].size()) {    // move cursor or add offset
        if(pos_x == width-1) 
            xoff++;
        else 
            move(pos_y,pos_x+1);
    }
}

void Kernel::moveLeft()
{
    int pos_x = getcurx(stdscr);
    int pos_y = getcury(stdscr);

    if(pos_x > 0) {
        move(pos_y,pos_x-1);
    }
    else {
        if(xoff > 0) 
            xoff--;
    }
}

void Kernel::insCh(char)
{
    int temp = getX();
    int y = getY();
    auto it = rows[y].begin();

    while(temp--)
        it++;

    if(it < rows[y].size()) {
        rows[y].insert(it,c);
    }
    
}

void Kernel::insStr(std::string str)
{
    int temp = getcurx(stdscr)+xoff;
    int y = getcury(stdscr)+yoff;
    auto it = rows[y].begin();
    std::string buff;

    while(temp-- && it < rows[y].end()) {
        buff.append(*it);
        it++;
    }

    if(it < rows[y].size()) {
        buff.append(str);
        while(temp-- && it < rows[y].end()) {
            buff.append(*it);
            it++;
        }
        rows[y] = buff;
    }
}

void Kernel::insRow(std::string str)
{
    int temp = getcury(stdscr)+yoff;
    auto it = rows.begin();

    while(temp--)
        it++;

    if(it < rows.end())
        rows.insert(it,str);
}

void Kernel::eraseCh()
{
    int temp = getX();
    
    auto it = rows[getY()].begin();
    while(temp--)
        it++;
    if(it < rows[y].end())
        rows[y].erase(it);
}

std::string Kernel::eraseRow()
{
    int temp = getY();

    auto it = rows.begin();
    while(temp--)
        it++;
    
    if(temp < rows.end()) {
        std::string row = *it;
        rows.erase(it);
        return row;
    }

    return nullptr;
}