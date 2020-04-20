#include"editorConfig.h"
#include"fileio.h"

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
    fstream fp;
    if(filesystem::exists(filename)){
        fp = fstream(filename);
    }
    else fp = fstream(filename, fstream::in | fstream::out | fstream::trunc);
    E.filename = string(filename);
    
    editorSelectSyntaxHighlight(); //calling syntaxhighlight when the filename changes

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
    if(E.filename == ""){
        E.filename = string(editorPrompt("Save as: %s (ESC to cancel)", NULL));
        if(E.filename == ""){
            editorSetStatusMessage("Save aborted");
            return;
        }
        editorSelectSyntaxHighlight(); //calling syntaxhighlight when the filename changes

    } 

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

