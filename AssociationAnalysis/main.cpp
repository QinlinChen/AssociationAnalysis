#include "AssociationAnalysis.h"
#include "NaiveLoader.h"
#include "GroceriesLoader.h"
#include "UnixUsageLoader.h"
#include "Stopwatch.h"
#include <iostream>
using namespace std;

int main()
{
    NaiveLoader naiveLoader;
    GroceriesLoader groceriesLoader;
    UnixUsageLoader unixUsageLoader(0);
    const int minSupport = 20;
    const double minConf = 0.5;
    DataLoader &loader = unixUsageLoader;

    Stopwatch timer;
    cout << "Begin baseline..." << endl;
    setAprioriPruning(false);
    apriori(loader, minSupport);
    cout << "Time consumed: " << timer << endl;

    timer.reset();
    cout << "Begin apriori..." << endl;
    setAprioriPruning(true);
    FreqItemsetsResult aprioriResult = apriori(loader, minSupport);
    cout << "Time consumed: " << timer << endl;

    timer.reset();
    cout << "Begin FP-Growth..." << endl;
    FreqItemsetsResult FPGrowthResult = FPGrowth(loader, minSupport);
    cout << "Time consumed: " << timer << endl;

    if (aprioriResult.itemsets.size() != FPGrowthResult.itemsets.size()) {
        cerr << "Why different results?" << endl;
        exit(1);
    }

    cout << aprioriResult << endl;
    cout << "Begin generating association rules..." << endl;
    AssociationResult associationResult = generateAssociationRules(aprioriResult, minConf);
    cout << associationResult << endl;
}
