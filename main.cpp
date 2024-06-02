#include "TEditor.hpp"

int main(int argc, char* argv[]) {

    Editor e(3);
    if(argv[1] != nullptr)
        e.start(argv[1]);
    else
        e.start("");
}