#pragma once
#include "AssociationAnalysis.h"
#include <fstream>

class GroceriesLoader : public DataLoader
{
public:
    GroceriesLoader();
    ~GroceriesLoader();

    bool readTransaction(Itemset &transaction);
    void rewind();

private:
    std::fstream fin;
};
