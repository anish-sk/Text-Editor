#include"output.h"

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
            string s="";
            ustring h=ustring();
            if(E.coloff<E.render[filerow].size()){
                s = E.render[filerow].substr(E.coloff);//rendering considering the offset.
                h = E.hl[filerow].substr(E.coloff); ///for rendering highlights.
            }
            int current_color = -1;
            int j;
            for(j=0;j<len && s.size();j++){
                if(iscntrl(s[j])){
                    char sym = (s[j]<=26)?'@'+s[j]:'?'; //render Ctrl-'X' as 'X' and other nonprintable characters as '?'
                    ab+="\x1b[7m";//switch to inverted colours
                    ab+=sym;
                    ab+="\x1b[m";//turn off inverted colours
                    if(current_color!=-1){
                        ab+="\x1b["+to_string(current_color)+"m"; //change to the required color
                    }
                }
                else if(h[j] == HL_NORMAL){
                    if(current_color!=-1){
                        ab+="\x1b[39m"; //redering normal characters in default color.
                        current_color=-1;
                    }
                    ab+=s[j];
                }
                else{
                    int color = editorSyntaxToColor(h[j]);
                    if(color!=current_color){
                        current_color = color;                        
                        ab+="\x1b["+to_string(color)+"m"; //change to the required color
                    }
                    ab+=s[j];
                }
            }
            ab+="\x1b[39m"; //changing back to default color
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
    string ftype;//filetype information
    if(!E.syntax){
        ftype = "no ft";
    }
    else
    {
        ftype = E.syntax->filetype;
    }
    
    string rstatus= ftype + " | " + to_string(E.cy+1)+"/"+to_string(E.numrows); //showing current line number
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