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

    Stopwatch timer;

    FreqItemsetsResult freqItemsetsResult = FPGrowth(unixUsageLoader, 20);
    AssociationResult associationResult = generateAssociationRules(freqItemsetsResult, 0.7);
    cout << "Time consumed: " << timer << endl;

    cout << freqItemsetsResult << endl;
    cout << associationResult << endl;
}
