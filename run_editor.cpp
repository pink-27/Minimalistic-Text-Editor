#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include "Line.hpp"
#include "Cursor.hpp"
#include "TextEditor.hpp"
#include <fstream>
using namespace std;
int main(int argc, char *argv[])
{

    Cursor *cursor = new Cursor();
    string filename = argv[1];
    TextEditor *editor = new TextEditor(*cursor, filename);

    editor->processInput();
}