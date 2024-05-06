#include "Editor.hpp"

int width,height;

void Editor::init()
{
    xoff = 0;
    yoff = 0;
    modif = false;
    rows.push_back(""); 
}

std::string Editor::getRowStr()
{
    int y = getcury(stdscr) + yoff;
    return rows[y];
}
void Editor::setFileName(std::string str) //
{
    if(str.empty()) {
        displaying_name = "Unnamed";
        return;
    }
    file_name = str;

    auto it = str.end()-1;
    auto b = str.begin();

    while(it-- > b)
        if(*it == '/' || *it == '\\')
            break;
    it++;
    
    displaying_name.clear();
    displaying_name.append(&(*it));
}

void Editor::insertChar(char c)
{
    int temp = getcurx(stdscr)+xoff;
    int y = getcury(stdscr)+yoff;
    auto it = rows[y].begin();

    while(temp--)
        it++;
    rows[y].insert(it,c);
}

void Editor::insertNewRow(std::string str)
{
    int temp = getcury(stdscr)+yoff;
    auto it = rows.begin();

    while(temp--)
        it++;
    rows.insert(it,str);
}


void Editor::printMenu()
{

    int sx = getcurx(stdscr);
    int sy = getcury(stdscr);

    move(height-MENU_HEIGHT,0);
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

int Editor::getAllSymCount()
{
    int res = 0;
    for(auto& el : rows)
        res += el.length();
    return res;
}

void Editor::addMessage(std::string str)
{
    message.append(str);
}

void Editor::eraseMessage()
{
    message.clear();
}

void Editor::printRows()
{
    int sx = getcurx(stdscr);
    int sy = getcury(stdscr);
    int ty = 0;

    clear();
    move(0,0);
    for(int y = yoff; y < height-MENU_HEIGHT+yoff; y++) {
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

void Editor::delChar()
{
    int temp = getcurx(stdscr)+xoff;
    int y = getcury(stdscr)+yoff;
    
    auto it = rows[y].begin();
    while(temp--)
        it++;
    if(it < rows[y].end())
        rows[y].erase(it);
}

void Editor::delRowBelow()               
{
    moveDown();

    auto it = rows.begin();         //str to erase
    int y = getcury(stdscr)+yoff;

    moveUp();
    goToRowEnd();

    while(y--)
        it++;

    auto prev_it = it-1;           //str to save data from erased row

    if(it < rows.end()) {
        if(prev_it >= rows.begin())
            prev_it->append(it->c_str());
            
        rows.erase(it);
    }
}

int Editor::rowsCount() { return rows.size(); }

int Editor::strSz() 
{ 
    int y = getcury(stdscr)+yoff;
    return (y >= rows.size())? 0 :rows[y].size(); 
}

void Editor::showScreen()
{
    printRows();
    printMenu();
    refresh();
}

std::string Editor::getFileName() 
{
    std::string buff;
    chtype c;
    addMessage("ECS to cancel | Enter name of the file: ");
    showScreen();
    while(c = getch()) {
        if(c == KEY_ESC){
            eraseMessage();
            showScreen();
            return "";
        }
        if(c == '\n' && !buff.empty())
            break;
        if(c == '\n')
            continue;
        if( c == KEY_BACKSPACE || c == KEY_DL) {
            if(buff.end()-1 >= buff.begin())
                buff.erase(buff.end()-1);
        }
        else if(isalnum(c) || c == '.')
            buff.push_back(c);

        addMessage(buff);
        showScreen();
        eraseMessage();
        addMessage("ECS to cancel | Enter name of the file: ");
    }   
    return buff;
}

void Editor::writeInFile()
{
    if(file_name.empty()) {
        setFileName(getFileName());
        eraseMessage();
        showScreen();
    }
    

    std::ofstream file(file_name);
    if(!file.is_open()) {
        addMessage("Cant open file, enter filename again. ");
        setFileName(getFileName());
        return;
    }

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

int Editor::searchWordInText(std::string str)
{
    int pos_y = getcury(stdscr) + yoff;
    int pos_x = getcurx(stdscr) + xoff;
    int pos = searchWordInRow(str,pos_x);

    if(pos >= 0)
        return pos;

    goToRowStart();
    while (pos_y != rowsCount())
    {
        moveDown();
        if((pos = searchWordInRow(str)) >= 0)
            return pos;
        pos_y = getcury(stdscr) + yoff;
    }
    goToRowEnd();

    return -1;
}

int Editor::searchWordInRow(std::string str,int i)
{
    int pos;
    int y = getcury(stdscr) + yoff;

    if(y == rowsCount())
       return -1;

    if((pos = rows[y].find(str,i)) != std::string::npos)
        return pos-i;

    return -1;
}

void Editor::setModif(bool)
{
    modif = true;
}

bool Editor::getModif()
{
    return modif;
}