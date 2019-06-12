#include "AssociationAnalysis.h"
#include "NaiveLoader.h"
#include "GroceriesLoader.h"
#include "UnixUsageLoader.h"
#include "Stopwatch.h"
#include <iostream>
using namespace std;

void profileMemory(int minSupport = 100)
{
    GroceriesLoader loader;
    cout << "Baseline..." << endl;
    setAprioriPruning(false);
    apriori(loader, minSupport);

    cout << "Apriori..." << endl;
    setAprioriPruning(true);
    apriori(loader, minSupport);

    cout << "FP-Growth..." << endl;
    FPGrowth(loader, minSupport);
}

void profileTime(int loop = 10, int minSupport = 100)
{
    GroceriesLoader loader;
    double baselineTime = 0.0;
    double aprioriTime = 0.0;
    double FPGrowthTime = 0.0;

    auto aprioriResult = apriori(loader, minSupport);
    auto FPGrowthResult = FPGrowth(loader, minSupport);
    if (aprioriResult.itemsets.size() != FPGrowthResult.itemsets.size()) {
        cerr << "Why different results?" << endl;
        exit(1);
    }

    cout << "Baseline..." << endl;
    for (int i = 0; i < loop; ++i) {
        Stopwatch timer;
        setAprioriPruning(false);
        apriori(loader, minSupport);
        baselineTime += timer.elapsedTime();
    }
    baselineTime /= loop;
    cout << "Avg time per loop(loop="<< loop <<"): " << baselineTime << endl;
    
    cout << "Apriori..." << endl;
    for (int i = 0; i < loop; ++i) {
        Stopwatch timer;
        setAprioriPruning(true);
        apriori(loader, minSupport);
        aprioriTime += timer.elapsedTime();
    }
    aprioriTime /= loop;
    cout << "Avg time per loop(loop=" << loop << "): " << aprioriTime << endl;

    cout << "FP-Growth..." << endl;
    for (int i = 0; i < loop; ++i) {
        Stopwatch timer;
        FPGrowth(loader, minSupport);
        FPGrowthTime += timer.elapsedTime();
    }
    FPGrowthTime /= loop;
    cout << "Avg time per loop(loop=" << loop << "): " << FPGrowthTime << endl;
}

void curve()
{
    GroceriesLoader loader;
    vector<int> minSupports;
    vector<int> freqItemsetsSizes;

    for (int i = 0; i < 16; ++i)
        minSupports.push_back(50 + 10 * i);

    setAprioriPruning(true);
    for (auto minSupport : minSupports) {
        auto result = apriori(loader, minSupport);
        freqItemsetsSizes.push_back(result.itemsets.size());
    }
    
    for (auto support : minSupports)
        cout << support << ", ";
    cout << endl;
    for (auto size : freqItemsetsSizes)
        cout << size << ", ";
    cout << endl;
}

void analyseGroceries(int minSupport = 100, double minConf = 0.5)
{
    GroceriesLoader loader;
    cout << "Begin searching frequent itemsets..." << endl;
    FreqItemsetsResult aprioriResult = apriori(loader, minSupport);
    cout << "Begin generating association rules..." << endl;
    AssociationResult associationResult = generateAssociationRules(aprioriResult, minConf);
    cout << associationResult << endl;
}

void analyseUnixUsage(double minConf = 0.9)
{
    vector<int> minSupports = { 40, 70, 50, 70, 80, 60, 250, 40, 200 };
    for (int i = 0; i < 9; ++i) {
        UnixUsageLoader loader(i);
        cout << "Begin searching frequent itemsets..." << endl;
        FreqItemsetsResult aprioriResult = apriori(loader, minSupports[i]);
        cout << "Begin generating association rules..." << endl;
        AssociationResult associationResult = generateAssociationRules(aprioriResult, minConf);
        cout << associationResult << endl;
    }
}

int main()
{
    analyseGroceries();
    analyseUnixUsage();
}
