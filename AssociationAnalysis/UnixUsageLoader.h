#pragma once
#include "AssociationAnalysis.h"
#include <fstream>

class UnixUsageLoader : public DataLoader
{
public:
    UnixUsageLoader(int userId);
    ~UnixUsageLoader();

    bool readTransaction(Itemset &transaction);
    void rewind();

private:
    std::fstream fin;
};

