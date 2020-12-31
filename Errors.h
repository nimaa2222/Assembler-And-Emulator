//
// Class to manage error reporting.
// Records error codes that are passed into it

#ifndef _ERRORS_H
#define _ERRORS_H

#include "stdafx.h"

//Container for the list of all possible and recorded errors
class Errors
{

public:
    
    //contains a list of all possible errors
    Errors()
    {
        /*                        Errors involving operands                                  */
        
        m_ErrorList.insert(pair<int, string>(0, "Operand Must Be Symbolic"));
        //when operand for opcodes is numeric
        
        m_ErrorList.insert(pair<int, string>(1, "Operand Must Be Positive Integer"));
        //when operand for ORG or DS is negative
        
        m_ErrorList.insert(pair<int, string>(2, "Operand Must Be Numeric"));
        //when operand for ORG, DC, or DS is symbolic
        
        m_ErrorList.insert(pair<int, string>(3, "Extra Operands"));
        //when there are more than 4 words in an instruction
        
        m_ErrorList.insert(pair<int, string>(4, "Operand Too Large For Quack3200"));
        //when operand for DS has more than 5 digits
        
        m_ErrorList.insert(pair<int, string>(5, "Operand Exceeds Quack3200 Final Location"));
        //when DS or ORG operands make program go beyond 99,999
        
        /*                                                                                   */
        
        /*                        Errors involving symbols/labels                            */
        
        m_ErrorList.insert(pair<int, string>(6, "Multiply Defined Symbol"));
        //when a constant is defined more than once
        
        m_ErrorList.insert(pair<int, string>(7, "Undefined Symbol"));
        //when a constant cannot be found in symbol table
        
        m_ErrorList.insert(pair<int, string>(8, "Symbol Does Not Meet Quack3200 Symbol Specification"));
        // when symbol is not 1-10 char long OR
        // does not start with an alphabetical character OR the remaining
        // characters after first char are NOT numbers and alphabetical characters
        
        m_ErrorList.insert(pair<int, string>(9, "Multiply Defined Label"));
        //when a "label" (NOT a constant) is used twice but there is no jump to it
        //and so its address is not in memory to be detected by code 6
        
        /*                                                                                    */
        
        /*                        Errors involving HALT instruction                           */
                
        m_ErrorList.insert(pair<int, string>(10, "HALT Instruction Can Only Be Included Once"));
        
        m_ErrorList.insert(pair<int, string>(11, "HALT Instruction Before Location 100 Will Not Be Detected By Emulator"));
        //to prevent emulator from going to an infinite loop looking for a HALT Instruction that is not there
        
        m_ErrorList.insert(pair<int, string>(12, "Assembler Language Statements Are Not Allowed Before HALT Instruction"));
        m_ErrorList.insert(pair<int, string>(13, "Machine Language Statements Are Not Allowed After HALT Instruction"));
        m_ErrorList.insert(pair<int, string>(14, "No HALT Instruction Detected for Execution Termination"));
        
        /*                                                                                    */
               
        /*                        Errors involving END instruction                            */
    
        m_ErrorList.insert(pair<int, string>(15, "END Instruction Cannot Have A Label"));
        m_ErrorList.insert(pair<int, string>(16, "END Instruction Can Only Be Included Once"));
        m_ErrorList.insert(pair<int, string>(17, "No END Instruction Detected"));
        m_ErrorList.insert(pair<int, string>(18, "Only Comments Are Allowed After END Instruction"));
        
        /*                                                                                    */
        
        /*               Errors involving location and memory of Quack3200                    */
        
        m_ErrorList.insert(pair<int, string>(19, "Constant Too Large For Quack3200"));
        //when a large constant is defined OR a large constant is read as input
        
        m_ErrorList.insert(pair<int, string>(20, "Insufficient Memory For Translation"));
        //when location exceeds 99,999
        
        /*                                                                                    */
        
        /*                                Other Errors                                        */
        
        m_ErrorList.insert(pair<int, string>(21, "Invalid Register Specified"));
        //when register is not in range 0-9
        
        m_ErrorList.insert(pair<int, string>(22, "Invalid Assembly Language Statement"));
        //when no valid instruction can be retrieved from statement
        
        m_ErrorList.insert(pair<int, string>(23, "The Origin's Operand Must be Higher Than Current Location"));
        m_ErrorList.insert(pair<int, string>(24, "Comma Can Only Be Used To Separate Register From Operand"));
        
        /*                                                                                    */
        
        /*                                Run-Time Errors                                     */
        
        m_ErrorList.insert(pair<int, string>(25, "ADD Instruction Causes Overflow In a Register"));
        //when addition of a constant and a register results in a number too large to be stored in register
        
        m_ErrorList.insert(pair<int, string>(26, "SUB Instruction Causes Overflow In a Register"));
        //when subtraction of a constant and a register results in a number too large to be stored in register
        
        m_ErrorList.insert(pair<int, string>(27, "MULT Instruction Causes Overflow In a Register"));
        //when multiplication of a constant and a register results in a number too large to be stored in register
        
        m_ErrorList.insert(pair<int, string>(28, "Only Integers Are Supported by Quack3200"));
        //when input contains non-numeric characters
        
        m_ErrorList.insert(pair<int, string>(29, "Division By Zero Is Undefined"));
        //when register value is divided by 0
        
        m_ErrorList.insert(pair<int, string>(30, "Negative Sign Cannot Be Followed By 0"));
        //when a constant is defined as -0 (Ex: Duck dc -0)
        
        /*                                                                                    */
    }
    
    // Initializes error reports
    static void InitErrorReporting()
    {
        m_ErrorMsgs.clear();
    }
   
    // Records an error message
    static void RecordError(const int& a_errorCode, const string& a_orgStatement)
    {
        // record the original statement
        m_ErrorMsgs.push_back(a_orgStatement);
        
        // record the error message corresponding to the error code
        m_ErrorMsgs.push_back(m_ErrorList[a_errorCode]);
    }
    
    // Returns the total number of recorded errors
    static int NumErrors()
    {
        return (int)m_ErrorMsgs.size();
    }
    
    // Displays the collected error messages
    static void DisplayErrors();
    
    
private:
    
    static vector<string> m_ErrorMsgs;   // List of the recorded errors
    static map<int, string> m_ErrorList; // List of all possible errors
};

#endif


