/**
 *@description: Baseline for STBC
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "TBCCore.hpp"
#include "GraphStream.hpp"
#include "Util.hpp"

struct STBC {
    STBC(StreamGraph& g, int64_t delta) : graph_(g), delta_(delta) {
        graph_.reset();
        count_ = std::vector<int64_t>(TYPE_SIZE);
    }

    std::vector<int64_t> count_edge(const Edge& edg) {
        auto& adj = graph_.adj_;

        int start = edg.from, v = edg.to;
        if (!graph_.priority(edg.from, edg.to)) {
            start = edg.to;
            v = edg.from;
        }

        std::vector<int64_t> res(TYPE_SIZE);

        std::unordered_map<int, Set> hm_v, hm_not_v;
        auto cmp_adj = [&](const AdjEdge& lhs, const int64_t& rhs) { return lhs.time < rhs; };
        int start_st =
            std::lower_bound(adj[start].begin(), adj[start].end(), edg.time - delta_, cmp_adj) - adj[start].begin();
        for (int start_pos = start_st; start_pos < adj[start].size(); ++start_pos) {
            const auto& e1 = adj[start][start_pos];
            int middle = e1.to;
            if (std::abs(e1.time - edg.time) > delta_)
                break;
            if (middle == v || e1.time == edg.time)
                continue;
            int64_t bound_low = std::max(edg.time, e1.time) - delta_;
            int64_t bound_up = std::min(edg.time, e1.time) + delta_;
            int middle_st =
                std::lower_bound(adj[middle].begin(), adj[middle].end(), bound_low, cmp_adj) - adj[middle].begin();
            for (int middle_pos = middle_st; middle_pos < adj[middle].size(); ++middle_pos) {
                const auto& e2 = adj[middle][middle_pos];
                int end = e2.to;
                if (bound_up < e2.time)
                    break;
                if (end == start || e2.time == edg.time || e2.time == e1.time)
                    continue;
                if (!hm_not_v.count(end)) {
                    hm_not_v[end] = Set();
                }
                if (e1.time < e2.time) {
                    hm_not_v[end].first.emplace_back(e1.time, e2.time);
                } else {
                    // if (e1.time > e2.time) swap
                    hm_not_v[end].second.emplace_back(e2.time, e1.time);
                }
            }
        }
        int v_st = std::lower_bound(adj[v].begin(), adj[v].end(), edg.time - delta_, cmp_adj) - adj[v].begin();
        for (int v_pos = v_st; v_pos < adj[v].size(); ++v_pos) {
            const auto& e2 = adj[v][v_pos];
            int end = e2.to;
            if (std::abs(e2.time - edg.time) > delta_)
                break;
            if (end == start || e2.time == edg.time)
                continue;
            if (!hm_v.count(end)) {
                hm_v[end] = Set();
            }
            if (edg.time < e2.time) {
                hm_v[end].first.emplace_back(edg.time, e2.time);
            } else {
                // if (e1.time > e2.time) swap
                hm_v[end].second.emplace_back(e2.time, edg.time);
            }
        }
        for (auto& _ : hm_v) {
            int end = _.first;
            auto& bkt = _.second;
            if (!hm_not_v.count(end))
                continue;
            std::sort(bkt.first.begin(), bkt.first.end(), [&](const auto& lhs, const auto& rhs) {
                if (lhs.first != rhs.first)
                    return lhs.first > rhs.first;
                return lhs.second < rhs.second;
            });
            std::sort(bkt.second.begin(), bkt.second.end(), [&](const auto& lhs, const auto& rhs) {
                if (lhs.first != rhs.first)
                    return lhs.first > rhs.first;
                return lhs.second < rhs.second;
            });
            std::sort(hm_not_v[end].first.begin(), hm_not_v[end].first.end(), [&](const auto& lhs, const auto& rhs) {
                if (lhs.first != rhs.first)
                    return lhs.first > rhs.first;
                return lhs.second < rhs.second;
            });
            std::sort(hm_not_v[end].second.begin(), hm_not_v[end].second.end(), [&](const auto& lhs, const auto& rhs) {
                if (lhs.first != rhs.first)
                    return lhs.first > rhs.first;
                return lhs.second < rhs.second;
            });
            set_cross_rbtree(res, graph_.layer(start), delta_, bkt, hm_not_v[end]);
        }
        return res;
    }

    bool pop_front() {
        if (graph_.ptr_l_ == graph_.ptr_r_)
            return false;
        auto& edg = graph_.edge_stream_[graph_.ptr_l_];
        graph_.adj_[edg.from].pop_front();
        graph_.adj_[edg.to].pop_front();
        graph_.ptr_l_++;
        auto change = count_edge(edg);
        for (int i : range(TYPE_SIZE)) {
            count_[i] -= change[i];
        }
        return true;
    }

    bool push_back() {
        if (graph_.ptr_r_ == graph_.edge_stream_.size())
            return false;
        auto& edg = graph_.edge_stream_[graph_.ptr_r_];
        auto change = count_edge(edg);
        for (int i : range(TYPE_SIZE)) {
            count_[i] += change[i];
        }
        graph_.adj_[edg.from].push_back({edg.to, edg.time});
        graph_.adj_[edg.to].push_back({edg.from, edg.time});
        graph_.ptr_r_++;
        return true;
    }

    void shift_window(int window_size, int stride_size, bool print = false) {
        for (int i = 0; i < window_size; ++i) {
            auto flag = push_back();
            if (!flag)
                break;
        }
        while (true) {
            if (print) print_count();
            bool flag = false;
            for (int i = 0; i < stride_size; ++i) {
                auto res = pop_front();
                flag |= res;
                if (!res)
                    break;
            }
            for (int i = 0; i < stride_size; ++i) {
                auto res = push_back();
                flag |= res;
                if (!res)
                    break;
            }
            if (!flag)
                break;
        }
    }

    void print_count() { std::cout << count_ << '\n'; }

    int64_t delta_;
    std::vector<int64_t> count_;
    StreamGraph& graph_;
};