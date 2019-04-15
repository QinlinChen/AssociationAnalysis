#include "AssociationAnalysis.h"
#include <unordered_map>
#include <algorithm>
#include <cassert>
using namespace std;

vector<Itemset> generateCandidates(const vector<Itemset> &freqItemsets, int minSupport);
void resultMerge(FreqItemsetsResult &target, const FreqItemsetsResult &result);
bool isJoinable(const Itemset &itemset1, const Itemset &itemset2);
Itemset join(const Itemset &itemset1, const Itemset &itemset2);
bool hasInfrequentSubset(const Itemset &candidate,
						 const vector<Itemset> &freqItemsets);
FreqItemsetsResult filterCandidates(const vector<Itemset> &candidates,
									DataLoader &loader, int minSupport);

FreqItemsetsResult apriori(DataLoader &loader, int minSupport)
{
	FreqItemsetsResult result;

	auto freqItemsetsResult = findFrequentOneItemsets(loader, minSupport);
	while (!freqItemsetsResult.itemsets.empty()) {
		resultMerge(result, freqItemsetsResult);
		auto candidates = generateCandidates(freqItemsetsResult.itemsets, minSupport);
		freqItemsetsResult = filterCandidates(candidates, loader, minSupport);
	}

	return result;
}

FreqItemsetsResult findFrequentOneItemsets(DataLoader &loader, int minSupport)
{
	Itemset transaction;
	unordered_map<string, int> ctr;
	loader.rewind();
	while (loader.readTransaction(transaction))
		for (auto &item : transaction)
			ctr[item]++;

	FreqItemsetsResult result;
	for (auto &pair : ctr)
		if (pair.second >= minSupport) {
			result.itemsets.push_back(Itemset(1, pair.first));
			result.supports.push_back(pair.second);
		}
	return result;
}

vector<Itemset> generateCandidates(const vector<Itemset> &freqItemsets, int minSupport)
{
	assert(!freqItemsets.empty());
	vector<Itemset> candidates;
	for (auto &itemset1 : freqItemsets)
		for (auto &itemset2 : freqItemsets)
			if (isJoinable(itemset1, itemset2)) {
				auto candidate = join(itemset1, itemset2);
				if (!hasInfrequentSubset(candidate, freqItemsets))
					candidates.push_back(candidate);
			}
	return candidates;
}

void resultMerge(FreqItemsetsResult &target, const FreqItemsetsResult &result)
{
	for (auto &itemset : result.itemsets)
		target.itemsets.push_back(itemset);
	for (auto support : result.supports)
		target.supports.push_back(support);
}

bool isJoinable(const Itemset &itemset1, const Itemset &itemset2)
{
	assert(itemset1.size() == itemset2.size());
	size_t k = itemset1.size();
	for (size_t i = 0; i < k - 1; ++i)
		if (itemset1[i] != itemset2[i])
			return false;
	return itemset1[k - 1] < itemset2[k - 1];
}

Itemset join(const Itemset &itemset1, const Itemset &itemset2)
{
	Itemset newItemset = itemset1;
	newItemset.push_back(itemset2.back());
	return newItemset;
}

bool hasInfrequentSubset(const Itemset &candidate,
						 const vector<Itemset> &freqItemsets)
{
	if (candidate.size() == 2)
		return false;
	for (size_t i = 0; i < candidate.size(); ++i) {
		Itemset candidateSubset;
		for (size_t j = 0; j < candidate.size(); ++j)
			if (j != i)
				candidateSubset.push_back(candidate[j]);
		auto pred = [&](const Itemset &itemset) {
			return candidateSubset == itemset;
		};
		if (find_if(freqItemsets.begin(), freqItemsets.end(), pred)
			== freqItemsets.end())
			return true;
	}
	return false;
}

FreqItemsetsResult filterCandidates(const vector<Itemset> &candidates,
									DataLoader &loader, int minSupport)
{
	FreqItemsetsResult result;
	vector<int> ctr(candidates.size(), 0);

	Itemset transaction;
	loader.rewind();
	while (loader.readTransaction(transaction)) {
		std::sort(transaction.begin(), transaction.end());
		for (size_t i = 0; i < candidates.size(); ++i) {
			const Itemset &candidate = candidates[i];
			if (includes(transaction.begin(), transaction.end(),
						 candidate.begin(), candidate.end()))
				ctr[i]++;
		}
	}
	for (size_t i = 0; i < candidates.size(); ++i)
		if (ctr[i] >= minSupport) {
			result.itemsets.push_back(candidates[i]);
			result.supports.push_back(ctr[i]);
		}

	return result;
}

ostream &operator<<(ostream &os, const Itemset &itemset)
{
	for (size_t i = 0; i < itemset.size(); ++i) {
		os << itemset[i];
		if (i + 1 != itemset.size())
			cout << ", ";
	}
	return os;
}

ostream &operator<<(ostream &os, const vector<Itemset> &itemsets)
{
	for (auto &itemset : itemsets)
		os << itemset << endl;
	return os;
}

ostream &operator<<(ostream &os, const FreqItemsetsResult &result)
{
	size_t size = result.itemsets.size();
	assert(size == result.supports.size());
	os << "The number of Frequent itemsets: " << size << endl;
	for (size_t i = 0; i < size; ++i)
		os << result.itemsets[i] << ": " << result.supports[i] << endl;
	return os;
}
