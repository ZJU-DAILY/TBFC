/**
 *@description: temporal butterfly counting (baseline)
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Graph.hpp"
#include "Util.hpp"
#include "TBCCore.hpp"

struct TBC {
    TBC(Graph& g) : graph_(g) { graph_.sort_by_vp(); }

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
                        check_wedge_pair_n_add(count, delta, layer, wedges[i], wedges[j]);
                    }
                }
            }
        }
        return count;
    }

    Graph& graph_;
};