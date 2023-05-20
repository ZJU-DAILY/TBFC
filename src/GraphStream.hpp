/**
 *@description: Data loader for streaming temporal bipartite graph
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Graph.hpp"

struct StreamGraph {
    int64_t load(std::string file_path, bool print = false) {
        std::ifstream in_file(file_path);
        if (in_file.fail()) {
            std::cerr << "Error: open " << file_path << " failed!\n";
            exit(EXIT_FAILURE);
        }
        ptr_l_ = 0;
        ptr_r_ = 0;
        in_file >> U_ >> L_;
        in_file.ignore();
        adj_.resize(U_ + L_);
        std::string line;
        int64_t maxn_t = I64_MIN, minn_t = I64_MAX;
        while (std::getline(in_file, line)) {
            std::istringstream in_str(line);
            int from, to;
            int64_t time;
            in_str >> from >> to >> time;
            maxn_t = std::max(maxn_t, time);
            minn_t = std::min(minn_t, time);
            from = from;
            to = to + U_;
            edge_stream_.push_back({from, to, time});
        }
        std::sort(edge_stream_.begin(), edge_stream_.end(),
                  [&](const auto& lhs, const auto& rhs) { return lhs.time < rhs.time; });
        if (print) {
            std::cout << "|U| = " << U_ << ", |L| = " << L_;
            std::cout << ", |V| = " << U_ + L_ << ", |E| = " << edge_stream_.size() << '\n';
            std::cout << "TimeSpan(TS) = " << maxn_t - minn_t << "(timestamps)";
            std::cout << " or = " << 1.0 * (maxn_t - minn_t) / 60 / 60 / 24 << "(days)\n";
        }
        return maxn_t - minn_t;
    }

    void reset() {
        ptr_l_ = 0;
        ptr_r_ = 0;
        for (auto& vec : adj_) {
            vec.clear();
        }
    }

    // (1) degree > (2) id >
    inline bool priority(int lhs, int rhs) const {
        if (adj_[lhs].size() != adj_[rhs].size())
            return adj_[lhs].size() > adj_[rhs].size();
        return lhs > rhs;
    }

    // node belong which layer
    inline bool layer(int node_id) const {
        if (node_id < U_) {
            return IS_UPPER;
        } else if (node_id < U_ + L_) {
            return IS_LOWER;
        } else {
            abort();
        }
    }

    int U_, L_;
    int ptr_l_, ptr_r_;  // [ptr_l, ptr_r)

    std::vector<Edge> edge_stream_;
    std::vector<std::deque<AdjEdge>> adj_;
};
