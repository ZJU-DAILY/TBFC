/**
 *@description: Optimization algorithm for TBE (TBE$^+$)
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Graph.hpp"
#include "TBECore.hpp"
#include "Util.hpp"

struct TBEPlus {
    TBEPlus(Graph& g, bool print=false) : graph_(g), print_(print) {
        graph_.sort_by_vp();
        counts_.resize(TYPE_SIZE, 0);
        butterflies_.resize(TYPE_SIZE);
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        if (print_) {
            for (int i = 0; i < TYPE_SIZE; ++i) {
                butterflies_[i].open("../output/" + std::to_string(timestamp) + "-tbf" + std::to_string(i) + ".ins");
                if (butterflies_[i].fail()) {
                    std::cerr << "Error: open failed!\n";
                    return;
                }
            }
        }
    }

    std::vector<int64_t> operator()(int64_t delta) {
        const auto& adj = graph_.adj_;

        for (int start : range(adj.size())) {
            int layer = graph_.layer(start);
            std::unordered_map<int, SequenceM> hash_map;
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
                            hash_map[end] = {};
                            hash_map[end].emplace_back();
                            end_back_middle[end] = middle;
                        }
                        if (middle != end_back_middle[end]) {
                            hash_map[end].emplace_back();
                            end_back_middle[end] = middle;
                        }
                        if (e1.time < e2.time) {
                            hash_map[end].back().first.emplace_back(e1.time, e2.time, middle);
                        } else {
                            // if (e1.time > e2.time) swap
                            hash_map[end].back().second.emplace_back(e2.time, e1.time, middle);
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
                    std::sort(bkt.first.begin(), bkt.first.end(), [&](const IntervalM& lhs, const IntervalM& rhs) {
                        if (std::get<0>(lhs) != std::get<0>(rhs))
                            return std::get<0>(lhs) > std::get<0>(rhs);
                        return std::get<1>(lhs) < std::get<1>(rhs);
                    });
                    std::sort(bkt.second.begin(), bkt.second.end(), [&](const IntervalM& lhs, const IntervalM& rhs) {
                        if (std::get<0>(lhs) != std::get<0>(rhs))
                            return std::get<0>(lhs) > std::get<0>(rhs);
                        return std::get<1>(lhs) < std::get<1>(rhs);
                    });
                }
                std::function<void(int, int)> merge_cal = [&](int l, int r) {
                    if (l >= r - 1)
                        return;
                    int m = (l + r) / 2;
                    merge_cal(l, m);
                    merge_cal(m, r);
                    set_cross_lb_enu(print_, counts_, butterflies_, layer, delta, sequences[l], sequences[m], start, end);
                };
                merge_cal(0, sequences.size());
            }
        }
        return counts_;
    }

    bool print_;
    Graph& graph_;
    std::vector<int64_t> counts_;
    std::vector<std::ofstream> butterflies_;
};