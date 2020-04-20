#ifndef E_SYNTAX_H
#define E_SYNTAX_H

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

/*syntax highlighting information for the file*/
class editorSyntax{
    public:
    string filetype; //filetype displayed to the user
    vector<string> filematch; //list of strings where each string contains a pattern for the filename to match against
    vector<string> keywords; //list of keywords;
    string singleline_comment_start; // single-line comment pattern
    string multiline_comment_start; //multi-line comment start
    string multiline_comment_end; //multi-line comment end
    int flags; //bit field which contains flags for whether to highlight numbers and strings.
    editorSyntax(string filetype, vector<string> filematch, vector<string> keywords, string singleline_comment_start, string multiline_comment_start, string multiline_comment_end, int flags){
        this->filetype = filetype;
        this->filematch = filematch;
        this->keywords = keywords;
        this->singleline_comment_start = singleline_comment_start;
        this->multiline_comment_start = multiline_comment_start;
        this->multiline_comment_end = multiline_comment_end;
        this->flags = flags;
    }
    
};

#endif