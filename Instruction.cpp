//
//  Implementation of instruction class.
//

#include "stdafx.h"

/*
NAME
 
    ParseInstruction - Identifies and parses each word in the instruction

SYNOPSIS
 
    InstructionType ParseInstruction(const string& a_buff);

DESCRIPTION
 
   Based on the number of words in the instruction (not inclusing comments),
   this function calls the appropriate function to identify the type of
   instruction and record all the components of the stataement.
   
   Returns - ST_MachineLanguage, ST_AssemblerInstr, ST_Comment or ST_End
*/

Instruction::InstructionType Instruction::ParseInstruction(const string& a_buff)
{
    //setting default register in case it is not specified
    m_Register = "9";
    
    //resetting label for next line of instruction
    m_Label = "";
    
    //recording original instruction before any modification
    m_instruction = a_buff;
    
    //holds a copy of the instruction
    string instCopy = a_buff;
       
    //look for ';' and find the index of its first appearance (if it exists)
    size_t index = instCopy.find(';');
        
    //if there is a comment
    if (index != string::npos)
    {
        //copy over everything before ";"
        instCopy.resize(index);
    }
        
    //if there is a comma error
    if (!CommaChecker(instCopy))
    {
         //Code 24: Comma Can Only Be Used To Separate Register From Operand
         Errors::RecordError(24, m_instruction);
    }
        
    //first word is considered a speical case
    string firstWord;
           
    stringstream line(instCopy);
    line>>firstWord;
       
    //if it's an empty line bc text after ";" are removed or simply an empty line
    if (firstWord == "")
    {
        m_type = ST_Comment;
        return ST_Comment;
    }
       
    //look for comma and find the index of its first appearance (if it exists)
    index = instCopy.find(',');
    
    //if there is a comma
    if (index != string::npos)
    {
        //replace with whitespace - this removes all commas from line
        replace(instCopy.begin(), instCopy.end(), ',', ' ');
    }
    
    //compute the number of words in the instruction
    int instrWords = WordsToReadFinder(instCopy);
    
    //because we need to read the first word again
    stringstream line2(instCopy);
       
    //instructions at most can have 4 words 
    const int MAXSIZE = 4;
    
    //will hold the original form of the words in the instruction
    string originalForm[MAXSIZE];
    
    for (int i = 0; i < MAXSIZE; i++)
        line2>>originalForm[i];
       
    //will hold the capitalized form of the words in the instruction
    string capitalForm[MAXSIZE];
       
    for (int i = 0; i < MAXSIZE; i++)
        for (size_t j = 0; j < originalForm[i].size(); j++)
            capitalForm[i] += toupper(originalForm[i][j]);
    
    //if there are 4 words in the instruction
    if (instrWords == 4)
        return FourInstrProcessor(capitalForm, originalForm);
    
    //if there are 3 words in the instruction
    if (instrWords == 3)
        return ThreeInstrProcessor(capitalForm, originalForm);
    
    //if there are 2 words in the instruction
    if (instrWords == 2)
        return TwoInstrProcessor(capitalForm, originalForm[1]);
    
    //if there is one word in the instruction
    if (instrWords == 1)
        return SingleInstrProcessor(capitalForm[0]);
    
    //if there are more than 4 words in the instruction
    //Code 3: Extra Operands
    Errors::RecordError(3, m_instruction);
    
    return ST_Comment;
}
/*Instruction::InstructionType Instruction::ParseInstruction(const string& a_buff); */


/*
NAME
 
    FourInstrProcessor - Identifies and records different elements of a instruction with 4 words

SYNOPSIS
 
    InstructionType FourInstrProcessor(const string a_capital[], const string a_original[]);

DESCRIPTION
 
   This function deals with a line that is consisted of 4 words.
   It helps identify the different components of the instruction
   (label, opcode, register, operand) by starting a chain of
   appropriate function calls using the components of the line in
   "a_capital[]" and "a_original[]". The only supported 4-word instruction is:
   
   LABEL OPCODE REGISTER OPERAND (Machine Language)
 
   Returns ST_Comment - If there is an error
   Returns ST_MachineLanguage - Otherwise
*/

Instruction::InstructionType Instruction::FourInstrProcessor(const string a_capital[], const string a_original[])
{
    //Check if symbol meets Quack3200 specifications
    SymbolValidation(a_original[0]);
    
    m_Label = a_original[0];
    
    //if the second word in the line is a valid opcode
    if (m_OpcodeList.find(a_capital[1]) != m_OpcodeList.end())
    {
        //now that label is recorded, the rest of the instruction can be treated
        //as a 3-word machine language statement
        
        //Record opcode, register, and operand of the 3-word machine language statement
        ThreeWordMachineLan(a_capital[1], a_capital[2], a_original[3]);
        
        m_type = ST_MachineLanguage;
        return ST_MachineLanguage;
    }
    
    else
    {
        //Code 22: Invalid Assembly Language Statement
        Errors::RecordError(22, m_instruction);
        
        //The statement is meaningless and is reported as a comment
        return ST_Comment;
    }
}
/*Instruction::InstructionType Instruction::FourInstrProcessor
  (const string a_capital[], const string a_original[]); */


/*
NAME
 
    ThreeInstrProcessor - Identifies and records different elements of a instruction with 3 words

SYNOPSIS
 
    InstructionType ThreeInstrProcessor(const string a_capital[], const string a_original[]);

DESCRIPTION
 
   This function deals with a line that is consisted of 3 words.
   It helps identify the different components of the instruction
   by starting a chain of appropriate function calls using the
   components of the line in "a_capital[]" and "a_original[]".
   The only supported 3-word instructions are:
 
   Case 1: OPCODE REGISTER OPERAND   (Machine Language)
   Case 2: LABEL  OPCODE   OPERAND   (Machine Language)
   Case 3: LABEL  ASSEMLAN OPERAND   (Assembler Language)
   
   Returns ST_Comment - If there is an error
   Returns ST_MachineLanguage or ST_AssemblerInstr - Otherwise
*/

Instruction::InstructionType Instruction::ThreeInstrProcessor(const string a_capital[], const string a_original[])
{
    //if the first word is a valid opcode (Case #1)
    if (m_OpcodeList.find(a_capital[0]) != m_OpcodeList.end())
    {
        //Record opcode, register, and operand of the 3-word machine language statement
        ThreeWordMachineLan(a_capital[0], a_capital[1], a_original[2]);
        
        m_type = ST_MachineLanguage;
        return ST_MachineLanguage;
    }
    
    //if the second word is a valid opcode (Case #2) - READ, WRITE, B
    if (TwoWordMachineLan(a_capital[1], a_original[2]))
    {
        //opcode and operand for this instruction are recorded by TwoWordMachineLan function call
        
        //Check if symbol meets Quack3200 specifications
        SymbolValidation(a_original[0]);
        
        m_Label = a_original[0];
        m_type = ST_MachineLanguage;
        return ST_MachineLanguage;
    }
    
    //if the second word is an assembler language word (Case #3)
    if (a_capital[1] == "DS" || a_capital[1] == "DC" || a_capital[1] == "ORG")
    {
        //Check if symbol meets Quack3200 specifications
        SymbolValidation(a_original[0]);
        
        //check and record the operand of the 3-word assembler language statement
        ThreeWordAssembly(a_capital[1], a_capital[2]);
        
        m_Label = a_original[0];
        m_type = ST_AssemblerInstr;
        return ST_AssemblerInstr;
    }
            
    else
    {
        //Code 22: Invalid Assembly Language Statement
        Errors::RecordError(22, m_instruction);
        
        //The statement is meaningless and is reported as a comment
        m_type = ST_Comment;
        return ST_Comment;
    }
}
/*Instruction::InstructionType Instruction::ThreeInstrProcessor
  (const string a_capital[], const string a_original[]); */


/*
NAME
 
    TwoInstrProcessor - Identifies and records different elements of a instruction with 2 words

SYNOPSIS
 
    InstructionType TwoInstrProcessor(const string a_capital[], const string& a_operand);

DESCRIPTION
 
   This function deals with a line that is consisted of 2 words.
   It helps identify the different components of the instruction
   by starting a chain of appropriate function calls using the
   components of the line in "a_capital[]" and "a_operand".
   The supported 2-word instructions are:
 
   Case 1: OPCODE OPERAND  (Machine Language)
   Case 2: HALT   REGISTER (Machine Language)
   Case 3: ORG    OPERAND  (Assembler Language)
   
   Returns ST_Comment or ST_End - If there is an error
   Returns ST_MachineLanguage or ST_AssemblerInstr - Otherwise
*/

Instruction::InstructionType Instruction::TwoInstrProcessor(const string a_capital[], const string& a_operand)
{
    //if first word is the opcode READ, WRITE, B or HALT (Case #1 and #2)
    if (TwoWordMachineLan(a_capital[0], a_operand))
    {
        //TwoWordMachineLan records opcode and operand/register of the 2-word machine language statement
        
        m_type = ST_MachineLanguage;
        return ST_MachineLanguage;
    }
    
    //if the first word is an assembler language word (Case #3)
    if (a_capital[0] == "ORG")
    {
        if (IsInteger(a_operand))
        {
            //Check and record the operand for the ORG instruction
            OriginInstr(a_operand);
            
            m_type = ST_AssemblerInstr;
            return ST_AssemblerInstr;
        }
        
        else
        {
            //Code 2: Operand Must Be Numeric
            Errors::RecordError(2, m_instruction);
        }
    }
    
     //Just to give a hint if a label is included with END instruction
    if (a_capital[1] == "END")
    {
        //Code 15: END Instruction cannot have a label
        Errors::RecordError(15, m_instruction);
        
        //still will accept END instruction but will not run emulator
        m_type = ST_End;
        
        return ST_End;
    }
    
    else
    {
        //Code 22: Invalid Assembly Language Statement
        Errors::RecordError(22, m_instruction);
        
        //The statement is meaningless and is reported as a comment
        m_type = ST_Comment;
        return ST_Comment;
    }
}
/*Instruction::InstructionType Instruction::TwoInstrProcessor
  (const string a_capital[], const string& a_operand); */


/*
NAME
 
    SingleInstrProcessor - Identifies and records the element of a instruction with 1 word

SYNOPSIS
 
    InstructionType SingleInstrProcessor(const string& a_single);

DESCRIPTION
 
   This function deals with a line that is consisted of 1 word.
   It determines if "a_single" is a HALT or END instruction.
   The only supported 1-word instructions are:
 
   Case 1: HALT  (Machine Language)
   Case 2: END   (END instruction)
   
   Returns ST_Comment - If there is an error
   Returns ST_MachineLanguage or ST_End - Otherwise
*/

Instruction::InstructionType Instruction::SingleInstrProcessor(const string& a_single)
{
    if (a_single == "HALT")
    {
        //record elements of the instruction
        m_OpCode = a_single;
        m_NumOpCode = stoi(m_OpcodeList[m_OpCode]);
        m_type = ST_MachineLanguage;
        return ST_MachineLanguage;
    }
    
    if (a_single == "END")
    {
        m_type = ST_End;
        return ST_End;
    }
    
    else
    {
        //Code 22: Invalid Assembly Language Statement
        Errors::RecordError(22, m_instruction);
        
        //The statement is meaningless and is reported as a comment
        m_type = ST_Comment;
        return ST_Comment;
    }
}
/*Instruction::InstructionType Instruction::SingleInstrProcessor(const string& a_single); */


/*
NAME
 
    ThreeWordMachineLan - Records opcode, register, and operand of a 3-word machine language statement

SYNOPSIS
 
    void ThreeWordMachineLan(const string& a_opcode, const string& a_reg, const string& a_operand);

DESCRIPTION
 
   This function checks and records the "a_opcode", "a_reg" and "a_operand"
   as the opcode, register, and operand included in a 3-word machine language statement.
   The only supported case is:
 
   OPCODE REGISTER OPERAND
 
*/

void Instruction::ThreeWordMachineLan(const string& a_opcode, const string& a_reg, const string& a_operand)
{
    m_OpCode = a_opcode;
    m_NumOpCode = stoi(m_OpcodeList[m_OpCode]);
    
    //register must be numeric and one digit long
    //because min register = 0 and max register = 9
    if (IsInteger(a_reg) && (a_reg.size() == 1))
    {
        //the above conditions ensure register value is positive and in the range 0-9
        //because negative numbers take 2 characters since they are in string form
        m_Register = a_reg;
        m_NumRegister = stoi(m_Register);
    }
    
    else
    {
        //Code 21: Invalid Register Specified
        Errors::RecordError(21, m_instruction);
    }
    
    //only symbolic operands are allowed with opcodes
    if (!IsInteger(a_operand))
        m_Operand = a_operand;
        
    else
    {
        //Code 0: Operand Must Be Symbolic
        Errors::RecordError(0, m_instruction);
    }
}
/*void Instruction::ThreeWordMachineLan(const string& a_opcode,
  const string& a_reg, const string& a_operand); */


/*
NAME
 
    TwoWordMachineLan - Records opcode and operand of a 2-word machine language statement

SYNOPSIS
 
    bool TwoWordMachineLan(const string& a_opcode, const string& a_operand);

DESCRIPTION
 
   This function checks and records the operand for opcodes that can be included
   without register number specification. The following cases are supported:
    
   (1) READ  OPERAND
   (2) WRITE OPERAND
   (3) B     OPERAND
   (4) HALT  REGISTER
 
   Returns true - If READ, WRITE, B or HALT is the opcode
   Returns false - Otherwise
*/

bool Instruction::TwoWordMachineLan(const string& a_opcode, const string& a_operand)
{
    //operand for these must be symbolic (Case 1-3)
    if (a_opcode == "READ" || a_opcode == "WRITE" || a_opcode == "B" )
    {
        m_OpCode = a_opcode;
        m_NumOpCode = stoi(m_OpcodeList[m_OpCode]);
            
        if (!IsInteger(a_operand))
            m_Operand = a_operand;
                
        else
        {
            //Code 0: Operand Must Be Symbolic
            Errors::RecordError(0, m_instruction);
        }
            
        return true;
    }
    
    //operand for HALT is a register value 0-9 (Case 4)
    if (a_opcode == "HALT")
    {
        m_OpCode = a_opcode;
        m_NumOpCode = stoi(m_OpcodeList[m_OpCode]);
        
        //register must be numeric and one digit long because
        //min register = 0 and max register = 9
        if (IsInteger(a_operand) && (a_operand.size() == 1))
        {
            //above conditions will also ensure register value is positive and in the range 0-9
            //because negative numbers take 2 characters since they are in string form
            m_Register = a_operand;
            m_NumRegister = stoi(m_Register);
        }
        
        else
        {
            //Code 21: Invalid Register Specified
            Errors::RecordError(21, m_instruction);
        }
        
        return true;
    }
    
    //otherwise none of the cases apply
    return false;
}
/*bool Instruction::TwoWordMachineLan(const string& a_opcode, const string& a_operand); */


/*
NAME
 
    ThreeWordAssembly - Checks and records the operand of a 3-word assembler language statement

SYNOPSIS
 
    void ThreeWordAssembly(const string& a_assemLan, const string& a_operand);

DESCRIPTION
 
   This function uses "a_assemLan" to identify, check and record the
   "a_operand" as the operand that is included in a 3-word assembler language statement.
   The symbols are already recorded. The following are supported:
 
   (1) SYMBOL DC  OPERAND
   (2) SYMBOL DS  OPERAND
   (3) LABEL  ORG OPERAND
*/

void Instruction::ThreeWordAssembly(const string& a_assemLan, const string& a_operand)
{
    //used to redefine DS operand in case of an error
    bool valid_DS_Operand = false;
    
    if (IsInteger(a_operand))
    {
        if (a_assemLan == "ORG")
        {
            //Check and record the operand for the ORG instruction
            OriginInstr(a_operand);
        }
        
        else if (a_assemLan == "DS")
        {
            m_IsNumericOperand = true;
            
            //since only positive operands are allowed
            //and max = 99,999 (5 digits)
            if (a_operand.size() < 6)
            {
                //operand for DS must be positive
                if (stoi(a_operand) > 0)
                {
                    valid_DS_Operand = true;
                    m_Operand = a_operand;
                    m_OperandValue = stoi(m_Operand);
                }
                
                else
                {
                    //Code 1: Operand Must Be Positive Integer
                    Errors::RecordError(1, m_instruction);
                }
                
            }
            
            else
            {
                //Code 4: Operand Too Large For Quack3200
                Errors::RecordError(4, m_instruction);
            }
        }
        
        else if (a_assemLan == "DC")
        {
            //Check and record the operand for the DC instruction
            DC_Instr(a_operand);
        }
    }
        
    else
    {
        //Code 2: Operand Must Be Numeric
        Errors::RecordError(2, m_instruction);
    }
    
    //do this to specify the location of next instruction in
    //case the operand for DS is invalid
    if ((a_assemLan == "DS") && (!valid_DS_Operand))
    {
        m_IsNumericOperand = true;
        m_Operand = "1";
        m_OperandValue = 1;
    }
}
/*void Instruction::ThreeWordAssembly(const string& a_assemLan, const string& a_operand); */


/*
NAME
 
    OriginInstr - Checks and records the operand for an ORG instruction

SYNOPSIS
 
    void OriginInstr(const string& a_operand);

DESCRIPTION
 
   This function checks if:
   (1) The "a_operand" is a positive integer
   (2) The "a_operand" is in the range of the Quack3200 memory
 
   If conditions are met, operand value is recorded
   Otherwise default values are recorded
*/

void Instruction::OriginInstr(const string& a_operand)
{
    //setting the default for origin in case it is specified incorrectly
    m_Operand = "100";
    
    m_OperandValue = stoi(m_Operand);
    m_IsNumericOperand = true;
    
    //bc last location to translate is loc = 99,999 (5 digits)
    if (a_operand.size() < 6)
    {
        int possibleOperand = stoi(a_operand);
        
        //operand for ORG must be positive
        if (possibleOperand > 0)
        {
            m_Operand = a_operand;
            m_OperandValue = stoi(m_Operand);
        }
               
        else
        {
            //Code 1: Operand Must Be Positive Integer
            Errors::RecordError(1, m_instruction);
        }
    }
        
    else
    {
        //Code 5: Operand Exceeds Quack3200 Final Location
        Errors::RecordError(5, m_instruction);
    }
}
/*void Instruction::OriginInstr(const string& a_operand); */


/*
NAME
 
    DC_Instr - Checks and records the operand for a DC instruction

SYNOPSIS
 
    void DC_Instr(const string& a_operand);

DESCRIPTION
 
   This function checks if "a_operand" fits into a Quack3200 memory location.
   If condition is met, the operand value is recorded.
   Otherwise zeros are recorded as operand value.
*/

void Instruction::DC_Instr(const string& a_operand)
{
    // max value = 99,999,999 (8 characters)
    size_t maxChar = 8;
    
    //if the operand is negative
    if (a_operand[0] == '-')
    {
        //min value = -99,999,999 (9 characters)
        maxChar = 9;
    }
    
    if (a_operand.size() <= maxChar)
        m_Operand = a_operand;
        
    else
    {
        //Record 0s as operand value if it is too big
        m_Operand = "00000000";
        
        //Code 19: Constant Too Large For Quack3200
        Errors::RecordError(19, m_instruction);
    }
    
    //record elements of instruction
    m_OperandValue = stoi(m_Operand);
    m_IsNumericOperand = true;
}
/*void Instruction::DC_Instr(const string& a_operand); */


/*
NAME
 
    LocationNextInstruction - Computes the location of the next instruction

SYNOPSIS
 
    int LocationNextInstruction(const int& a_loc) const;

DESCRIPTION
 
   This function, based on the type of instruction, computes
   the location of the next instruction from the current location "a_loc".
   There are four possibilities:
 
   (1) DS Instruction  -> nextLocation = currentLocation + operand of DS
   (2) ORG Instruction -> nextLocation = operand of ORG
   (3) COMMENT or END  -> nextLocation = currentLocation
   (4) Otherwise       -> nextLocation = currentLocation + 1
 
   Returns - The location of the next instruction
*/

int Instruction::LocationNextInstruction(const int& a_loc) const
{
    if (m_type == ST_AssemblerInstr)
    {
        //determine if it's DS or ORG
        string assemLanType;
        stringstream line(m_instruction);
        
        //the first word in the instruction
        line>>assemLanType;
        
        //change to capital form of this word
        for (size_t i = 0; i < assemLanType.size(); i++)
            assemLanType[i] = toupper(assemLanType[i]);
        
        //saving first instruction in case we have a 2-word ORG instruction (Ex: ORG 100)
        string possibleOrg = assemLanType;
        
        //DS and DC are 3-word instructions and ORG could have a label (Ex: Duck ORG 100)
        //the second word in the instruction
        line>>assemLanType;
        
        //change to capital form of this word
        for (size_t i = 0; i < assemLanType.size(); i++)
            assemLanType[i] = toupper(assemLanType[i]);
    
        if (assemLanType == "DS")
        {
            //because last location of Quack3200 Memory = 99,999
            if (a_loc + m_OperandValue < 100'000)
                return a_loc + m_OperandValue;
            
            else
            {
                //Code 5: Operand Exceeds Quack3200 Final Location
                Errors::RecordError(5, m_instruction);
            }
        }

        //if we have a 2-word or 3-word ORG instruction
        else if (assemLanType == "ORG" || possibleOrg == "ORG")
        {
            if (m_OperandValue > a_loc)
                return m_OperandValue;
            
            else
            {
                //Code 23: Origin Operand Must be Higher Than Current Location
                Errors::RecordError(23, m_instruction);
            }
        }
        //other necessary checks on the operand of ORG are already performed in OriginInstr function
    }
    
    //COMMENT and END have no effect on location
    else if (m_type == ST_Comment || m_type == ST_End)
        return a_loc;
    
    //if final memeory location of Qucack3200 is exceeded (last location = 99,999)
    if (a_loc + 1 > 99'999)
    {
        //Code 20: Insufficient Memory for Translation
        Errors::RecordError(20, m_instruction);
        
        return a_loc;
    }
    
    //this also includes DC statements
    return a_loc + 1;
}
/*int Instruction::LocationNextInstruction(const int& a_loc) const; */


/*
NAME
 
    WordsToReadFinder - Finds the number of words in the instruction

SYNOPSIS
 
    int WordsToReadFinder(const string& a_buff) const;

DESCRIPTION
 
   This function finds the number words in the line "a_buff".
   Each word is a component of the instruction.
 
   Returns - number of words in the line
*/

int Instruction::WordsToReadFinder(const string& a_buff) const
{
    //holds the number of words in the instruction
    int wordsToRead = 0;
    
    for (size_t i = 0; i < a_buff.size() - 1; i++)
    {
         //if a letter if followed by a space, it is a word
         if (a_buff[i] != ' ' && a_buff[i+1] == ' ')
             wordsToRead++;
    }
       
    //there is however a special case because the last word might not
    //be followed by a space. So as long as last character is not a space,
    //we have another word
    if (a_buff[a_buff.size()-1] != ' ')
        wordsToRead++;
    
    //note that there is at least one word to read otherwise
    //we would not reach this part of the program
    return wordsToRead;
}
/*int Instruction::WordsToReadFinder(const string& a_buff) const; */


/*
NAME
 
    SymbolValidation - Checks if symbol meets Quack3200 specifications

SYNOPSIS
 
    bool SymbolValidation(const string& a_symbol) const;

DESCRIPTION
 
   This function checks if:
   (1) "a_symbol" is 1-10 characters long
   (2) The first character of "a_symbol" is a letter and remaining are letters and digits
 
   Returns true - if conditions are met
   Returns false - Otherwise
*/

bool Instruction::SymbolValidation(const string& a_symbol) const
{
    //if first character is not alphabetical or there is more than 10 characters
    if ((!isalpha(a_symbol[0])) || (a_symbol.size() > 10))
    {
        //Code 8: Symbol Does Not Meet Quack3200 Symbol Specification
        Errors::RecordError(8, m_instruction);
        
        return false;
    }
        
    //start at index 1 because index 0 was already checked
    for (size_t i = 1; i < a_symbol.size(); i++)
    {
        //if any character is not a letter, then it must be a number
        if ((!isalpha(a_symbol[i])))
        {
            //if it is not a number either
            if ((!isdigit(a_symbol[i])))
            {
                //Code 8: Symbol Does Not Meet Quack3200 Symbol Specification
                Errors::RecordError(8, m_instruction);
                
                return false;
            }
        }
    }
    
    return true;
}
/*bool Instruction::SymbolValidation(const string& a_symbol) const; */


/*
NAME
 
    CommaChecker - Checks Quack3200 comma rules

SYNOPSIS
 
    bool CommaChecker(const string& a_line) const;

DESCRIPTION
 
   This function checks to see whether comma in "a_line"
   is only used to separate register value from operand.
 
   Returns true - if condition is met or there is no comma
   Returns false - Otherwise
*/

bool Instruction::CommaChecker(const string& a_line) const
{
    // total number of commas in the instruction
    int commaCount = 0;
    
    // determine how many commas there are
    for (size_t i = 0; i < a_line.size(); i++)
        if (a_line[i] == ',')
            commaCount++;
    
    if (commaCount == 0)
        return true;
    
    if (commaCount > 1)
        return false;
    
    // at this point we have only one comma, so perform the special check
    bool firstEmptyChar = false;
    
    // to meet comma condition: An empty char must be followed
    // by a single digit whose next non-empty char is a comma
    for (size_t i = 0; i < a_line.size() - 3; i++)
    {
        if (a_line[i] == ' ')
            if (isdigit(a_line[i+1]))
                for (size_t j = i + 2; j < a_line.size() - 1; j++)
                {
                    if (a_line[j] != ' ')
                    {
                        if (a_line[j] == ',' && (!firstEmptyChar))
                            return true;
                        
                        else
                            firstEmptyChar = true;
                    }
                }
    }
                            
    return false;
}
/*bool Instruction::CommaChecker(const string& a_line) const; */


/*
NAME
 
    IsInteger - Determines if all characters of a string are numeric

SYNOPSIS
 
    bool IsInteger(const string& a_str) const;

DESCRIPTION
 
   This function determines if all characters of "a_str" are numeric
   (except for the first character which could be a negative sign)
   
   Returns true - If we have a positive or negative integer
   Returns false - Otherwise
 
*/

bool Instruction::IsInteger(const string& a_str) const
{
    //if number is positive, all characters must be numeric starting at index 0
    int startIndex = 0;
    
    //if number is negative, all characters must be numeric starting at index 1
    if (a_str[0] == '-')
    {
        //to protect against this: x dc - (i.e. a negative sign by itself)
        if (a_str.size() == 1)
            return false;
        
        //to protect against this: x dc -0
        if (a_str[1] == '0')
        {
            //Code 30: Negative Sign Cannot Be Followed By 0
            Errors::RecordError(30, m_instruction);
        }
        
        startIndex = 1;
    }
    
    //check all characters
    for (int i = startIndex; i < (int)a_str.size(); i++)
    {
        //if there is a non-numeric character
        if (!(isdigit(a_str[i])))
            return false;
    }
    
    return true;
}
/*bool Instruction::IsInteger(const string& a_str) const; */
