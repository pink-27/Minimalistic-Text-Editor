#ifndef CURSOR_HPP // ✅ If CURSOR_HPP is not defined
#define CURSOR_HPP // ✅ Define it to prevent multiple inclusion
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include "Line.hpp"
using namespace std;

class Cursor
{
public:
    int row;
    int column;
    void getCurrentPos();
    void moveCursorTo(int row, int column);
    Cursor();
};
#endif