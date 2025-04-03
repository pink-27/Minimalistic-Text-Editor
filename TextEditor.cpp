#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include "Line.hpp"
#include "Cursor.hpp"
#include "TextEditor.hpp"
#include <sys/ioctl.h>
#include <fstream>

using namespace std;

int lastEditRow, lastEditCol;
TextEditor::TextEditor(Cursor &cursor, string &filename)
    : cursor(cursor), controlBuffer(cursor) // Use shared reference
{
    this->lines.push_back(Line());
    this->termWidth = getTerminalWidth();
    this->filename = filename;
    enable_raw_mode();
    openFile();
}
void TextEditor::openFile()
{
    fstream myFile;
    myFile.open(filename, ios::in);
    string line;
    while (getline(myFile, line))
    {
        this->lines.push_back(Line());
        this->lines.back().line = " ";
        this->lines.back().line += line;
    }
    write(STDOUT_FILENO, "\x1b[H", 3); // Move cursor to the top-left
    write(STDOUT_FILENO, "\x1b[J", 3); // Clear screen from cursor onward
    renderScreen();
}

void TextEditor::renderScreen()
{
    for (int i = 1; i < lines.size(); i++)
    {
        for (int j = 1; j < lines[i].line.size(); j++)
        {
            cursor.moveCursorTo(i, j);
            write(STDOUT_FILENO, &lines[i].line[j], 1);
            cout.flush(); // Ensure the output is printed
        }
    }
    updateCursor();
}

void TextEditor::disable_raw_mode()
{
    raw = orig_termios;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void TextEditor::enable_raw_mode(void)
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    {
        perror("tcgetattr");
        exit(1);
    }

    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        perror("tcsetattr");
        exit(1);
    }
}

void TextEditor::reset_termios()
{

    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void TextEditor::processInput()
{
    while (1)
    {

        char input;
        if (currentMode == EDIT)
        {
            processEditMode(input);
        }
        else
        {
            processControlMode(input);
        }
    }
}

void TextEditor::updateCursor()
{
    cursor.getCurrentPos();
    while (cursor.row >= lines.size())
    {
        lines.push_back(Line());
    }
    while (cursor.column >= lines[cursor.row].line.size())
    {
        lines[cursor.row].line.push_back(' ');
    }
    cursor.getCurrentPos();
}

void TextEditor::moveLeft()
{
    int row = cursor.row, col = cursor.column;
    if (cursor.column > 1)
    {
        col = cursor.column - 1;
        row = cursor.row;
    }
    else
    {
        if (cursor.row > 1)
        {
            row = cursor.row - 1;
            col = lines[row].line.size() - 1;
        }
    }
    cursor.moveCursorTo(row, col);
    updateCursor();
}

void TextEditor::moveRight()
{
    int row = cursor.row, col = cursor.column;
    if (cursor.column < lines[row].line.size() - 1)
    {
        col = cursor.column + 1;
        row = cursor.row;
    }
    else
    {
        if (cursor.row < lines.size() - 1)
        {
            row = cursor.row + 1;
            col = 1;
        }
    }
    cursor.moveCursorTo(row, col);
    updateCursor();
}

void TextEditor::moveDown()
{
    int row = cursor.row, col = cursor.column;
    if (cursor.row < lines.size() - 1)
    {
        row = cursor.row + 1;
        col = cursor.column;
    }
    cursor.moveCursorTo(row, col);
    updateCursor();
}
void TextEditor::moveUp()
{
    int row = cursor.row, col = cursor.column;
    if (cursor.row > 1)
    {
        row = cursor.row - 1;
        col = cursor.column;
    }
    cursor.moveCursorTo(row, col);
    updateCursor();
}
void TextEditor::deleteChar()
{

    updateCursor();

    if (cursor.column == 1)
    {
        if (cursor.row == 1)
            return;
        int oldCol = cursor.column, oldRow = cursor.row;
        int upRowsz = lines[oldRow - 1].line.size();
        for (int i = 1; i < lines[oldRow].line.size(); i++)
        {
            lines[oldRow - 1].line.push_back(lines[oldRow].line[i]);
        }
        lines.erase(lines.begin() + oldRow);
        cursor.moveCursorTo(oldRow, 1);
        write(STDOUT_FILENO, "\x1b[2K", 4); // Clear entire line
        write(STDOUT_FILENO, "\x1b[M", 3);  // delete entire line
        for (int i = upRowsz; i < lines[oldRow - 1].line.size(); i++)
        {
            cursor.moveCursorTo(oldRow - 1, i);

            write(STDOUT_FILENO, &lines[oldRow - 1].line[i], 1);
        }
        cursor.moveCursorTo(oldRow - 1, upRowsz);
        return;
    }

    moveLeft();
    write(STDOUT_FILENO, "\x1b[P", 3);
    updateCursor();
    lines[cursor.row].line.erase(lines[cursor.row].line.begin() + cursor.column);
}

void TextEditor::insertLine()
{
    updateCursor();

    lines.insert(lines.begin() + cursor.row + 1, Line());

    int newRow = cursor.row + 1;
    int oldCol = cursor.column;
    int oldRow = cursor.row;

    vector<char> charactersToMove;
    for (int i = oldCol; i < lines[oldRow].line.size(); i++)
    {
        charactersToMove.push_back(lines[oldRow].line[i]);
    }

    lines[oldRow].line.erase(
        lines[oldRow].line.begin() + oldCol,
        lines[oldRow].line.end());

    lines[newRow].line.insert(
        lines[newRow].line.end(),
        charactersToMove.begin(),
        charactersToMove.end());

    cursor.moveCursorTo(newRow, 1);
    write(STDOUT_FILENO, "\x1b[L", 3);

    cursor.moveCursorTo(oldRow, 1);
    write(STDOUT_FILENO, "\x1b[2K", 4);

    for (int i = 1; i < lines[oldRow].line.size(); i++)
    {
        write(STDOUT_FILENO, &lines[oldRow].line[i], 1);
    }

    cursor.moveCursorTo(newRow, 1);
    for (int i = 1; i < lines[newRow].line.size(); i++)
    {
        write(STDOUT_FILENO, &lines[newRow].line[i], 1);
    }

    cursor.moveCursorTo(newRow, 1);
    updateCursor();
}

int TextEditor::getTerminalWidth()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void TextEditor::saveToFile()
{
    ofstream file(this->filename);
    if (!file)
    {
        perror("Error opening file");
        return;
    }
    int cnt = 0;
    for (const auto &line : lines)
    {
        if (!cnt)
        {
            cnt++;
            continue;
        }
        for (int i = 1; i < line.line.size(); i++)
        {
            file << line.line[i];
        }
        file << endl;
    }
    cout << endl;
    file.close();
}

void TextEditor::insertCharAtCursor(char &input)
{
    updateCursor();

    if (cursor.column == this->termWidth)
    {
        write(STDOUT_FILENO, &input, 1); // Write the actual character

        insertLine();
        updateCursor();
        return;
    }
    lines[cursor.row].line.insert(lines[cursor.row].line.begin() + cursor.column, input);
    write(STDOUT_FILENO, "\x1b[@", 3); // Insert Character sequence
    write(STDOUT_FILENO, &input, 1);   // Write the actual character
    moveRight();

    return;
}
void TextEditor::processEditMode(char &input)
{
    updateCursor();
    ssize_t bytesRead = read(STDIN_FILENO, &input, 1);
    if (bytesRead <= 0)
    {
        perror("Error reading input");
        exit(1);
    }

    if (input == QUIT_KEY)
    {
        exit(0);
    }
    else if (int(input) == BACKSPACE_KEY)
    {
        deleteChar();
    }
    else if (input == ESCAPE_KEY)
    {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000; // 10ms timeout

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        if (result == 0)
        {
            currentMode = CONTROL;
            updateCursor();
            lastEditCol = cursor.column;
            lastEditRow = cursor.row;
            enterControlMode();
            cursor.moveCursorTo(lines.size() + 1, 1);
            // updateCursor();
            return;
        }

        char seq[2];
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);

        if (seq[0] == '[')
        {
            if (seq[1] == 'D') // left
            {
                updateCursor();
                moveLeft();
            }
            else if (seq[1] == 'C') // right
            {
                updateCursor();
                moveRight();
            }
            else if (seq[1] == 'A') // up
            {
                updateCursor();
                moveUp();
            }
            else if (seq[1] == 'B') // down
            {
                updateCursor();
                moveDown();
            }
            updateCursor();
        }
    }
    else if (input == ENTER_KEY)
    {
        insertLine();
    }
    else if (input == SAVE)
    {
        saveToFile();
        exit(0);
    }
    else
    {
        insertCharAtCursor(input);
    }
}

void TextEditor::enterControlMode()
{
    for (int i = 1; i <= lines.size() - 1; i++)
    {
        cursor.moveCursorTo(i, 1);
        write(STDOUT_FILENO, "\x1b[@", 3);
        write(STDOUT_FILENO, "\x1b[@", 3);
        write(STDOUT_FILENO, "~", 1);
        cursor.moveCursorTo(i, 2);

        write(STDOUT_FILENO, " ", 1);
    }
}
void TextEditor::exitControlMode()
{
    write(STDOUT_FILENO, "\x1b[2K\r", 4);
    controlBuffer.controlBuffer.clear();

    for (int i = 1; i <= lines.size() - 1; i++)
    {
        cursor.moveCursorTo(i, 1);
        write(STDOUT_FILENO, "\x1b[P", 3);
        write(STDOUT_FILENO, "\x1b[P", 3);
    }
}

void TextEditor::takeControlModeAction()
{
    string action = controlBuffer.controlBuffer;
    while (action.back() == ' ')
    {
        action.pop_back();
    }
    reverse(action.begin(), action.end());
    while (action.back() == ' ')
    {
        action.pop_back();
    }
    reverse(action.begin(), action.end());

    if (action.size() == 0)
        return;
    if (action == ":w")
    {
        exitControlMode();
        saveToFile();
        cursor.moveCursorTo(lastEditRow, lastEditCol);
        currentMode = EDIT;

        updateCursor();
        return;
    }
    if (action == ":wq")
    {
        saveToFile();
        exit(1);
        return;
    }
    if (action == ":i")
    {
        exitControlMode();
        cursor.moveCursorTo(lastEditRow, lastEditCol);
        currentMode = EDIT;
        updateCursor();
        return;
    }
}
void TextEditor::processControlMode(char &input)
{

    controlBuffer.updateCursor();
    ssize_t bytesRead = read(STDIN_FILENO, &input, 1);
    if (bytesRead <= 0)
    {
        perror("Error reading input");
        exit(1);
    }

    if (input == QUIT_KEY)
    {
        exit(0);
    }
    else if (int(input) == BACKSPACE_KEY)
    {
        controlBuffer.deleteChar();
    }
    else if (input == ESCAPE_KEY)
    {
        struct timeval timeout;
        timeout.tv_sec = 0;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        // if (result == 0)
        // {
        //     currentMode = EDIT;
        //     exitControlMode();
        //     cursor.moveCursorTo(lastEditRow, lastEditCol);
        //     updateCursor();
        //     return;
        // }

        char seq[2];
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);

        if (seq[0] == '[')
        {
            if (seq[1] == 'D') // left
            {
                controlBuffer.updateCursor();

                controlBuffer.moveLeft();
            }
            else if (seq[1] == 'C') // right
            {
                controlBuffer.updateCursor();
                controlBuffer.moveRight();
            }
            controlBuffer.updateCursor();
        }
    }
    else if (input == ENTER_KEY)
    {
        takeControlModeAction();
    }
    else
    {
        controlBuffer.insertCharAtCursor(input);
    }
}
