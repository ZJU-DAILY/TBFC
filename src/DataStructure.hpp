/**
 *@description: ListBucket (i.e., HP in paper), RBTree (i.e., TS, TA in paper)
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Core.h"

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;

using ListBucket = std::map<int64_t, std::vector<int64_t>>;

void delete_itv(int64_t bound, ListBucket& lb) {
    for (auto iter = lb.begin(); iter != lb.end();) {
        if (iter->first > bound) {
            iter = lb.erase(iter);
        } else {
            auto& vec = iter->second;
            auto ptr = std::upper_bound(vec.begin(), vec.end(), bound);
            if (ptr != vec.end()) {
                vec.erase(ptr, vec.end());
            }
            if (vec.empty()) {
                iter = lb.erase(iter);
            } else {
                iter++;
            }
        }
    }
}
void insert_itv(const std::pair<int64_t, int64_t>& itv, ListBucket& lb) {
    if (lb.count(itv.first)) {
        lb[itv.first].push_back(itv.second);
    } else {
        lb[itv.first] = {itv.second};
    }
}

using RBTree_G = tree<std::pair<ino64_t, int>,
                      int64_t,
                      std::less<std::pair<int64_t, int>>,
                      rb_tree_tag,
                      tree_order_statistics_node_update>;
inline int count_less(int64_t t2, const RBTree_G& tree) {
    return tree.order_of_key({t2, -1});
}
inline int count_greater(int64_t t2, const RBTree_G& tree) {
    return tree.size() - tree.order_of_key({t2 + 1, -1});
}
inline void print_tree(const RBTree_G& tree) {
    for (auto& item : tree) {
        std::cout << "((t1=" << item.second;
        std::cout << ",t2=" << item.first.first;
        std::cout << "),idx=" << item.first.second;
        std::cout << ") ";
    }
    std::cout << "\n";
}

using RBTree_S = tree<std::pair<ino64_t, int>,
                      null_type,
                      std::less<std::pair<ino64_t, int>>,
                      rb_tree_tag,
                      tree_order_statistics_node_update>;
inline int count_less(int64_t t1, const RBTree_S& tree) {
    return tree.order_of_key({t1, -1});
}
inline int count_greater(int64_t t1, const RBTree_S& tree) {
    return tree.size() - tree.order_of_key({t1 + 1, -1});
}
inline int count_ge(int64_t t1, const RBTree_S& tree) {
    return tree.size() - tree.order_of_key({t1, -1});
}
inline void print_tree(const RBTree_S& tree) {
    for (auto& item : tree) {
        std::cout << "(t1=" << item.first;
        std::cout << ",idx=" << item.second;
        std::cout << ") ";
    }
    std::cout << "\n";
}

// insert interval into two tree, idx is the index of interval for unique
void insert_itv(std::pair<int64_t, int64_t> itv, int idx, RBTree_G& tree_g, RBTree_S& tree_s) {
    tree_g.insert({std::make_pair(itv.second, idx), itv.first});
    tree_s.insert({itv.first, idx});
}
// delete node in two tree, bound = t1 + delta
void delete_itv(int64_t bound, RBTree_G& tree_g, RBTree_S& tree_s) {
    for (auto iter = tree_g.rbegin(); iter != tree_g.rend();) {
        if (iter->first.first > bound) {
            int t1 = iter->second;
            tree_s.erase(tree_s.lower_bound({iter->second, iter->first.second}));
            iter = tree_g.erase(iter);
        } else {
            break;
        }
    }
}

using ListBucketM = std::map<int64_t, std::vector<std::pair<int64_t, int>>>;

void delete_itv(int64_t bound, ListBucketM& lb) {
    for (auto iter = lb.begin(); iter != lb.end();) {
        if (iter->first > bound) {
            iter = lb.erase(iter);
        } else {
            auto& vec = iter->second;
            auto ptr = std::lower_bound(vec.begin(), vec.end(), std::make_pair(bound + 1, -1));
            if (ptr != vec.end()) {
                vec.erase(ptr, vec.end());
            }
            if (vec.empty()) {
                iter = lb.erase(iter);
            } else {
                iter++;
            }
        }
    }
}
void insert_itv(const std::tuple<int64_t, int64_t, int>& itvm, ListBucketM& lb) {
    if (!lb.count(std::get<0>(itvm))) {
        lb[std::get<0>(itvm)] = {};
    }
    lb[std::get<0>(itvm)].emplace_back(std::get<1>(itvm), std::get<2>(itvm));
}