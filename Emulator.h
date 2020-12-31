//
//        Emulator class - supports the emulation of Quack3200 programs
//

#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "stdafx.h"

class Emulator
{

public:
    
    //the supported opcodes
    enum OpcodeType
    {
        ADD = 1,    // Addition        1
        SUB,        // Subtraction     2
        MULT,       // Multiplication  3
        DIV,        // Division        4
        LOAD,       // LOAD            5
        STORE,      // STORE           6
        READ,       // READ            7
        WRITE,      // WRITE           8
        B,          // BRANCH          9
        BM,         // BRANCH MINUS    10
        BZ,         // BRANCH ZERO     11
        BP,         // BRANCH POSITIVE 12
        HALT        // HALT            13
    };
    
    // The size of the memory of the Quack3200
    const static int MEMSZ = 100000;
    
    Emulator()
    {
        memset(m_memory, 0, MEMSZ * sizeof(int));
        
        for (int i = 0; i < 10; i++)
            m_reg[i] = 0;
    }
    
    // Records instructions and data into Quack3200 memory
    bool InsertMemory(const int& a_location, const int& a_contents)
    {
        if (a_location >= 0 && a_location < 100'000)
            m_memory[a_location] = a_contents;
        
        // location will never exceed 99,999 as it is checked
        // by LocationNextInstruction Function
        
        return true;
    }
    
    // Runs the Quack3200 program recorded in memory
    bool RunProgram();
    
    // Checks run-time inputs
    bool InputChecker(const string&) const;
    
    // Checks the result of operations at run-time
    bool ResultChecker(const int&, const int&, const int&, const OpcodeType&) const;
    
    
private:
    
    int m_memory[MEMSZ];                    // The memory of the Quack3200
    int m_reg[10];                          // The accumulator for the Quack3200
};

#endif



