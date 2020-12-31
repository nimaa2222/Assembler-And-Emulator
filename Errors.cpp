//
//  Implementation of the errors class.
//

#include "stdafx.h"

//"giving life" to static data members
vector<string> Errors::m_ErrorMsgs;
map<int, string> Errors::m_ErrorList;

/*
NAME
 
    DisplayErrors - Displays the collected error messages

SYNOPSIS
 
    void DisplayErrors();

DESCRIPTION
 
   This function ouputs all the error messages that were recorded
   throughout the translation process. It will also display
   run-time errors after the emulator begins to run if there is any.
*/

void Errors::DisplayErrors()
{
    cout<<"LIST OF ERRORS:"<<endl<<endl;
    
    int errorIndex = 0;
    
    for (size_t i = 0; i < m_ErrorMsgs.size() / 2; i++)
    {
        // offending statement
        cout<<m_ErrorMsgs[errorIndex]<<endl;
        
        // error message
        cout<<"<ERROR: "<<m_ErrorMsgs[errorIndex+1]<<">"<<endl<<endl;
        
        // skip one because each pair is one error
        // (offending statement + error message)
        errorIndex += 2;
    }
}
/*void Errors::DisplayErrors(); */
