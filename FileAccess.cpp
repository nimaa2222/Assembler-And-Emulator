//
//  Implementation of file access class.
//

#include "stdafx.h"

FileAccess::FileAccess(int argc, char *argv[])
{
    // Check that there is exactly one run time parameter
    if(argc != 2)
    {
        cerr << "Usage: Assem <FileName>" << endl;
        exit(1);
    }
    
    // Open the file
    m_sfile.open(argv[1], ios::in);

    // If the open failed, report the error and terminate.
    if(!m_sfile)
    {
        cerr << "Source file could not be opened, assembler terminated." << endl;
        exit(1);
    }
}

FileAccess::~FileAccess()
{
    m_sfile.close();
}


/*
NAME
 
    GetNextLine - Gets the next line from the file

SYNOPSIS
 
    bool GetNextLine(string &a_buff);

DESCRIPTION
 
   This function gets the next line from the file
   and stores it in "a_buff".
 
   Returns false - If end of file is reached
   Returns true - Otherwise
*/

bool FileAccess::GetNextLine(string &a_buff)
{
    // If there is no more data
    if (m_sfile.eof())
        return false;
    
    getline(m_sfile, a_buff);
    
    return true;
}
/*bool FileAccess::GetNextLine(string &a_buff); */


/*
NAME
 
    Rewind - Goes to the beginning of file

SYNOPSIS
 
    void Rewind();

DESCRIPTION
 
   This function cleans all file flags and
   goes back to the beginning of the file.
   
*/

void FileAccess::Rewind()
{
    m_sfile.clear();
    m_sfile.seekg(0, ios::beg);
}
/*void FileAccess::rewind(); */
    
