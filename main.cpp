/*** includes ***/

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

#include"editorConfig.h"
#include"editorHighlight.h"
#include"editorKey.h"
#include"editorSyntax.h"
#include"find.h"
#include"fileio.h"
#include"input.h"
#include"output.h"
#include"rowops.h"
#include"syntaxHighlight.h"
#include"terminal.h"

using namespace std;

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

typedef std::basic_string<unsigned char> ustring;


/*** data ***/
editorConfig E = editorConfig();

/*** filetypes ***/

vector<string> C_HL_extensions = {".c",".h",".cpp"}; //list of extensions for C/C++ type files
vector<string> C_HL_keywords ={
    "switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef", "static", "enum", "class","case",

    "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "string|", "vector|", "string|", "set|", "unordered_map|", "unordered_set|", "greater|", "pair|" 
};//set of keywords and common data types(terminated by a pipe)

vector<editorSyntax> HLDB = {
    editorSyntax(   "c",
                    C_HL_extensions,
                    C_HL_keywords,
                    "//","/*","*/",
                    HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS)
}; //highlight database

/*** init ***/

/* initializes editor with window size*/
void initEditor(){
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

int main(int argc, char *argv[]){
    enableRawMode();
    initEditor();
    if(argc >= 2) {
        editorOpen(argv[1]); //reading file given by user.
    }
    
    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    } 
    return 0;
}
