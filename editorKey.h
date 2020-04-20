#ifndef E_KEY_H
#define E_KEY_H

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

using namespace std;

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

typedef std::basic_string<unsigned char> ustring;

enum editorKey{ //mapping of arrow keys
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};


#endif