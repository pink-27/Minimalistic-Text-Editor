#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include "Line.hpp"
#include "Cursor.hpp"
#include "ControlBuffer.hpp"

using namespace std;

ControlBuffer::ControlBuffer(Cursor &cursor) : cursor(cursor) {}
void ControlBuffer::insertCharAtCursor(char &input)
{
    string buf = "";
    buf += input;
    controlBuffer.insert(this->cursor.column, buf);
    write(STDOUT_FILENO, "\x1b[@", 3); // Insert Character sequence
    write(STDOUT_FILENO, &input, 1);   // Write the actual character
}

void ControlBuffer::deleteChar()
{
    if (cursor.column == 1)
        return;

    moveLeft();
    write(STDOUT_FILENO, "\x1b[P", 3);
    updateCursor();
    controlBuffer.erase(controlBuffer.begin() + this->cursor.column);
}

void ControlBuffer::updateCursor()
{
    cursor.getCurrentPos();

    while (cursor.column >= controlBuffer.size())
    {
        controlBuffer.push_back(' ');
    }
    cursor.getCurrentPos();
}

void ControlBuffer::moveLeft()
{
    int row = cursor.row, col = cursor.column;
    if (cursor.column > 1)
    {
        col = cursor.column - 1;
    }

    cursor.moveCursorTo(row, col);
    updateCursor();
}

void ControlBuffer::moveRight()
{
    int row = cursor.row, col = cursor.column;
    if (cursor.column < controlBuffer.size() - 1)
    {
        col = cursor.column + 1;
        row = cursor.row;
    }

    cursor.moveCursorTo(row, col);
    updateCursor();
}