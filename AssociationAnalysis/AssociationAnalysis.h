#pragma once
#include <vector>
#include <string>
#include <iostream>

using Itemset = std::vector<std::string>;

/* ------------------------------------------- *
 *         Interface for loading data.         *
 * ------------------------------------------- */

class DataLoader
{
public:
    virtual bool readTransaction(Itemset &retTrans) = 0;
    virtual void rewind() = 0;
};

/* ------------------------------------------- *
 *  Algorithms for finding frequent itemsets.  *
 * ------------------------------------------- */

struct FreqItemsetsResult
{
    std::vector<Itemset> itemsets;
    std::vector<int> supports;
};

/* Find frequent itemsets and return the result in 'FreqItemsetsResult'. */
FreqItemsetsResult apriori(DataLoader &loader, int minSupport);
FreqItemsetsResult FPGrowth(DataLoader &loader, int minSupport);

/* Utilities */
FreqItemsetsResult findFrequentOneItemsets(DataLoader &loader, int minSupport);
void setAprioriPruning(bool enable);

std::ostream &operator<<(std::ostream &os, const Itemset &itemset);
std::ostream &operator<<(std::ostream &os, const std::vector<Itemset> &itemsets);
std::ostream &operator<<(std::ostream &os, const FreqItemsetsResult &result);

/* --------------------------------------------- *
 *  Algorithms for analysing association rules.  *
 * --------------------------------------------- */

struct AssociationRule
{
    Itemset antecedent;
    Itemset consequent;
};

struct AssociationResult
{
    std::vector<AssociationRule> rules;
    std::vector<int> supports;
    std::vector<double> confidences;
};

/* Generate association rules from FreqItemsetsResult and return the
 * result in 'AssociationResult' */
AssociationResult generateAssociationRules(FreqItemsetsResult &result,
                                           double minConfidence);

/* Utilities */
std::ostream &operator<<(std::ostream &os, const AssociationRule &rule);
std::ostream &operator<<(std::ostream &os, const AssociationResult &result);
