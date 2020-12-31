//
//  Implementation of assembler class.
//

#include "stdafx.h"
#include "Assembler.h"

// Constructor for the assembler.  Note: passing argc and argv to the file access constructor.
// See main program.
// feeding in argc, argv to file to start reading file
Assembler::Assembler(int argc, char *argv[]): m_facc(argc, argv){}     //file access class object defined

/*
NAME
 
    PassI - Establishles the locations of the symbols

SYNOPSIS
 
    void PassI();

DESCRIPTION
 
    This function processes each line of instruction
    and calls a function to identify and add the symbols
    to the symbol table as well as updating the location.
 
*/

void Assembler::PassI()
{
    Errors();        // need this to detect MULTIPLY DEFINED LABELS which are not detected by Pass II
    int loc = 0;     // Tracks the location of the instructions to be generated

    // Successively process each line of source code
    for( ; ; )
    {
        // Read the next line from the source file
        string line;
        if(!m_facc.GetNextLine(line))
        {
            //eliminate errors because same errors will be recorded again in Pass II
            Errors::InitErrorReporting();
            
            // If there are no more lines, we are missing an end statement.
            // We will let this error be reported by Pass II.
            return;
        }
        
        // Parse the line and get the instruction type
        Instruction::InstructionType st =  m_inst.ParseInstruction(line);

        // If this is an end statement, there is nothing left to do in pass I
        // Pass II will determine if the end is the last statement
        if (st == Instruction::ST_End)
        {
            //need to eliminate errors because same errors will be recorded again in Pass II
            Errors::InitErrorReporting();
            
            return;
        }
            
        // Labels can only be on machine language and assembler language
        // instructions.  So, skip other instruction types.
        if(st != Instruction::ST_MachineLanguage && st != Instruction::ST_AssemblerInstr)
            continue;
        
        // If the instruction has a label, record it and its location in the symbol table.
        if(m_inst.isLabel())
            m_symtab.AddSymbol(m_inst.GetLabel(), loc);
        
        //update the location for next instruction
        loc = m_inst.LocationNextInstruction(loc);
    }
}
/*void Assembler::PassI(); */


/*
NAME
 
    PassII - Generates a translation for each line

SYNOPSIS
 
    void PassII();

DESCRIPTION
 
    This function processes each line of instruction
    and calls a function to parse and identify the
    different components of the instruction. The identified
    components are used to generate a translation for each line.
 
*/

void Assembler::PassII()
{
    Errors();         // need this to record errors using the error list.
    m_facc.Rewind();  // go to the beginning of the file.
    
    int loc = 0;      // Tracks the location of the instructions to be generated.
    
    cout<<"TRANSLATION OF PROGRAM: "<<endl<<endl;
    cout<<"LOCATION "<<"  CONTENTS"<<"     ORIGINAL STATEMENT"<<endl;

    // used to check if there is an END statement
    bool endInstr = false;
    
    // used to check if there is a HALT instruction
    bool haltInstr = false;
    
    // used to check if there is any instructions before location 100
    bool instrBeforeHundred = false;
    
    // Successively process each line of source code.
    for( ; ; )
    {
        // Read the next line from the source file.
        string line;
        if(!m_facc.GetNextLine(line))
        {
            // if there is no END instruction
            if (!endInstr)
            {
                //Code 17: No END Instruction Was Detected
                Errors::RecordError(17, "*****");
            }
            
            // if there is no HALT instruction
            if (!haltInstr)
            {
                //Code 14: No HALT Instruction Detected for Execution Termination
                Errors::RecordError(14, "*****");
            }
            
            // if there is statements before location 100
            if (instrBeforeHundred)
                cout<<endl<<endl<<endl<<"<WARNING: Instructions Before Location 100 Will Not Be Executed>"<<endl;
            
            //Now going to report errors or run the emulator
            cout<<endl<<"Press [Enter] to continue . . ."<<endl;
            cin.get();
            
            return;
        }
       
        // Parse the line and get the instruction type
        Instruction::InstructionType st =  m_inst.ParseInstruction(line);
        
        string content; // holds the numeric translation
            
        //as long as we have not reached the END instruction
        if (!endInstr)
        {
            if (st == Instruction::ST_MachineLanguage)
            {
                if (haltInstr)
                {
                    //Code 13: Machine Language Statements Are NOT Allowed After the HALT Instruction
                    Errors::RecordError(13, line);
                }
                
                content = m_inst.GetOpcode() + m_inst.GetRegister();
                
                // if we have HALT's opcode, 13
                if (m_inst.GetOpcode() == "13")
                {
                    if (haltInstr)
                    {
                        //Code 10: HALT Instruction Can Only Be Included Once
                        Errors::RecordError(10, line);
                    }
                    
                    if (loc < 100)
                    {
                        //Code 11: HALT Instruction Before Location 100 Will Not Be Detected By Emulator
                        Errors::RecordError(11, line);
                    }
                    
                    haltInstr = true;
                    
                    //address part of HALT instruction translation
                    content += "00000";
                }
            
                else
                {
                    //this part appends operand's address to the translation
                  
                    int locForTranslation = 0;
                    
                    //if symbol is not undefined and not multiply defined
                    if (!(HasSymbolError(line, content, locForTranslation)))
                    {
                        //locForTranslation holds the addres portion of the CONTENT of translation
                        //need to replace the actual operand with its location in symbol table
                        
                        string address = to_string(locForTranslation);
                                
                        //append zeros for the remaining digits of the address
                        //useful when address is less than 5 digits
                        for (size_t i = 0; i < 5 - address.size(); i++)
                            content += '0';
                        
                        //now content holds the full translation
                        content += address;
                    }
                }
            }
                        
            //otherwise there is a COMMENT, END, or AssemblerInstruction (ORG, DS, DC)
            else
            {
                //i.e. if we have DS, DC or ORG
                if (st != Instruction::ST_Comment && st != Instruction::ST_End)
                {
                    //we have no way of knowing if assembler instruction is DS, DC, or ORG
                    //so we do a quick parsing
                    string assemLanType = QuickParse(line, content);
                    
                    //if HALT instruction is not visited and we have a statement with a DS or DC
                    if ((!haltInstr) && (assemLanType != "ORG"))
                    {
                        //Code 12: Assembler Language Statements Are Not Allowed Before HALT Instruction
                        Errors::RecordError(12, line);
                    }
                }
                
                // otherwise we have a COMMENT or END
                else
                {
                    //to make sure an END instruction is included
                    if (st == Instruction::ST_End)
                        endInstr = true;
                    
                    //nothing to do if it is a COMMENT
                }
                
            }
        }
        
        //if END instruction is visited but there is more lines to read
        else
        {
            // if END is instruction is included again
            if (st == Instruction::ST_End)
            {
                //Code 16: END Instruction Can Only Be Included Once
                Errors::RecordError(16, line);
            }
            
            else
            {
                //if the lines are machine or assembler language statements
                if (st != Instruction::ST_Comment)
                {
                    //Code 18: Only Comments Are Allowed After END Instruction
                    Errors::RecordError(18, line);
                }
            }
        }
                
        //Output translation
        DisplayTranslation(loc, content, line, st);
        
        //update location for next instruction
        loc = m_inst.LocationNextInstruction(loc);
        
        //to give warning for instructions before 100th location
        if (loc < 100 && loc != 0)
            instrBeforeHundred = true;
    }
}
/*void Assembler::PassII(); */


/*
NAME
 
    HasSymbolError - Checks if symbol is defined only once

SYNOPSIS
 
    bool HasSymbolError(const string& a_line, string& a_content, int& a_locForTranslation);

DESCRIPTION
 
   This function looks up the symbol in the symbol table
   and uses "a_locForTranslation" to make sure it is only defined once.
   If the symbol is undefined or multiply defined, appropriate error messages
   will be recorded with original statement "a_line" and "a_content"
   will be replaced with question marks.
 
   Returns true - if symbol is undefined or multiply defined
   Returns false - if symbol is defined once
*/

bool Assembler::HasSymbolError(const string& a_line, string& a_content, int& a_locForTranslation)
{
    //if the symbol is undefined
    if (!(m_symtab.LookupSymbol(m_inst.GetOperand(), a_locForTranslation)))
    {
        //Code 7: Undefined Symbol
        Errors::RecordError(7, a_line);
        
        return true;
    }
    
    //a_locForTranslation holds the CORRECT location of symbol if it is defined ONLY once
    int multiplyDefinedSymbol = -999;
    
    if (a_locForTranslation == multiplyDefinedSymbol)
    {
        // show question marks for translation of multiply defined symbols
        for (int i = 0; i < 5; i++)
            a_content += '?';
                          
        //Code 6: Multiply Defined Symbol
        Errors::RecordError(6, a_line);
        
        return true;
    }
                  
    return false;
}
/*bool Assembler::HasSymbolError(const string& a_line,
  string& a_content, int& a_locForTranslation); */


/*
NAME
 
    DefinedConstantTranslation - Translates DC statement

SYNOPSIS
 
    void DefinedConstantTranslation(string& a_content) const;

DESCRIPTION
 
   This function generates a translation for a defined constant
   based on its sign (negative or positive) and the number of
   its digits. The result is placed in "a_content".
 
*/

void Assembler::DefinedConstantTranslation(string& a_content) const
{
    //need to determine if number is positive or negative
    //initally assume the number is positive
    //bc max value for a constant = 99,999,999 (8 characters)
    int maxChar = 8;
        
    //if number is negative
    if (m_inst.GetOperandValue() < 0)
    {
        //bc min value for a constant = -99,999,999 (9 characters)
        maxChar = 9;
        
        //stick the sign in front of translation
        a_content += '-';
    }
        
    string operand = m_inst.GetOperand();
     
    //add zeros for remaining digits of constant
    //useful when constant has less than 8 digits
    for (size_t i = 0; i < (maxChar - operand.size()); i++)
            a_content += '0';
        
    //now append the digits of the positive constant
    if (m_inst.GetOperandValue() >= 0)
        a_content += operand;
        
    //if constant is negative
    else
    {
        //skip the negative sign because it is already added
        //now append the digits of negative constant
        for (size_t i = 1; i < operand.size(); i++)
            a_content += operand[i];
    }
}
/*void Assembler::DefinedConstantTranslation(string& a_content) const; */


/*
NAME
 
    QuickParse - Identifies the specific type of assembler language

SYNOPSIS
 
    string QuickParse(const string& a_line, string& a_content) const;

DESCRIPTION
 
   This function uses "a_line" to parse an assembler langauage
   instruction to identify whether a DC, DS, or ORG is specified.
   It uses "a_content" to check the operand for DC.
 
   Returns - DC, DS, or ORG
*/

string Assembler::QuickParse(const string& a_line, string& a_content) const
{
    // holds the capital form of a_line
    string capitalForm;
    
    // need the capital form of assembler language words to detect these instructions
    for (size_t i = 0; i < a_line.size(); i++)
        capitalForm += toupper(a_line[i]);
    
    // will be either DC, DS, or ORG
    string assemLanType;
    
    //extract the second word in the line since formatting = LABEL ASSEMLAN OPERAND
    stringstream instruction(capitalForm);
    instruction>>assemLanType;
    instruction>>assemLanType;
    
    if (assemLanType == "DC")
    {
        // The constant's value will become the content
        DefinedConstantTranslation(a_content);
    }
    
    else if (assemLanType == "DS")
    {
        // content will be empty since DS has no translation
        return assemLanType;
    }
    
    // because ORG may or may not have a label
    // so if second word is not DC or DS then it must be ORG
    else
    {
        // content will be empty since ORG has no translation
        assemLanType = "ORG";
    }
    
    return assemLanType;
}
/*string Assembler::QuickParse(const string& a_line, string& a_content) const; */


/*
NAME
 
    DisplayTranslation - Outputs translation

SYNOPSIS
 
    void DisplayTranslation(const int& a_loc, const string& a_content,
        const string& a_line, const Instruction::InstructionType& a_st)

DESCRIPTION
 
    This function outputs the "a_loc", "a_content", and "a_line" as
    the location, contents and the original statement in three columns
    and inserts the translation into memory. This is the translation
    generated by PassII().
*/

void Assembler::DisplayTranslation(const int& a_loc, const string& a_content,
                                   const string& a_line, const Instruction::InstructionType& a_st) 
{
    //setting up the formatting of the columns
    cout<<setw(11)<<left;
    
    //END and COMMENTS have no content or location for translation
    if (a_st == Instruction::ST_End || a_st == Instruction::ST_Comment)
    {
        if (a_line != "")
            cout<<setw(25)<<"      "<<setw(10)<<a_line<<endl;
    }
    
    //DS and ORG have only location for translation
    else if (a_content == "")
        cout<<a_loc<<setw(14)<<"      "<<setw(10)<<a_line<<endl;
    
    //LOCATION -> CONTENT -> ORIGINAL STATEMENT
    else
    {
        cout<<a_loc<<setw(8)<<a_content<<"      "<<setw(10)<<a_line<<endl;
        int content_as_num = stoi(a_content);
        m_emul.InsertMemory(a_loc, content_as_num);
    }
    
    return;
}
/*void Assembler::DisplayTranslation(const int& a_loc, const string& a_content,
  const string& a_line, const Instruction::InstructionType& a_st); */


/*
NAME
 
    RunProgramInEmulator - Calls the emulator to run program

SYNOPSIS
 
    void RunProgramInEmulator();

DESCRIPTION
 
    (1) If there is no error, then the function calls
    the emulator to run the program
 
    (2) If there is an error, then the function calls
    a function to display all errors
 
*/

void Assembler::RunProgramInEmulator()
{
    cout<<endl<<endl;
    
    //we run emulator only if there is no error
    if (Errors::NumErrors() == 0)
    {
        m_emul.RunProgram();
        
        //if there are run-time errors
        if (Errors::NumErrors() != 0)
        {
            cout<<endl<<endl<<"RUN-TIME ";
            Errors::DisplayErrors();
        }
    }
        
    //if at least one error has been recorded throughout the translation process 
    else
    {
        cout<<"NUMBER OF ERRORS: "<<Errors::NumErrors()/2<<endl;
        Errors::DisplayErrors();
    }
}
/*void Assembler::RunProgramInEmulator(); */

