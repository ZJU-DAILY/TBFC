/**
 *@description: Data loader for static temporal bipartite graph
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Core.h"

const int IS_UPPER = 0;
const int IS_LOWER = 1;

struct AdjEdge {
    int to;
    int64_t time;
};

struct Edge {
    int from, to;
    int64_t time;
};

struct Graph {
    /*
    you can reformat konect-data after reformat,
    or use other data following the specified format below,
    where $L$ donates left_node_size, $R$ donates right_node_size,
    then following multiple lines represent
    undirected edges between node $u(0<=u<L)$ and node $v(0<=v<R)$ at time $t$:
    ```
    L R
    u_0 v_0 t_0
    u_1 v_1 t_1
    ...
    u_x v_x t_x
    ```
    */
    int64_t load(std::string file_path, bool store_all_edge = false, bool print = false) {
        std::ifstream in_file(file_path);
        if (in_file.fail()) {
            std::cerr << "Error: open " << file_path << " failed!\n";
            exit(EXIT_FAILURE);
        }
        in_file >> U_ >> L_;
        in_file.ignore();
        adj_.resize(U_ + L_);
        std::string line;
        E_ = 0;
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
            adj_[from].push_back({to, time});
            adj_[to].push_back({from, time});
            E_++;
            if (store_all_edge)
                all_edge_.push_back({from, to, time});
        }
        if (print) {
            std::cout << "|U| = " << U_ << ", |L| = " << L_;
            std::cout << ", |V| = " << U_ + L_ << ", |E| = " << E_ << '\n';
            std::cout << "TimeSpan(TS) = " << maxn_t - minn_t << "(timestamps)";
            std::cout << " or = " << 1.0 * (maxn_t - minn_t) / 60 / 60 / 24 << "(days)\n";
        }
        return maxn_t - minn_t;
    }

    void sort_by_vp() {
        for (auto& edges : adj_) {
            std::sort(edges.begin(), edges.end(),
                      [&](const AdjEdge& e1, const AdjEdge& e2) { return priority(e2.to, e1.to); });
        }
    }

    void sort_by_tp() {
        for (auto& edges : adj_) {
            std::sort(edges.begin(), edges.end(),
                      [&](const AdjEdge& e1, const AdjEdge& e2) { return e1.time < e2.time; });
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

    int U_, L_, E_;
    std::vector<Edge> all_edge_;
    std::vector<std::vector<AdjEdge>> adj_;
};
