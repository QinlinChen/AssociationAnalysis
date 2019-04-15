#include "GroceriesLoader.h"
#include <sstream>
using namespace std;

GroceriesLoader::GroceriesLoader()
    : fin("dataset/GroceryStore/Groceries.csv")
{
    if (!fin) {
        cerr << "Fail to load dataset." << endl;
        exit(1);
    }
}

GroceriesLoader::~GroceriesLoader()
{
}

bool GroceriesLoader::readTransaction(Itemset &transaction)
{
    string line;
    size_t left, right;
    transaction.clear();

    while (true) {
        if (!getline(fin, line))
            return false;
        left = line.find_first_of('{');
        right = line.find_first_of('}');
        if (left == line.npos || right == line.npos)
            continue;
        if (left >= right) {
            cerr << "Fail to find transaction." << endl;
            exit(1);
        }
        break;
    }

    istringstream iss(line.substr(left + 1, right - left - 1));
    string item;
    while (getline(iss, item, ','))
        transaction.push_back(item);
    return true;
}

void GroceriesLoader::rewind()
{
    fin.clear();
    fin.seekg(0);
}
