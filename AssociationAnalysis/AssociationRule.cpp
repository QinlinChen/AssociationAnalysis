#include "AssociationAnalysis.h"
#include <map>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <cassert>
using namespace std;

void analyseItemset(const Itemset &itemset, const map<Itemset, int> &supportMap,
                    double minConf, AssociationResult &result);
void analyseItemset(Itemset &antecedent, const Itemset &itemset,
                    int itemsetSupport, int depth,
                    const map<Itemset, int> &supportMap, double minConf,
                    AssociationResult &result);

AssociationResult generateAssociationRules(FreqItemsetsResult &result,
                                           double minConfidence)
{
    /* Make sure that itemsets in result are ordered. */
    for (auto &itemset : result.itemsets)
        sort(itemset.begin(), itemset.end());

    map<Itemset, int> supportMap;
    assert(result.itemsets.size() == result.supports.size());
    for (size_t i = 0; i < result.itemsets.size(); ++i)
        supportMap.insert(make_pair(result.itemsets[i], result.supports[i]));

    AssociationResult assocResult;
    for (auto &itemset : result.itemsets)
        analyseItemset(itemset, supportMap, minConfidence, assocResult);

    return assocResult;
}

void analyseItemset(const Itemset &itemset, const map<Itemset, int> &supportMap,
                    double minConf, AssociationResult &result)
{
    if (itemset.size() <= 1)
        return;
    Itemset antecedent;
    analyseItemset(antecedent, itemset, supportMap.at(itemset), 0,
                   supportMap, minConf, result);
}

/* Recursively find all antecedents from itemset,
 * which are non-empty subsets of itemset. */
void analyseItemset(Itemset &antecedent, const Itemset &itemset,
                    int itemsetSupport, int depth,
                    const map<Itemset, int> &supportMap, double minConf,
                    AssociationResult &result)
{
    if (depth == itemset.size()) {
        if (antecedent.empty() || antecedent.size() == itemset.size())
            return;
        double conf = (double)itemsetSupport / supportMap.at(antecedent);
        if (conf >= minConf) {
            /* Construct association rule. */
            AssociationRule rule;
            rule.antecedent = antecedent;
            set_difference(itemset.begin(), itemset.end(),
                           antecedent.begin(), antecedent.end(),
                           back_inserter(rule.consequent));
            /* Add the rule to 'result' with the support count and confidence. */
            result.rules.push_back(rule);
            result.supports.push_back(itemsetSupport);
            result.confidences.push_back(conf);
        }
        return;
    }

    analyseItemset(antecedent, itemset, itemsetSupport, depth + 1,
                   supportMap, minConf, result);
    antecedent.push_back(itemset[depth]);
    analyseItemset(antecedent, itemset, itemsetSupport, depth + 1,
                   supportMap, minConf, result);
    antecedent.pop_back();
}

ostream &operator<<(ostream &os, const AssociationRule &rule)
{
    os << rule.antecedent << " -> " << rule.consequent;
    return os;
}

ostream &operator<<(ostream &os, const AssociationResult &result)
{
    size_t size = result.rules.size();
    assert(size == result.supports.size());
    assert(size == result.confidences.size());
    os << "The number of association rules: " << size << endl;
    for (size_t i = 0; i < size; ++i) {
        os << result.rules[i] << " [support = " << result.supports[i]
            << ", confidence = " << setiosflags(ios::fixed) << setprecision(2)
            << 100 * result.confidences[i] << "%]" << endl;
    }
    return os;
}
