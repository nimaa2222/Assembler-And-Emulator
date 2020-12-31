//
//        File access to source file.
//        Opens and reads from the file 
#ifndef _FILEACCESS_H
#define _FILEACCESS_H 

class FileAccess
{

public:

    // Opens the file.
    FileAccess(int argc, char *argv[]);

    // Closes the file.
    ~FileAccess();

    // Get the next line from the source file.
    bool GetNextLine(string &);

    // Put the file pointer back to the beginning of the file
    void Rewind();

private:

    ifstream m_sfile;        // Source file object.
};

#endif
