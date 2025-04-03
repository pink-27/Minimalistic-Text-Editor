

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include "Line.hpp"
#include "Cursor.hpp"
#include "ControlBuffer.hpp"

#define CLEAR_SCREEN "\x1b[2J\x1b[H"
#define ESCAPE_KEY '\x1b'
#define BACKSPACE_KEY 127
#define QUIT_KEY '\x11'
#define ENTER_KEY '\n'
#ifndef TEXTEDITOR_HPP // Start of header guard
#define TEXTEDITOR_HPP
#define MOVE_DOWN "\x1b[B"
#define MOVE_UP "\x1b[A"
#define MOVE_LEFT "\x1b[D"
#define MOVE_RIGHT "\x1b[C"
#define SAVE 19
#define SPACEBAR 32

enum Mode
{
    CONTROL,
    EDIT
};

class TextEditor
{
private:
    termios orig_termios, raw;
    int mode, termWidth;
    Mode currentMode = EDIT;

public:
    std::vector<Line> lines;
    Cursor cursor;
    ControlBuffer controlBuffer;
    string filename;

    TextEditor(Cursor &cursor, string &filename);
    // ~TextEditor();
    void disable_raw_mode();
    void openFile();
    void enable_raw_mode();
    void reset_termios();
    void processInput();
    void renderScreen();
    void processEditMode(char &input);
    void updateCursor();
    void moveLeft();
    void moveRight();
    void moveDown();
    void moveUp();
    void insertLine();
    void deleteChar();
    void saveToFile();
    void insertCharAtCursor(char &input);
    int getTerminalWidth();
    void processControlMode(char &input);
    void enterControlMode();
    void exitControlMode();
    void takeControlModeAction();
};

#endif // TEXTEDITOR_HPP  // End of header guard
