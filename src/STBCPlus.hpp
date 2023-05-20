/**
 *@description: Optimization algorithm for STBC
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "GraphStream.hpp"
#include "Util.hpp"

#include <omp.h>

struct STBCPlus {
    STBCPlus(StreamGraph& g, int64_t delta, int ts) : graph_(g), delta_(delta), thread_size_(ts) {
        graph_.reset();
        omp_set_num_threads(thread_size_);
        count_ = std::vector<int64_t>(TYPE_SIZE);
    }

    using TimeVec = std::vector<int64_t>;
    using IntervalVec = std::pair<TimeVec, TimeVec>;
    using Set = std::pair<IntervalVec, IntervalVec>;

    int count_less(const TimeVec& vec, int64_t x) { return std::lower_bound(vec.begin(), vec.end(), x) - vec.begin(); }

    int count_greater(const TimeVec& vec, int64_t x) { return vec.end() - std::upper_bound(vec.begin(), vec.end(), x); }

    int count_ge(const TimeVec& vec, int64_t x) { return vec.end() - std::lower_bound(vec.begin(), vec.end(), x); }

    int count_le(const TimeVec& vec, int64_t x) { return std::upper_bound(vec.begin(), vec.end(), x) - vec.begin(); }

    std::vector<int64_t> count_front_edge(const Edge& edg) {
        auto& adj = graph_.adj_;

        int start = edg.from, v = edg.to;
        if (!graph_.priority(edg.from, edg.to)) {
            start = edg.to;
            v = edg.from;
        }

        std::vector<int64_t> res(TYPE_SIZE);
        std::unordered_map<int, Set> hash_map;
        auto cmp_adj = [&](const AdjEdge& lhs, const int64_t& rhs) { return lhs.time < rhs; };
        int start_st =
            std::lower_bound(adj[start].begin(), adj[start].end(), edg.time + 1, cmp_adj) - adj[start].begin();
        for (int start_pos = start_st; start_pos < adj[start].size(); ++start_pos) {
            const auto& e1 = adj[start][start_pos];
            int middle = e1.to;
            if (e1.time > edg.time + delta_) break;
            if (middle == v) continue;
            int middle_st =
                std::lower_bound(adj[middle].begin(), adj[middle].end(), edg.time + 1, cmp_adj) - adj[middle].begin();
            for (int middle_pos = middle_st; middle_pos < adj[middle].size(); ++middle_pos) {
                const auto& e2 = adj[middle][middle_pos];
                int end = e2.to;
                if (e2.time > edg.time + delta_) break;
                if (end == start || e2.time == e1.time) continue;
                if (!hash_map.count(end)) {
                    hash_map[end] = {};
                }
                if (e1.time < e2.time) {
                    hash_map[end].first.first.push_back(e1.time);
                    hash_map[end].first.second.push_back(e2.time);
                } else {
                    // if (e1.time > e2.time) swap
                    hash_map[end].second.first.push_back(e2.time);
                    hash_map[end].second.second.push_back(e1.time);
                }
            }
        }
        int layer = graph_.layer(start);
        std::unordered_map<int, bool> sorted;
        int v_st = std::lower_bound(adj[v].begin(), adj[v].end(), edg.time + 1, cmp_adj) - adj[v].begin();
        for (int v_pos = v_st; v_pos < adj[v].size(); ++v_pos) {
            const auto& e2 = adj[v][v_pos];
            int end = e2.to;
            if (e2.time > edg.time + delta_) break;
            if (end == start) continue;
            if (!hash_map.count(end))
                continue;
            if (!sorted.count(end)) {
                auto sort = [&](std::vector<int64_t>& vec) { std::sort(vec.begin(), vec.end()); };
                sort(hash_map[end].first.first);
                sort(hash_map[end].first.second);
                sort(hash_map[end].second.first);
                sort(hash_map[end].second.second);
                sorted[end] = true;
            }
            auto& aa = hash_map[end].first;
            res[BF_TYPE_0 ^ layer] += count_greater(aa.first, e2.time);
            res[BF_TYPE_1 ^ layer] += count_greater(aa.second, e2.time) - count_ge(aa.first, e2.time);
            res[BF_TYPE_2 ^ layer] += count_less(aa.second, e2.time);
            auto& dd = hash_map[end].second;
            res[BF_TYPE_3 ^ layer] += count_greater(dd.first, e2.time);
            res[BF_TYPE_4 ^ layer] += count_greater(dd.second, e2.time) - count_ge(dd.first, e2.time);
            res[BF_TYPE_5 ^ layer] += count_less(dd.second, e2.time);
        }
        return res;
    }

    bool pop_front(int cnt = 1) {
        if (graph_.ptr_l_ == graph_.ptr_r_)
            return false;
        cnt = std::min(cnt, graph_.ptr_r_ - graph_.ptr_l_);
        if (thread_size_ > 1) {
            int64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0;
            #pragma omp parallel for reduction(+:sum0,sum1,sum2,sum3,sum4,sum5)
            for (int i = 0; i < cnt; ++i) {
                auto& edg = graph_.edge_stream_[graph_.ptr_l_ + i];
                auto change = count_front_edge(edg);
                sum0 += change[0]; sum1 += change[1]; sum2 += change[2];
                sum3 += change[3]; sum4 += change[4]; sum5 += change[5];
            }
            count_[0] -= sum0; count_[1] -= sum1; count_[2] -= sum2;
            count_[3] -= sum3; count_[4] -= sum4; count_[5] -= sum5;
        } else {
            for (int i = 0; i < cnt; ++i) {
                auto& edg = graph_.edge_stream_[graph_.ptr_l_ + i];
                auto change = count_front_edge(edg);
                count_[0] -= change[0]; count_[1] -= change[1]; count_[2] -= change[2];
                count_[3] -= change[3]; count_[4] -= change[4]; count_[5] -= change[5];
            }
        }
        for (int i = 0; i < cnt; ++i) {
            auto& edg = graph_.edge_stream_[graph_.ptr_l_ + i];
            graph_.adj_[edg.from].pop_front();
            graph_.adj_[edg.to].pop_front();
        }
        graph_.ptr_l_  += cnt;
        return true;
    }

    std::vector<int64_t> count_back_edge(const Edge& edg) {
        auto& adj = graph_.adj_;

        int start = edg.from, v = edg.to;
        if (!graph_.priority(edg.from, edg.to)) {
            start = edg.to;
            v = edg.from;
        }

        std::vector<int64_t> res(TYPE_SIZE);
        std::unordered_map<int, Set> hash_map;
        auto cmp_adj = [&](const AdjEdge& lhs, const int64_t& rhs) { return lhs.time < rhs; };
        int start_st =
            std::lower_bound(adj[start].begin(), adj[start].end(), edg.time - delta_, cmp_adj) - adj[start].begin();
        for (int start_pos = start_st; start_pos < adj[start].size(); ++start_pos) {
            const auto& e1 = adj[start][start_pos];
            int middle = e1.to;
            if (e1.time >= edg.time) break;
            if (middle == v) continue;
            int middle_st =
                std::lower_bound(adj[middle].begin(), adj[middle].end(), edg.time - delta_, cmp_adj) - adj[middle].begin();
            for (int middle_pos = middle_st; middle_pos < adj[middle].size(); ++middle_pos) {
                const auto& e2 = adj[middle][middle_pos];
                int end = e2.to;
                if (e2.time >= edg.time) break;
                if (end == start || e2.time == e1.time) continue;
                if (!hash_map.count(end)) {
                    hash_map[end] = {};
                }
                if (e1.time < e2.time) {
                    hash_map[end].first.first.push_back(e1.time);
                    hash_map[end].first.second.push_back(e2.time);
                } else {
                    // if (e1.time > e2.time) swap
                    hash_map[end].second.first.push_back(e2.time);
                    hash_map[end].second.second.push_back(e1.time);
                }
            }
        }
        int layer = graph_.layer(start);
        std::unordered_map<int, bool> sorted;
        int v_st = std::lower_bound(adj[v].begin(), adj[v].end(), edg.time - delta_, cmp_adj) - adj[v].begin();
        for (int v_pos = v_st; v_pos < adj[v].size(); ++v_pos) {
            const auto& e2 = adj[v][v_pos];
            int end = e2.to;
            if (e2.time >= edg.time) break;
            if (end == start) continue;
            if (!hash_map.count(end))
                continue;
            if (!sorted.count(end)) {
                auto sort = [&](std::vector<int64_t>& vec) { std::sort(vec.begin(), vec.end()); };
                sort(hash_map[end].first.first);
                sort(hash_map[end].first.second);
                sort(hash_map[end].second.first);
                sort(hash_map[end].second.second);
                sorted[end] = true;
            }
            auto& aa = hash_map[end].first;
            res[BF_TYPE_3 ^ layer] += count_less(aa.second, e2.time);
            res[BF_TYPE_4 ^ layer] += count_less(aa.first, e2.time) - count_le(aa.second, e2.time);
            res[BF_TYPE_5 ^ layer] += count_greater(aa.first, e2.time);
            auto& dd = hash_map[end].second;
            res[BF_TYPE_0 ^ layer] += count_less(dd.second, e2.time);
            res[BF_TYPE_1 ^ layer] += count_less(dd.first, e2.time) - count_le(dd.second, e2.time);
            res[BF_TYPE_2 ^ layer] += count_greater(dd.first, e2.time);
        }
        return res;
    }

    bool push_back(int cnt = 1) {
        if (graph_.ptr_r_ == graph_.edge_stream_.size())
            return false;
        cnt = std::min(cnt, (int)graph_.edge_stream_.size() - graph_.ptr_r_);
        for (int i = 0; i < cnt; ++i) {
            auto& edg = graph_.edge_stream_[graph_.ptr_r_ + i];
            graph_.adj_[edg.from].push_back({edg.to, edg.time});
            graph_.adj_[edg.to].push_back({edg.from, edg.time});
        }
        if (thread_size_ > 1) {
            int64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0;
            #pragma omp parallel for reduction(+:sum0,sum1,sum2,sum3,sum4,sum5)
            for (int i = 0; i < cnt; ++i) {
                auto& edg = graph_.edge_stream_[graph_.ptr_r_ + i];
                auto change = count_back_edge(edg);
                sum0 += change[0]; sum1 += change[1]; sum2 += change[2];
                sum3 += change[3]; sum4 += change[4]; sum5 += change[5];
            }
            count_[0] += sum0; count_[1] += sum1; count_[2] += sum2;
            count_[3] += sum3; count_[4] += sum4; count_[5] += sum5;
        } else {
            for (int i = 0; i < cnt; ++i) {
                auto& edg = graph_.edge_stream_[graph_.ptr_r_ + i];
                auto change = count_back_edge(edg);
                count_[0] += change[0]; count_[1] += change[1]; count_[2] += change[2];
                count_[3] += change[3]; count_[4] += change[4]; count_[5] += change[5];
            }
        }
        graph_.ptr_r_ += cnt;
        return true;
    }

    void shift_window(int window_size, int stride_size, bool print = false) {
        push_back(window_size);
        while (true) {
            if (print) print_count();
            bool flag = false;
            flag |= pop_front(stride_size);
            flag |= push_back(stride_size);
            if (!flag)
                break;
        }
    }

    void print_count() { std::cout << count_ << '\n'; }

    int thread_size_;
    int64_t delta_;
    std::vector<int64_t> count_;
    StreamGraph& graph_;
};