#include "AssociationAnalysis.h"
#include "NaiveLoader.h"
#include "GroceriesLoader.h"
#include "Stopwatch.h"
#include <iostream>
using namespace std;

int main()
{
	GroceriesLoader loader;
	Stopwatch timer;

	FreqItemsetsResult freqItemsetsResult = FPGrowth(loader, 100);
	AssociationResult result = generateAssociationRules(freqItemsetsResult, 0.5);
	cout << "Time consumed: " << timer << endl;

	cout << freqItemsetsResult << endl;
	cout << result << endl;
}
