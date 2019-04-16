#include "UnixUsageLoader.h"
#include <sstream>
#include <set>
using namespace std;

UnixUsageLoader::UnixUsageLoader(int userId)
{
    if (userId < 0 || userId > 8) {
        cerr << "UserId should be between 0 and 8" << endl;
        exit(1);
    }
    ostringstream oss;
    oss << "dataset/UNIX_usage/USER" << userId << "/sanitized_all.981115184025";
    fin.open(oss.str(), fstream::in);
    if (!fin) {
        cerr << "Fail to load dataset." << endl;
        exit(1);
    }
}

UnixUsageLoader::~UnixUsageLoader()
{
}

bool UnixUsageLoader::readTransaction(Itemset &transaction)
{
    string line;
    string itembuf;
    set<string> itemset;
    transaction.clear();

    while (line != "**SOF**")
        if (!getline(fin, line))
            return false;

    while (getline(fin, line)) {
        if (line == "**EOF**") {
            if (!itembuf.empty())
                itemset.insert(itembuf);
            break;
        }
        if (!line.empty()) {
            if (line.front() == '-' ||
                (line.front() == '<' && line.back() == '>')) {
                itembuf += line;
            }
            else {
                if (!itembuf.empty())
                    itemset.insert(itembuf);
                itembuf = line;
            }
        }
    }
    copy(itemset.begin(), itemset.end(), back_inserter(transaction));
    return true;
}

void UnixUsageLoader::rewind()
{
    fin.clear();
    fin.seekg(0);
}
