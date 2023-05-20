/**
 *@description: Optimization algorithm for TBC (TBC$^+$, TBC$^{++}$)
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Graph.hpp"
#include "TBCCore.hpp"
#include "Util.hpp"

struct TBCPlus {
    TBCPlus(Graph& g) : graph_(g) { graph_.sort_by_vp(); }

    // if data_structure = 0, use ListBucket
    // if data_structure = 1, use RBTree (default)
    std::vector<int64_t> operator()(int64_t delta, int data_struture = 1) {
        const auto& adj = graph_.adj_;

        std::vector<int64_t> count(TYPE_SIZE);
        for (int start : range(adj.size())) {
            int layer = graph_.layer(start);
            std::unordered_map<int, Sequence> hash_map;
            {
                std::unordered_map<int, int> end_back_middle;
                for (const auto& e1 : adj[start]) {
                    int middle = e1.to;
                    if (!graph_.priority(start, middle))
                        break;
                    for (const auto& e2 : adj[middle]) {
                        int end = e2.to;
                        if (!graph_.priority(start, end))
                            break;
                        if (e2.time == e1.time || std::abs(e2.time - e1.time) > delta)
                            continue;
                        if (!hash_map.count(end)) {
                            hash_map[end] = Sequence();
                            hash_map[end].emplace_back();
                            end_back_middle[end] = middle;
                        }
                        if (middle != end_back_middle[end]) {
                            hash_map[end].emplace_back();
                            end_back_middle[end] = middle;
                        }
                        if (e1.time < e2.time) {
                            hash_map[end].back().first.emplace_back(e1.time, e2.time);
                        } else {
                            // if (e1.time > e2.time) swap
                            hash_map[end].back().second.emplace_back(e2.time, e1.time);
                        }
                    }
                }
            }
            for (auto& _ : hash_map) {
                const auto& end = _.first;
                auto& sequences = _.second;
                if (sequences.size() <= 1)
                    continue;
                for (auto& bkt : sequences) {
                    std::sort(bkt.first.begin(), bkt.first.end(), [&](const Interval& lhs, const Interval& rhs) {
                        if (lhs.first != rhs.first)
                            return lhs.first > rhs.first;
                        return lhs.second < rhs.second;
                    });
                    std::sort(bkt.second.begin(), bkt.second.end(), [&](const Interval& lhs, const Interval& rhs) {
                        if (lhs.first != rhs.first)
                            return lhs.first > rhs.first;
                        return lhs.second < rhs.second;
                    });
                }
                std::function<void(int, int)> merge_cal = [&](int l, int r) {
                    if (l >= r - 1)
                        return;
                    int m = (l + r) / 2;
                    merge_cal(l, m);
                    merge_cal(m, r);
                    if (data_struture == 0) {
                        set_cross_lb(count, layer, delta, sequences[l], sequences[m]);
                    } else {
                        set_cross_rbtree(count, layer, delta, sequences[l], sequences[m]);
                    }
                };
                merge_cal(0, sequences.size());
            }
        }
        return count;
    }

    Graph& graph_;
};