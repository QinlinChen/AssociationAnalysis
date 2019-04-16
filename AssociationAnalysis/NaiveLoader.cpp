#include "NaiveLoader.h"
#include <sstream>
using namespace std;

NaiveLoader::NaiveLoader()
    : fin("dataset/Naive/Naive.txt", fstream::in)
{
    if (!fin) {
        cerr << "Fail to load dataset." << endl;
        exit(1);
    }
}

NaiveLoader::~NaiveLoader()
{
}

bool NaiveLoader::readTransaction(Itemset &transaction)
{
    string line;
    transaction.clear();
    if (!getline(fin, line))
        return false;
    istringstream iss(line);
    string item;
    while (getline(iss, item, ','))
        transaction.push_back(item);
    return true;
}

void NaiveLoader::rewind()
{
    fin.clear();
    fin.seekg(0);
}
