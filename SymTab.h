//
//        Symbol table class.
//        Adds, looks up and displays symbols of the program
#pragma once

// This class is our symbol table
class SymbolTable
{
    
public:
    
    const int multiplyDefinedSymbol = -999;

    // Add a new symbol to the symbol table
    void AddSymbol(const string&, int&);

    // Display the symbol table
    void DisplaySymbolTable() const;

    // Lookup a symbol in the symbol table
    bool LookupSymbol(const string&, int&) const;

private:
    
    // This is the actual symbol table.
    // The symbol is the key, the location is the value.
    map<string, int> m_symbolTable;
    
};


