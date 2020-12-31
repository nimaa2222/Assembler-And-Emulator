//
//        Implementation of the symbol table class  
//

#include "stdafx.h"

/*
NAME
 
    AddSymbol - adds a new symbol to the symbol table.

SYNOPSIS
 
    void AddSymbol(const string &a_symbol, int& a_loc);

DESCRIPTION
 
    This function will place the symbol "a_symbol" and
    its location "a_loc" in the symbol table.
*/

void SymbolTable::AddSymbol(const string &a_symbol, int& a_loc)
{
    // if the symbol is already in the symbol table
    if (LookupSymbol(a_symbol, a_loc))
    {
        m_symbolTable[a_symbol] = multiplyDefinedSymbol;
        return;
    }
    
    // otherwise record the symbol and its location
    m_symbolTable[a_symbol] = a_loc;
}
/*void SymbolTable::AddSymbol(const string &a_symbol, int& a_loc); */


/*
NAME
 
    DisplaySymbolTable - Displays all symbols and their associated location

SYNOPSIS
 
    void DisplaySymbolTable() const;

DESCRIPTION
 
    This function will output the symbol and its
    location and the order the symbols appear in.
*/

void SymbolTable::DisplaySymbolTable() const
{
    cout<<"SYMBOL TABLE:"<<endl<<endl;
    cout<<"SYMBOL#"<<setw(5)<<"   SYMBOL          "<<setw(5)<<"LOCATION"<<endl;
    
    int symbolNumber = 0;
    
    //display the symbol table
    for(auto element : m_symbolTable)
    {
        //setting up the format of the table
        std::cout<< left<<setw(11)<< symbolNumber <<setw(17)<<element.first << " ";
        
        if (element.second == multiplyDefinedSymbol)
        {
            cout<<"???"<<endl;
            
            //Code 9: Multiply Defined Label
            Errors::RecordError(9, element.first);
        }
    
        else
            cout<< element.second<<endl;
        
       symbolNumber++;
    }
    
    cout<<endl<<endl<<"Press [Enter] to continue . . ."<<endl;
    cin.get();
    cout<<endl;
}
/*void SymbolTable::DisplaySymbolTable() const; */


/*
NAME
 
    LookupSymbol - Searches for a symbol in the symmbol table

SYNOPSIS
 
    bool LookupSymbol(const string &a_symbol, int &a_loc) const;

DESCRIPTION
 
    This function will determine whether "a_symbol"
    symbol exists in the symbol table. It will also return
    the location of the symbol in "a_loc" by reference.
*/

bool SymbolTable::LookupSymbol(const string &a_symbol, int &a_loc) const
{
    //if symbol exists
    if (m_symbolTable.find(a_symbol) != m_symbolTable.end())
    {
        //return its location by reference
        a_loc = (m_symbolTable.find(a_symbol))->second;
        
        return true;
    }
    
    //otherwise it is not defined
    return false;
}
/*bool SymbolTable::LookupSymbol(const string &a_symbol, int &a_loc) const; */
