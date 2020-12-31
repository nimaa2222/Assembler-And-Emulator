//
//        Assembler class.  This is a container for all the components
//        that make up the assembler.
//

#pragma once

class Assembler
{

public:
    
    Assembler(int argc, char *argv[]);

    // Pass I - establish the locations of the symbols
    void PassI();
    
    // Display the symbols in the symbol table
    void DisplaySymbolTable() const {m_symtab.DisplaySymbolTable();}

    // Pass II - generate a translation
    void PassII();
    
    // Checks if symbol is defined only once
    bool HasSymbolError(const string&, string&, int&);
    
    // Translates a DC statement
    void DefinedConstantTranslation (string&) const;
    
    // Identifies the specific type of assembler language
    string QuickParse (const string&, string&) const;

    // Displays the translation made in Pass II
    void DisplayTranslation(const int&, const string&,
            const string&, const Instruction::InstructionType&);
    
    // Run emulator on the translation
    void RunProgramInEmulator();
    
private:

    FileAccess m_facc;          // File Access object
    SymbolTable m_symtab;       // Symbol table object
    Instruction m_inst;         // Instruction object
    Emulator m_emul;            // Emulator object
};
