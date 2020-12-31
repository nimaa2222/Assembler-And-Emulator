//
//  Implementation of the emulator class
//

#include "stdafx.h"

/*
NAME
 
    RunProgram - Runs the Quack3200 program recorded in memory

SYNOPSIS
 
    bool RunProgram();

DESCRIPTION
 
    This function starts executing instructions starting at location 100
    until the HALT instruction is detected in a memory location or a run-time
    error is detected. The translated machine language statements are executed
    and the translated assembler language statements hold the values of the
    constants in the program.
*/

bool Emulator::RunProgram()
{
    cout<<"RESULTS FROM EMULATING PROGRAM:"<<endl<<endl;
    
    // used to extract the opcode portion of the translation
    int opcodeDivisor = 1'000'000;
    
    // used to extract the register portion of the translation
    int regDivisor = 100'000;
    
    // starting location for execution of Quack3200
    int executionIndex = 100;
    
    // used to terminate emulator
    bool haltInstr = false;
    
    int translation, opcode, regNumber, address;
       
    //executionIndex will never go beyond memory because emulator will not
    //be called unless there is a HALT instruction within the range of memory
    
    //to stop immediately if we have run-time errors to prevent program from breaking
    while ((!haltInstr) && (Errors::NumErrors() == 0))
    {
        //extracting elements of the translation
        translation = m_memory[executionIndex];
        opcode = translation / opcodeDivisor;
        regNumber = (translation % opcodeDivisor) / regDivisor;
        address = (translation % opcodeDivisor) % regDivisor;
            
        //check this first to make sure we do not attempt to execute assembler language instructions
        if (opcode == HALT)
        {
            cout<<endl<<"END OF EMULATION"<<endl<<endl<<endl;
            haltInstr = true;
        }
            
        else if (opcode == ADD)
        {
            //if the result can be stored in register
            if (ResultChecker(regNumber, m_reg[regNumber], m_memory[address], ADD))
                m_reg[regNumber] += m_memory[address];
        }
                    
        else if (opcode == SUB)
        {
            //if the result can be stored in register
            if (ResultChecker(regNumber, m_reg[regNumber], m_memory[address], SUB))
                m_reg[regNumber] -= m_memory[address];
        }
                    
        else if (opcode == MULT)
        {
            //if the result can be stored in register
            if (ResultChecker(regNumber, m_reg[regNumber], m_memory[address], MULT))
                m_reg[regNumber] *= m_memory[address];
        }
                    
        else if (opcode == DIV)
        {
            if (m_memory[address] != 0)
                m_reg[regNumber] /= m_memory[address];
            
            else
            {
                // to specify the register where error is happening in
                string errorMsg = "REG# ";
                errorMsg += to_string(regNumber);
                
                //Code 29: Division By Zero Is Undefined
                Errors::RecordError(29, errorMsg);
            }
        }
        
        else if (opcode == LOAD)
            m_reg[regNumber] = m_memory[address];
                    
        else if (opcode == STORE)
            m_memory[address] = m_reg[regNumber];
                    
        else if (opcode == READ)
        {
            string input;
            cout<<"? ";
            cin>>input;
            cin.ignore();
            
            //if we have a valid input
            if (InputChecker(input))
            {
                int inputValue = stoi(input);
                m_memory[address] = inputValue;
            }
        }
                    
        else if (opcode == WRITE)
            cout<<m_memory[address]<<endl;
              
        //go to address
        else if (opcode == B)
        {
            executionIndex = address;
            continue;
        }
             
        else if (opcode == BM)
        {
            //go to address if content of register < 0
            if(m_reg[regNumber] < 0)
            {
                executionIndex = address;
                continue;
            }
        }
                   
        else if (opcode == BZ)
        {
            //go to address if content of register = 0
            if (m_reg[regNumber] == 0)
            {
                executionIndex = address;
                continue;
            }
        }
                   
        else if (opcode == BP)
        {
            //go to address if content of register > 0
            if (m_reg[regNumber] > 0)
            {
                executionIndex = address;
                continue;
            }
        }
        
        // otherwise we have zeros in memory since nothing
        // has been inserted into memory in those locations
        // so we do nothing
        
        executionIndex++;
        // if we reach this point it means we have not had any TRUE special cases of
        // BM, BZ, BP, B (which do not simply increment executionIndex and
        // cause jumps to other memory locations)
    }
    
    return true;
}
/*bool emulator::runProgram(); */


/*
NAME
 
    InputChecker - Checks the input at run-time

SYNOPSIS
 
    bool InputChecker(const string& a_input) const;

DESCRIPTION
 
    This function determines if all characters of "a_input"
    are numeric (except for the first character which could be a
    negative sign). It also determines whether "a_input" fits
    the memory location of Quack3200.
 
    Returns true - if conditions are met
    Returns false - Otherwise
*/

bool Emulator::InputChecker(const string& a_input) const
{
    //will be 8 if input is positive and 9 if input is negative (because of the sign)
    //maxVal = 99,999,999 (8 digits)
    size_t maxChar = 8;
    
    //if number is positive, all characters must be numeric starting at index 0
    int startIndex = 0;
    
    //if number is negative, all characters must be digits starting at index 1
    if (a_input[0] == '-')
    {
        //minVal = -99,999,999 (9 characters)
        maxChar = 9;
        startIndex = 1;
    }
    
    //Test 1: Is input an integer?
    for (int i = startIndex; i < (int)a_input.size(); i++)
    {
        if (!(isdigit(a_input[i])))
        {
            //Code 28: Only Integers Are Supported by Quack3200
            Errors::RecordError(28, a_input);
            
            return false;
        }
    }
    
    //Test 2: Does it fit Quack3200 memory?
    if (a_input.size() > maxChar)
    {
        //Code 19: Constant Too Large For Quack3200
        Errors::RecordError(19, a_input);
        
        return false;
    }
       
    return true;
}
/*bool emulator::InputChecker(const string& a_input) const; */


/*
NAME
 
    ResultChecker - Checks the result of an operation at run-time

SYNOPSIS
 
    bool ResultChecker(const int& a_regNumber, const int& a_regVal,
                const int& a_memVal, const opcodeName& a_operation) const;

DESCRIPTION
 
    This function performs the "a_operation" (ADD, SUB, or MULT)
    on the "a_regVal", the register value
    and the "a_memVal", the value in memory location
    to determine whether the result can be stored into Quack3200 register.
    "a_regNumber" is used to specify register number when giving errors
    if there is any.
 
    Returns true - if result fits into register
    Returns false - Otherwise
*/

bool Emulator::ResultChecker(const int& a_regNumber, const int& a_regVal,
                             const int& a_memVal, const OpcodeType& a_operation) const
{
    // to specify the register number with the overflow if there is an error
    string errorMsg = "REG# ";
    errorMsg += to_string(a_regNumber);
    
    int maxVal = 99'999'999;
    int minVal = -99'999'999;
    
    if (a_operation == ADD)
    {
        //if addition results in a number bigger than maxVal or smaller than minVal
        if ( (a_regVal + a_memVal > maxVal) || (a_regVal + a_memVal < minVal) )
        {
            //Code 25: ADD Instruction Causes Overflow In a Register
            Errors::RecordError(25, errorMsg);
            
            return false;
        }
    }
    
    else if (a_operation == SUB)
    {
        //if subtraction results in a number bigger than maxVal or smaller than minVal
        if ( (a_regVal - a_memVal > maxVal) || (a_regVal - a_memVal < minVal) )
        {
            //Code 26: SUB Instruction Causes Overflow In a Register
            Errors::RecordError(26, errorMsg);
            
            return false;
        }
    }
    
    else if (a_operation == MULT)
    {
        //if multiplication results in a number bigger than maxVal or smaller than minVal
        if ( (a_regVal * a_memVal > maxVal) || (a_regVal * a_memVal < minVal) )
        {
            //Code 27: MULT Instruction Causes Overflow In a Register
            Errors::RecordError(27, errorMsg);
            
            return false;
        }
    }
    
    // we do not have to worry about the division operation because numbers
    // will stay within the Quack3200 constant range (-99,999,999 - 99,999,999)
    // since fractions are not allowed
    
    return true;
}
/*bool emulator::resultChecker(const int& a_regNumber, const int& a_regVal,
  const int& a_memVal, const opcodeName& a_operation) const; */
