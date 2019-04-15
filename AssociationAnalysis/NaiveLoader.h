#pragma once
#include "AssociationAnalysis.h"
#include <fstream>

class NaiveLoader : public DataLoader
{
public:
    NaiveLoader();
    ~NaiveLoader();

    bool readTransaction(Itemset &transaction);
    void rewind();

private:
    std::fstream fin;
};
