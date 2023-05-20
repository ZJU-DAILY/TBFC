/**
 *@description: temporal butterfly enumeration (baseline)
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Graph.hpp"
#include "Util.hpp"
#include "TBECore.hpp"

struct TBE {
    TBE(Graph& g, bool print=false) : graph_(g), print_(print) {
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

        std::vector<int64_t> count(TYPE_SIZE);
        for (int start : range(adj.size())) {
            std::unordered_map<int, std::vector<Wedge>> hash_map;
            for (const auto& e1 : adj[start]) {
                int middle = e1.to;
                if (!graph_.priority(start, middle))
                    break;
                for (const auto& e2 : adj[middle]) {
                    int end = e2.to;
                    if (!graph_.priority(start, end))
                        break;
                    if (!hash_map.count(end)) {
                        hash_map[end] = std::vector<Wedge>();
                    }
                    hash_map[end].push_back({start, middle, end, e1.time, e2.time});
                }
            }
            int layer = graph_.layer(start);
            for (const auto& _ : hash_map) {
                const auto& end = _.first;
                const auto& wedges = _.second;
                for (int i : range(wedges.size())) {
                    for (int j = 0; j < i; ++j) {
                        check_wedge_pair_n_enu(print_, counts_, butterflies_, delta, layer, wedges[i], wedges[j]);
                    }
                }
            }
        }
        return counts_;
    }

    bool print_;
    Graph& graph_;
    std::vector<int64_t> counts_;
    std::vector<std::ofstream> butterflies_;
};