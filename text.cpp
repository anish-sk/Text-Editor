/*** includes ***/

#define _DEFAULT_SOURCE //feature test macros
#define _BSD_SOURCE

#include<bits/stdc++.h>
#include<fstream>
#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
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
    int rx; //horizontal coordinate for render string
    int rowoff; //row offset for vertical scrolling
    int coloff; // column offset for horizontal scrolling
    int screenrows; //screen height
    int screencols; //screen width
    int numrows; //number of rows in the file
    vector<string> row; //stores the rows of the file
    vector<string> render; //actual characters to print on the screen (for dealing with tabs and non printable characters)
    int dirty; //indicates whether the file has unsaved changes or not
    string filename; //filename for displaying in status bar
    char statusmsg[80]; //status message
    time_t statusmsg_time; //time for which status message will be displayed
    termios orig_termios;
    // editorConfig(){
    //     cx = cy = 0;
    //     numrows = 0;
    //     if(getWindowSize(&screenrows, &screencols) == -1) die("getWindowSize");
    //     row = vector<string>();
    // }
};

editorConfig E;

/** prototypes ***/

void editorSetStatusMessage(const char *fmt, ...);

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

    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw)==-1) die("tcsetattr"); //passing the modified instance to write the new terminal attributes back out
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

/*convert row index to render index*/
int editorRowCxtoRx(string &row, int cx){
    int rx = 0;
    for(int j=0; j<cx; j++){
        if(row[j] == '\t')
            rx += (EDITOR_TAB_STOP - 1) - (rx % EDITOR_TAB_STOP); //advance to the next tab stop
        rx++;
    }
    return rx;
}

/*fill the render string using row*/
string editorUpdateRow(string row){
    string render="";
    int idx=0;
    for(auto i: row){
        if(i=='\t'){
            render+=' ';idx++;
            while(idx % EDITOR_TAB_STOP != 0){//we advance till the next tab stop which is a column divisible by the tab_stop
                render+=' ';idx++;
            }
        }
        else{
            render+=i; idx++;
        }
    }
    return render;
}

/*insert a row into the editor*/
void editorInsertRow(int at, string s, int len){
    if(at == E.row.size()){
        E.row.push_back(s.substr(0,len)); 
        E.render.push_back(editorUpdateRow(s.substr(0,len)));
    }
    else{
        E.row.insert(E.row.begin()+at,s.substr(0,len)); 
        E.render.insert(E.render.begin()+at,editorUpdateRow(s.substr(0,len)));
    }
    E.numrows++;
    E.dirty++;
}

/* deleting a row */
void editorDelRow(int at){
    if(at < 0 || at >= E.numrows) return;
    E.row.erase(E.row.begin()+at);
    E.render.erase(E.render.begin()+at);
    E.numrows--;
    E.dirty++;
}

/* insert a character into row*/
void editorRowInsertChar(string &row, int at, char c, int pos){
    if(at < 0 || at > row.size()){
        at = row.size();
    }
    row.insert(row.begin()+at,c);
    E.render[pos] = editorUpdateRow(row);
    E.dirty++;
}

/* Appending one row to another */
void editorRowAppendString(string &row, string to_append, int pos){
    row+=to_append;
    E.render[pos] = editorUpdateRow(row);
    E.dirty++;
}

/* delete a character from row*/
void editorRowDelChar(string &row, int at, int pos){
    if(at < 0 || at >= row.size()) return;
    row.erase(row.begin()+at);
    E.render[pos] = editorUpdateRow(row);
    E.dirty++;
}

/*** editor operations***/

/* insert a character in the current position */
void editorInsertChar(char c){
    if(E.cy == E.numrows){
        editorInsertRow(E.numrows,"",0); //If the cursor is at the end we insert an empty row
    }
    editorRowInsertChar(E.row[E.cy], E.cx, c, E.cy);
    E.cx++;
}

/* insert newline when Enter key is pressed*/
void editorInsertNewline(){
    if(E.cx == 0){
        editorInsertRow(E.cy,"",0);//If we are at the beginnig of a line , we just need to insert a blank row above
    }
    else{
        string row = E.row[E.cy];
        editorInsertRow(E.cy+1,row.substr(E.cx),row.size()-E.cx); //We insert the characters that are to the right of the cursor
        E.row[E.cy].erase(E.cx); //We truncate the current row;
        row = E.row[E.cy];
        E.render[E.cy]= editorUpdateRow(row);
    }
    E.cy++;
    E.cx=0;
}

/*deletes a character from the current position */
void editorDelChar(){
    if(E.cy == E.numrows) return;
    if(E.cx == 0 && E.cy == 0) return;
    if(E.cx > 0){
        editorRowDelChar(E.row[E.cy], E.cx-1, E.cy);
        E.cx--;
    }else{
        E.cx = E.row[E.cy-1].size(); //Pressing backspace from the beginning of a line.
        editorRowAppendString(E.row[E.cy-1], E.row[E.cy],E.cy-1); //We append the current row too the previous row
        editorDelRow(E.cy); //The current row is deleted
        E.cy--;
    }
}

/*** file i/o ***/

/* concatenating the rows of the file into a single string */
void editorRowsToString(string &buf){
    for(int j=0;j<E.numrows; j++){
        buf+=E.row[j];
        buf+='\n';
    }
} 

/*opening and reading a file from disk*/
void editorOpen(char *filename){
    ifstream fp(filename);
    E.filename = string(filename);
    if(fp.fail()) die("fopen"); //open the file given by the user

    string line;
    getline(fp, line);
    while(!fp.eof())
        {
            int linelen= line.size();
            if(linelen= -1){
                while(linelen > 0 && (line[linelen-1]=='\n' || line[linelen-1]=='\r')) 
                    linelen--;
            editorInsertRow(E.numrows,line, linelen);
            getline(fp, line);
        }
    }
    fp.close();
    E.dirty = 0;
}

/* saving the file */
void editorSave(){
    if(E.filename == "") return;

    string buf="";
    editorRowsToString(buf);

    int fd = open(E.filename.c_str(),O_RDWR | O_CREAT, 0644); //opening the file 
    if(fd != -1){
        if(ftruncate(fd, buf.size())!=-1){ //truncating the file to the give length
            if(write(fd, buf.c_str(), buf.size())== buf.size()){
                close(fd);
                E.dirty = 0;
                editorSetStatusMessage("%d bytes written to disk", buf.size());
                return;
            }
        }
        close(fd);
    }
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/*** output ***/

/*scrolling the editor*/
void editorScroll(){
    E.rx = 0;
    if(E.cy < E.numrows){
        E.rx = editorRowCxtoRx(E.row[E.cy], E.cx);
    }
    if(E.cy < E.rowoff){
        E.rowoff = E.cy;
    }
    if(E.cy >= E.rowoff + E.screenrows){
        E.rowoff = E.cy - E.screenrows + 1;
    }
    if(E.rx < E.coloff){
        E.coloff = E.rx;
    }
    if(E.rx >= E.coloff + E.screencols){
        E.coloff = E.rx - E.screencols + 1;
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
            int len = (int)E.render[filerow].size()-E.coloff;
            if(len < 0) len = 0;
            if(len > E.screencols) len = E.screencols;
            if(E.coloff >= E.render[filerow].size()) ;//ab+="";
            else ab+=E.render[filerow].substr(E.coloff,len); //rendering considering the offset
        }
        ab+="\x1b[K"; //clears the next line  
        ab+="\r\n"; //we dont print a newline character in the last line
    }
}

/* drawing status bar at the bottom */
void editorDrawStatusBar(string &ab){
    ab+="\x1b[7m"; //switch to inverted colours
    string status;
    if(E.filename == ""){
        status+="[No Name]";
    }
    else{
        status+=E.filename.substr(0,20); //adding filename to status
        status+=" - " + to_string(E.numrows) + " lines"; //adding number of lines in file to status
        status+= E.dirty?"(modified)" : "";
    }
    ab+=status.substr(0,E.screencols); //cut the status string short in case it does not fit inside the width of the window
    string rstatus=to_string(E.cy+1)+"/"+to_string(E.numrows); //showing current line number
    for(int len= min(E.screencols,(int)status.size()); len < E.screencols; len++){
        if(E.screencols - len == rstatus.size()){ //right aligning the current line number
            ab+=rstatus;break;
        }
        ab+=" ";
    }
    ab+="\x1b[m"; //switch back to normal colours
    ab+="\r\n";//line for status message
}

/* drawing message bar */
void editorDrawMessageBar(string &ab){
    ab+="\x1b[K"; //clear the message bar
    int msglen = strlen(E.statusmsg);
    if(msglen > E.screencols) msglen = E.screencols;
    if(msglen && (time(NULL) - E.statusmsg_time < 5))
        ab+=string(E.statusmsg).substr(0,msglen); //We will display the status message if it is less than 5 seconds old
}

/* refreshing the screen */
void editorRefreshScreen(){
    editorScroll(); //calls the function to scroll the editor

    string ab=""; //append buffer
    ab+="\x1b[?25l"; //hides the cursor
    ab+="\x1b[H"; //repositions the cursor to the top left of the screen using H command

    editorDrawRows(ab);
    editorDrawStatusBar(ab);
    editorDrawMessageBar(ab);

    ab+="\x1b["+to_string(E.cy-E.rowoff+1)+";"+to_string(E.rx-E.coloff+1)+"H";  //repositions the cursor to its position along with scrolling
    ab+="\x1b[?25h"; //shows the cursor
    write(STDOUT_FILENO, ab.c_str(), ab.size()); //one single write is better than multiple writes to avoid flicker effects
}

/*set status message*/
void editorSetStatusMessage(const char *fmt, ...){
    //this function takes a format string and a variable number of arguments, like the printf family ( a variadic function)
    va_list ap;
    va_start(ap, fmt); //The last argument before ... must be passed to va_start()
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap); //vsnprintf() reads the format string and calls va_arg() for each argument
    va_end(ap);
    E.statusmsg_time = time(NULL);
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
            else if(E.cy > 0){
                E.cy--;
                E.cx = E.row[E.cy].size(); // Allowing the user to press the left arrow key at the beginning of the line to move to the end of the previous line. 
            }
            break;
        case ARROW_RIGHT:
            if(row!="" && (E.cx < (int)row.size())){ //preventing cursor to move past the line end
                E.cx++;
            }
            else if(row!="" && E.cx == (int)row.size()){
                E.cy++;
                E.cx = 0; // Allowing the user to press the right key at the end of a line to go to the beginning of the next line.
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
    static int quit_times = EDITOR_QUIT_TIMES;

    int c = editorReadKey();

    switch(c){
        case '\r':
            editorInsertNewline();
            break;

        case CTRL_KEY('q'):
            if(E.dirty && quit_times > 0){
                editorSetStatusMessage("WARNING!!! File has unsaved changes. Press Ctrl-Q %d more times to quit.", quit_times);
                //warning the user of unsaved changes before quitting the editor
                quit_times -- ;
                return;
            }
            write(STDOUT_FILENO, "\x1b[2J", 4); //clears the screen
            write(STDOUT_FILENO, "\x1b[H",3); //repositions the cursor to the top left of the screen 
            exit(0); //quit when you read 'Ctrl-Q'
            break;

        case CTRL_KEY('s'):
            editorSave(); //Ctrl-S for save
            break;

        case HOME_KEY:
            E.cx = 0;
            break;

        case END_KEY:
            if(E.cy < E.numrows)
                E.cx = E.row[E.cy].size();
            break;
            
        case BACKSPACE:
        case CTRL_KEY('h'):
        case DEL_KEY:
            if(c == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
            editorDelChar();
            break;

        case PAGE_UP:
        case PAGE_DOWN:
            {
                if(c == PAGE_UP){
                    E.cy = E.rowoff;
                }
                else if(c == PAGE_DOWN){
                    E.cy = E.rowoff + E.screenrows - 1;
                    if(E.cy > E.numrows) E.cy = E.numrows;
                }
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
        
        case CTRL_KEY('l'):
        case '\x1b':
            break;
        
        default:
            editorInsertChar(c);
            break;
    }
    quit_times = EDITOR_QUIT_TIMES;
}

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
    E.dirty = 0;
    E.screenrows -=2;
    E.filename = "";
    E.statusmsg[0] = '\0';
    E.statusmsg_time = 0;
}

int main(int argc, char *argv[]){

    enableRawMode();
    initEditor();
    if(argc >= 2) {
        editorOpen(argv[1]); //reading file given by user.
    }
    
    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");

    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    } 
    return 0;
}
