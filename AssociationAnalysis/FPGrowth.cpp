#include "AssociationAnalysis.h"
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cassert>
using namespace std;

/* ---------------------------------------------- *
 *   Declerations of FPTree used by FP-Growth.    *
 * ---------------------------------------------- */

class FPTreeNode;

class LessFPTreeNodePtr
{
public:
    bool operator()(const FPTreeNode *lhs, const FPTreeNode *rhs) const;
};

class FPTreeNode
{
public:
    FPTreeNode(string item_ = "", int count_ = 0, FPTreeNode *parent_ = nullptr)
        : item(item_), count(count_), parent(parent_) {}

    string item;
    int count;
    FPTreeNode *parent;
    set<FPTreeNode *, LessFPTreeNodePtr> children;
};

class HeaderTableEntry
{
public:
    HeaderTableEntry() : supportCount(0) {}

    int supportCount;
    list<FPTreeNode *> nodeList;
};

class FPTree
{
public:
    FPTree() : root() {}
    FPTree(DataLoader &loader, int minSupport);
    ~FPTree();

    void addItemset(const Itemset &itemset, int count = 1);
    Itemset getAscendingFrequentItems() const;
    void constructConditionalFPTree(const string &item, int minSupport,
                                    FPTree &retTree) const;
    int querySupportCount(const string &item) const;
    void print(ostream &os) const;

private:
    FPTreeNode root;
    unordered_map<string, HeaderTableEntry> headerTable;
};

void deleteFPTree(FPTreeNode *node);
void printFPTree(ostream &os, const FPTreeNode *tree, int depth);

/* ------------------------------------------- *
 *        Implementations of FPTree            *
 * ------------------------------------------- */

bool LessFPTreeNodePtr::operator()(const FPTreeNode *lhs, const FPTreeNode *rhs) const
{
    return lhs->item < rhs->item;
}

FPTree::FPTree(DataLoader &loader, int minSupport)
    : root()
{
    FreqItemsetsResult result = findFrequentOneItemsets(loader, minSupport);
    /* Construct a map from frequent items to the support count. */
    unordered_map<string, int> supportMap;
    for (size_t i = 0; i < result.itemsets.size(); ++i) {
        assert(result.itemsets[i].size() == 1);
        supportMap[result.itemsets[i][0]] = result.supports[i];
    }
    auto pred = [&](const string &lhs, const string &rhs) {
        return (supportMap[lhs] > supportMap[rhs]);
    };	/* Used to sort frequent items according to the support count. */

    Itemset transaction;
    loader.rewind();
    while (loader.readTransaction(transaction)) {
        Itemset freqItems;
        /* Ignore infrequent items in a transaction. */
        for (auto &item : transaction)
            if (supportMap.find(item) != supportMap.end())
                freqItems.push_back(item);
        /* Add sorted frequent items to FPTree. */
        if (!freqItems.empty()) {
            sort(freqItems.begin(), freqItems.end(), pred);
            addItemset(freqItems);
        }
    }
}

void FPTree::addItemset(const Itemset &itemset, int count)
{
    FPTreeNode *cur = &root;
    for (auto &item : itemset) {
        /* Add item to FPTree rooted by current node */
        FPTreeNode target(item);
        auto pchild = cur->children.find(&target);
        if (pchild != cur->children.end()) {
            (*pchild)->count += count;
            /* Besides, update the entry in headTable. */
            headerTable[item].supportCount += count;
            /* Remember to update current node */
            cur = *pchild;
        }
        else {
            FPTreeNode *newNode = new FPTreeNode(item, count, cur);
            cur->children.insert(newNode);
            /* Besides, add the new node to headTable. */
            HeaderTableEntry &entry = headerTable[item];
            entry.supportCount += count;
            entry.nodeList.push_back(newNode);
            /* Remember to update current node */
            cur = newNode;
        }
    }
}

Itemset FPTree::getAscendingFrequentItems() const
{
    Itemset freqentItems;
    for (auto &p : headerTable)
        freqentItems.push_back(p.first);

    auto pred = [&](const string &lhs, const string &rhs) {
        return headerTable.at(lhs).supportCount < headerTable.at(rhs).supportCount;
    };
    sort(freqentItems.begin(), freqentItems.end(), pred);

    return freqentItems;
}

void FPTree::constructConditionalFPTree(const string &item, int minSupport,
                                        FPTree &retTree) const
{
    /* Scan conditonal pattern bases and find all items that suffice minSupport */
    unordered_map<string, int> ctr;
    for (auto node : headerTable.at(item).nodeList) {
        auto cur = node->parent;
        assert(cur);
        while (cur->parent) {
            ctr[cur->item] += node->count;
            cur = cur->parent;
        }
    }
    unordered_map<string, int> supportMap; /* Same as the one in FPTree() */
    for (auto &pair : ctr)
        if (pair.second >= minSupport)
            supportMap[pair.first] = pair.second;
    auto pred = [&](const string &lhs, const string &rhs) {
        return (supportMap[lhs] > supportMap[rhs]);
    };	/* Used to sort frequent items according to the support count. */

    /* Scan conditional pattern bases again to construct conditional FPTree */
    for (auto node : headerTable.at(item).nodeList) {
        Itemset conditionalBase;
        auto cur = node->parent;
        assert(cur);
        while (cur->parent) {
            if (supportMap.find(cur->item) != supportMap.end())
                conditionalBase.push_back(cur->item);
            cur = cur->parent;
        }
        if (!conditionalBase.empty()) {
            sort(conditionalBase.begin(), conditionalBase.end(), pred);
            retTree.addItemset(conditionalBase, node->count);
        }
    }
}

int FPTree::querySupportCount(const string &item) const
{
    auto iter = headerTable.find(item);
    if (iter == headerTable.end())
        return 0;
    return iter->second.supportCount;
}

void deleteFPTree(FPTreeNode *node)
{
    if (!node)
        return;
    for (auto child : node->children)
        deleteFPTree(child);
    delete node;
}

FPTree::~FPTree()
{
    for (auto child : root.children)
        deleteFPTree(child);
}

void printFPTree(ostream &os, const FPTreeNode *tree, int depth)
{
    for (int i = 0; i < depth; ++i)
        os << "  ";
    os << tree->item << "(" << tree->count << ")" << endl;
    for (auto child : tree->children)
        printFPTree(os, child, depth + 1);
}

void FPTree::print(ostream &os) const
{
    printFPTree(os, &root, 0);
    for (auto &pair : headerTable) {
        os << pair.first << ": " << pair.second.supportCount << ", "
            << pair.second.nodeList.size() << endl;
    }
}

/* ------------------------------------------- *
 *        Implementations of FP-Growth         *
 * ------------------------------------------- */

void FPGrowth(const FPTree &tree, Itemset &pattern, int minSupport,
              FreqItemsetsResult &result);

FreqItemsetsResult FPGrowth(DataLoader &loader, int minSupport)
{
    FreqItemsetsResult result;
    FPTree tree(loader, minSupport);
    Itemset pattern;

    FPGrowth(tree, pattern, minSupport, result);

    return result;
}

void FPGrowth(const FPTree &tree, Itemset &pattern, int minSupport,
              FreqItemsetsResult &result)
{
    Itemset items = tree.getAscendingFrequentItems();
    for (auto &item : items) {
        pattern.push_back(item);
        /* Add a new itemset constructed from pattern */
        result.itemsets.push_back(pattern);
        result.supports.push_back(tree.querySupportCount(item));
        /* Construct conditional FPTree and recursively expand the pattern */
        FPTree conditionalFPTree;
        tree.constructConditionalFPTree(item, minSupport, conditionalFPTree);
        FPGrowth(conditionalFPTree, pattern, minSupport, result);
        pattern.pop_back();
    }
}