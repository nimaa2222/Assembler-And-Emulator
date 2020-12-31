//
// Class to parse and provide information about instructions. 
// by establishing different components of instructions

#pragma once
#include "stdafx.h"

// The elements of an instruction
class Instruction
{

public:
    
    Instruction(): m_Label(""), m_Register(""), m_OpCode(""), m_Operand(""),
    m_instruction(""), m_NumRegister(-1), m_IsNumericOperand(false)
    {
        // inserting all opcodes supported by Quack3200
        m_OpcodeList.insert(pair<string, string>("ADD", "01"));
        m_OpcodeList.insert(pair<string, string>("SUB", "02"));
        m_OpcodeList.insert(pair<string, string>("MULT", "03"));
        m_OpcodeList.insert(pair<string, string>("DIV", "04"));
        m_OpcodeList.insert(pair<string, string>("LOAD", "05"));
        m_OpcodeList.insert(pair<string, string>("STORE", "06"));
        m_OpcodeList.insert(pair<string, string>("READ", "07"));
        m_OpcodeList.insert(pair<string, string>("WRITE", "08"));
        m_OpcodeList.insert(pair<string, string>("B", "09"));
        m_OpcodeList.insert(pair<string, string>("BM", "10"));
        m_OpcodeList.insert(pair<string, string>("BZ", "11"));
        m_OpcodeList.insert(pair<string, string>("BP", "12"));
        m_OpcodeList.insert(pair<string, string>("HALT", "13"));
    }
    
    //the type of instruction being processed
    enum InstructionType
    {
        ST_MachineLanguage,              // A machine language instruction. 0
        ST_AssemblerInstr,               // Assembler Language instruction. 1
        ST_Comment,                      // Comment or blank line           2
        ST_End                           // end instruction.                3
    };

    inline string &GetLabel()
    {
        return m_Label;
    };
    
    // To determine if a label is blank.
    inline bool isLabel() const
    {
        return ! m_Label.empty();
    };
    
    inline string &GetOpcode()
    {
        return m_OpcodeList[m_OpCode];
    }
    
    inline string GetRegister() const
    {
        return m_Register;
    }
    
    inline string GetOperand() const
    {
        return m_Operand;
    }
    
    inline int GetOperandValue() const
    {
        return m_OperandValue;
    }
    
    // Identifies and parses each word in the instruction
    InstructionType ParseInstruction(const string&);
    
    
    /*                       The Processors                           */
    
    // Identifies and records different elements of a instruction with 4 words
    InstructionType FourInstrProcessor(const string [], const string []);
    
    // Identifies and records different elements of a instruction with 3 words
    InstructionType ThreeInstrProcessor(const string [], const string []);
    
    // Identifies and records different elements of a instruction with 2 words
    InstructionType TwoInstrProcessor(const string [], const string&);
    
    // Identifies and records the element of a instruction with 1 word
    InstructionType SingleInstrProcessor(const string&);
    
    /*                                                                 */
    
    
    /*              Machine Language Statement Translators             */
    
    // Records opcode, register, and operand of a 3-word machine language statement
    void ThreeWordMachineLan(const string&, const string&, const string&);
    
    // Records opcode and operand of a 2-word machine language statement
    bool TwoWordMachineLan(const string&, const string&);
    
    /*                                                                 */
    
    
    /*             Assembler Language Statement Translators            */
    
    // Checks and records the operand of a 3-word assembler language statement
    void ThreeWordAssembly(const string&, const string&);
    
    // Checks and records the operand for an ORG instruction
    void OriginInstr(const string&);
    
    // Checks and records the operand for a DC instruction
    void DC_Instr(const string&);
    
    /*                                                                 */
    
    // Computes the location of the next instruction
    int LocationNextInstruction(const int&) const;
    
    // Finds the number of words in the instruction
    int WordsToReadFinder(const string&) const;
    
    // Checks if symbol meets Quack3200 specifications
    bool SymbolValidation(const string&) const;
    
    // Checks Quack3200 comma rules
    bool CommaChecker(const string&) const;
    
    // Determines if all characters of a string are numeric
    bool IsInteger(const string&) const;
    
    
private:
    
    map<string, string> m_OpcodeList;   // The list of all Quack3200 opcodes

    string m_instruction;               // The original instruction
    
    // The elemements of a instruction
    string m_Label;                  // The label
    string m_Register;               // The register specified.  Use -1 if there is none.
    string m_OpCode;                 // The symbolic opcodes
    string m_Operand;                // The operand
    
    // Derived values.
    int m_NumOpCode;                 // The numerical value of the opcode
    int m_NumRegister;               // the numeric value for the register
    InstructionType m_type;          // The type of instruction
    
    bool m_IsNumericOperand;         // == true if the operand is numeric
    int m_OperandValue;              // The value of the operand if it is numeric
};
