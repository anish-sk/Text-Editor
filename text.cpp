/*** includes ***/

#define _DEFAULT_SOURCE //feature test macros
#define _BSD_SOURCE

#include<bits/stdc++.h>
#include<fstream>
#include<ctype.h>
#include<errno.h>
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

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

enum editorKey{ //mapping of arrow keys
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

/*** data ***/

/* data type for storing  a row of text */
typedef class erow{
    int size;
    char *chars;
} erow;

/*editor configuration */
class editorConfig{
    public:
    int cx,cy; //cursor position
    int rowoff; //row offset for vertical scrolling
    int coloff; // column offset for horizontal scrolling
    int screenrows; //screen height
    int screencols; //screen width
    int numrows;
    vector<string> row;
    termios orig_termios;
    // editorConfig(){
    //     cx = cy = 0;
    //     numrows = 0;
    //     if(getWindowSize(&screenrows, &screencols) == -1) die("getWindowSize");
    //     row = vector<string>();
    // }
};

editorConfig E;

/*** terminal ***/

/*printing error message and exiting the program*/
void die(const char *s){
    write(STDOUT_FILENO, "\x1b[2J", 4); //clears the screen
    write(STDOUT_FILENO, "\x1b[H",3); //repositions the cursor to the top left of the screen 
    perror(s);
    exit(1);
}

/*disabling raw mode*/
void disableRawMode(){
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &E.orig_termios)==-1)
    die("tcsetattr");
}

/*enabling raw mode*/
void enableRawMode(){    
    if(tcgetattr(STDIN_FILENO, &E.orig_termios)==-1) die("tcgetattr"); //reading the current attributes into a global class instance
    atexit(disableRawMode); //we register our disableRawMode() function to be called automatically when the program exits.

    termios raw = E.orig_termios; // we assign the original instance to another instance to make changes
    /*(IXON)Turning Ctrl-S and Ctrl-Q flags off
    Fixing Ctrl-M
    turning off some miscellaneous flags which are generally turned off by default*/
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP );
    /*preventing translation of "\n" to "\r\n"*/
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    /*setting ECHO (which causes each key we type to be printed to the terminal) feature, 
    ICANON(which turns on canonical mode where we read line by line) feature, 
    ISIG(which turns off SIGINT(Ctrl-C) and SIGTSTP(Ctrl-Z)) to false
    IEXTEN(which turns of Ctrl-V)*/
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0; //setting the minimum number of bytes of inout needed before read() can return
    raw.c_cc[VTIME] = 1; //setting the maximum amount of time to wait before read() returns

    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw)==-1) die("tcsetattr"); //passing the modified instance to wrtie the new terminal attributes back out
}

/*returns a keypress*/
int editorReadKey(){
    int nread;
    char c;
    while((nread=read(STDIN_FILENO,&c,1))!=1){
       if(nread==-1 && errno != EAGAIN) die("read"); 
    }

    if(c == '\x1b'){ //reading escape sequences as a single keypress
        char seq[3];

        if(read(STDOUT_FILENO,&seq[0],1)!=1) return '\x1b';
        if(read(STDOUT_FILENO,&seq[1],1)!=1) return '\x1b';

        if(seq[0]=='['){
            if(seq[1] >= '0' && seq[1] <= '9'){
                if(read(STDIN_FILENO,&seq[2],1)!=1) return '\x1b';
                if(seq[2] == '~'){
                    switch(seq[1]){
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            }
            else{
                switch(seq[1]){
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }                        
        }
        else if(seq[0] == '0'){
                switch(seq[1]){
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        return '\x1b';
    }
    else{
        return c;
    }
    
}

/* find cursor position */
int getCursorPosition(int *rows, int *cols){
    char buf[32];
    unsigned int i = 0;

    if(write(STDOUT_FILENO,"\x1b[6n",4)!=4) return -1; //n command gives the reply in the form of an escape sequence to the input
    
    while(i < sizeof(buf) - 1){ //read from stdin tilll we get R and put it into buf
        if(read(STDIN_FILENO, &buf[i], 1)!=1) break;
        if(buf[i] == 'R') break;
        i++;
    }
    buf[i]='\0';

    if(buf[0]!= '\x1b' || buf[1] != '[') return -1; // making sure the reply is an escape sequence
    if(sscanf(&buf[2], "%d;%d",rows,cols)!=2) return -1; //setting rows and cols from the output of n command
    
    return 0;
}

/*get size of the terminal*/
int getWindowSize(int *rows, int *cols){
    winsize ws;
    //ioctl places the number of columns wide and the number of rows high the terminal is into the winsize object
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        //ioctl doesnt work on all systems
        if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B",12) != 12) return -1; //moving the cursor to the bottom right using B and C command
        return getCursorPosition(rows,cols); //uses cursor position to find window size
    }
    else{
        *cols = ws.ws_col; 
        *rows = ws.ws_row;
        return 0;
    }
}

/*** row operations ***/

/*add a row to the editor*/
void editorAppendRow(string s, int len){
    E.row.push_back(s.substr(0,len)); 
    E.numrows++;
}
/*** file i/o ***/

/*opening and reading a file from disk*/
void editorOpen(char *filename){
    ifstream fp(filename);
    if(fp.fail()) die("fopen"); //open the file given by the user

    string line;
    getline(fp, line);
    while(!fp.eof())
        {
            int linelen= line.size();
            if(linelen= -1){
                while(linelen > 0 && (line[linelen-1]=='\n' || line[linelen-1]=='\r')) 
                    linelen--;
            editorAppendRow(line, linelen);
            getline(fp, line);
        }
    }
    fp.close();
}
/*** output ***/

/*scrolling the editor*/
void editorScroll(){
    if(E.cy < E.rowoff){
        E.rowoff = E.cy;
    }
    if(E.cy >= E.rowoff + E.screenrows){
        E.rowoff = E.cy - E.screenrows + 1;
    }
    if(E.cx < E.coloff){
        E.coloff = E.cx;
    }
    if(E.cx >= E.coloff + E.screencols){
        E.coloff = E.cx - E.screencols + 1;
    }
}

/* drawing ~ */
void editorDrawRows(string &ab){
    int y;
    for(y=0;y<E.screenrows;y++){
        int filerow = y + E.rowoff;
        if(filerow >= E.numrows){
            if(E.numrows == 0 && y == E.screenrows/3){
                string welcome = "Text Editor -- version ";
                welcome+=EDITOR_VERSION;
                int welcomelen = welcome.size();
                if(welcomelen > E.screencols) welcomelen = E.screencols;
                int padding = (E.screencols - welcomelen)/2;
                if(padding){
                    ab+="~";
                    padding--;
                }
                while(padding--){ab+=" ";} //centering the message
                ab+=welcome.substr(0,welcomelen);
            }
            else{
                ab+="~";
            }
        }
        else{
            int len = min(max((int)E.row[filerow].size()-E.coloff,0),E.screencols);
            ab+=E.row[filerow].substr(E.coloff,len);
        }
        ab+="\x1b[K"; //clears the next line  
        if(y < E.screenrows - 1)
        {ab+="\r\n";} //we dont print a newline character in the last line
    }
}

/* refreshing the screen */
void editorRefreshScreen(){
    editorScroll(); //calls the function to scroll the editor

    string ab=""; //append buffer
    ab+="\x1b[?25l"; //hides the cursor
    ab+="\x1b[H"; //repositions the cursor to the top left of the screen using H command

    editorDrawRows(ab);

    ab+="\x1b["+to_string(E.cy-E.rowoff+1)+";"+to_string(E.cx-E.coloff+1)+"H";  //repositions the cursor to its position along with scrolling
    ab+="\x1b[?25h"; //shows the cursor
    write(STDOUT_FILENO, ab.c_str(), ab.size()); //one single write is better than multiple writes to avoid flicker effects
}

/*** input ***/

/* moves cursor according to key pressed */
void editorMoveCursor(int key){
    string row = (E.cy >= E.numrows) ? "" : E.row[E.cy]; 
    switch(key){
        case ARROW_LEFT:
            if(E.cx != 0){
                E.cx--;
            }            
            break;
        case ARROW_RIGHT:
            if(row!="" && (E.cx < (int)row.size())){ //preventing cursor to move past the line end
                E.cx++;
            }
            break;
        case ARROW_UP:
            if(E.cy != 0){
                E.cy--;
            }            
            break;
        case ARROW_DOWN:
            if(E.cy < E.numrows){
                E.cy++;
            }            
            break;
    }
    row = (E.cy >= E.numrows) ? "" : E.row[E.cy];
    if(E.cx > row.size()){
        E.cx = row.size(); //snapping cursor to end of line
    }
}

/* handling a keypress */
void editorProcessKeypress(){
    int c = editorReadKey();

    switch(c){
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4); //clears the screen
            write(STDOUT_FILENO, "\x1b[H",3); //repositions the cursor to the top left of the screen 
            exit(0); //quit when you read 'Ctrl-Q'
            break;

        case HOME_KEY:
            E.cx = 0;
            break;

        case END_KEY:
            E.cx = E.screencols - 1;
            break;
        
        case PAGE_UP:
        case PAGE_DOWN:
            {
                int times = E.screenrows;
                while(times--){
                    editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
                }
            }
            break;
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
        editorMoveCursor(c);
        break;
    }
}

/*** init ***/

/* initializes editor with window size*/
void initEditor(){
    E.cx = E.cy = 0;
    E.rowoff = E.coloff = 0;
    E.numrows = 0;
    if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
    E.row = vector<string>();
}

int main(int argc, char *argv[]){

    enableRawMode();
    initEditor();
    if(argc >= 2) {
        editorOpen(argv[1]); //reading file given by user.
    }
    
    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    } 
    return 0;
}
