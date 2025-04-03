#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include "Line.hpp"
#include "Cursor.hpp"
#include <sys/ioctl.h>

using namespace std;

#ifndef CONTROLBUFFER_HPP
#define CONTROLBUFFER_HPP

class ControlBuffer
{
    int position = 0;
    Cursor cursor;

public:
    string controlBuffer = " ";
    ControlBuffer(Cursor &cursor);
    // void processControlBuffer();
    void deleteChar();
    void insertCharAtCursor(char &input);
    void updateCursor();
    void moveLeft();
    void moveRight();
};
#endif