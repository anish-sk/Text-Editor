#ifndef E_CONFIG_H
#define E_CONFIG_H

/*** includes ***/

#include<bits/stdc++.h>
#include<fstream>
#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<filesystem>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<termios.h>
#include<time.h>
#include<unistd.h>

#include"editorSyntax.h"
#include"terminal.h"

using namespace std;

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

typedef std::basic_string<unsigned char> ustring;

/*editor configuration */
class editorConfig{
    public:
    int cx,cy; //cursor position
    int rx; //horizontal coordinate for render string
    int rowoff; //row offset for vertical scrolling
    int coloff; // column offset for horizontal scrolling
    int screenrows; //screen height
    int screencols; //screen width
    int numrows; //number of rows in the file
    vector<string> row; //stores the rows of the file
    vector<string> render; //actual characters to print on the screen (for dealing with tabs and non printable characters)
    vector<ustring> hl; //Each value corresponds to a character in render and indicates the type of highlight
    //vector<int> idx; //
    vector<int> hl_open_comment;//stores whether the previous line is part of an unclosed multi line comment
    int dirty; //indicates whether the file has unsaved changes or not
    string filename; //filename for displaying in status bar
    char statusmsg[80]; //status message
    time_t statusmsg_time; //time for which status message will be displayed
    editorSyntax *syntax; //syntax information for the current file
    termios orig_termios;
    
    /* initializes editor with window size*/
    editorConfig(){
        editorConfig E = *this;
        E.cx = E.cy = 0;
        E.rx = 0;
        E.rowoff = E.coloff = 0;
        E.numrows = 0;
        if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
        E.row = vector<string>();
        E.render = vector<string>();
        E.hl_open_comment = vector<int>();
        E.dirty = 0;
        E.screenrows -=2;
        E.filename = "";
        E.statusmsg[0] = '\0';
        E.statusmsg_time = 0;
        E.syntax = NULL;
    }
};

extern editorConfig E;

#endif
