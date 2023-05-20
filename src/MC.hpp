/**
 *@description: Comparison algorithm based on motif counting, which can't finish within a limited time on the dataset specified in the paper
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "GraphStream.hpp"
#include "Util.hpp"

struct MC {
    MC(StreamGraph& g):graph_(g){
        graph_.reset();
    }

    std::vector<int64_t> operator()(int64_t delta) {
        auto& edge_stream = graph_.edge_stream_;
        auto& ptr_l = graph_.ptr_l_, &ptr_r = graph_.ptr_r_;
        using iedge = std::pair<int, int>;

        std::vector<int64_t> res(6, 0);

        std::map<int, std::map<std::vector<iedge>, int64_t>> counts0;
        auto insert0 = [&](const std::vector<iedge> &edges) {
            for (auto &edg : edges) {
                for (auto &item : counts0[3]) {
                    auto &vec = item.first;
                    if (vec[0].first == vec[2].first && vec[0].second == vec[1].second && vec[1].first == edg.first && vec[2].second == edg.second &&
                        vec[0].first != vec[1].first && vec[0].second != vec[2].second) res[0] += item.second;
                    if (vec[0].first == vec[1].first && vec[0].second == vec[2].second && vec[2].first == edg.first && vec[1].second == edg.second &&
                        vec[0].first != vec[2].first && vec[0].second != vec[1].second) res[1] += item.second;
                    if (vec[0].first == vec[1].first && vec[0].second == edg.second && vec[2].first == edg.first && vec[2].second == vec[1].second &&
                        vec[0].first != edg.first && vec[0].second != vec[1].second) res[2] += item.second;
                    if (vec[0].first == edg.first && vec[0].second == vec[1].second && vec[2].first == vec[1].first && vec[2].second == edg.second &&
                        vec[0].first != vec[1].first && vec[0].second != edg.second) res[3] += item.second;
                    if (vec[0].first == edg.first && vec[0].second == vec[2].second && vec[1].first == vec[2].first && vec[1].second == edg.second &&
                        vec[0].first != vec[2].first && vec[0].second != edg.second) res[4] += item.second;
                    if (vec[0].first == vec[2].first && vec[0].second == edg.second && vec[1].first == edg.first && vec[1].second == vec[2].second &&
                        vec[0].first != edg.first && vec[0].second != vec[2].second) res[5] += item.second;
                }
            }
            for (auto &edg : edges) {
                for (auto &item : counts0[2]) {
                    if (edg.first == item.first[0].first && edg.second == item.first[0].second) continue;
                    if (edg.first == item.first[1].first && edg.second == item.first[1].second) continue;
                    if (edg.first == item.first[0].first && edg.first == item.first[1].first) continue;
                    if (edg.first != item.first[0].first && edg.first != item.first[1].first && item.first[0].first != item.first[1].first) continue;
                    if (edg.second == item.first[0].second && edg.second == item.first[1].second) continue;
                    if (edg.second != item.first[0].second && edg.second != item.first[1].second && item.first[0].second != item.first[1].second) continue;
                    auto vec = item.first;
                    vec.push_back(edg);
                    counts0[3][vec] += item.second;
                }
            }
            for (auto &edg : edges) {
                for (auto &item : counts0[1]) {
                    if (edg != item.first[0]) {
                        auto vec = item.first;
                        vec.push_back(edg);
                        counts0[2][vec] += item.second;
                    }
                }
            }
            for (auto &edg : edges) {
                std::vector<iedge> tmp = {edg};
                if (counts0[1].count(tmp)) {
                    counts0[1][tmp]++;
                } else {
                    counts0[1][tmp] = 1;
                }
            }
        };
        auto delete0 = [&](const std::vector<iedge> &edges) {
            for (auto &edg : edges) {
                counts0[1][{edg}]--;
            }
            for (auto &edg : edges) {
                auto start_iter = counts0[2].lower_bound({edg});
                for (auto iter = start_iter; iter != counts0[2].end(); ++iter) {
                    if (iter->first[0] != edg) {
                        break;
                    }
                    std::vector<iedge> suffix(iter->first.begin() + 1, iter->first.end());
                    // for (auto &e : suffix) {
                    //     std::cout << e.first << ',' << e.second << ' ';
                    // }std::cout << '\n';
                    if (counts0[1].count(suffix)) {
                        // std::cout << 233 << '\n';
                        iter->second -= counts0[1][suffix];
                    }
                }
            }
            for (auto &edg : edges) {
                auto start_iter = counts0[3].lower_bound({edg});
                for (auto iter = start_iter; iter != counts0[3].end(); ++iter) {
                    if (iter->first[0] != edg) {
                        break;
                    }
                    std::vector<iedge> suffix(iter->first.begin() + 1, iter->first.end());
                    if (counts0[2].count(suffix)) {
                        // std::cout << 233 << '\n';
                        iter->second -= counts0[2][suffix];
                    }
                }
            }
        };
        while (ptr_r < edge_stream.size()) {
            std::vector<iedge> inserts;
            std::vector<std::vector<iedge>> deletes;
            int64_t pre_time = -1;
            while (ptr_l < ptr_r && edge_stream[ptr_l].time + delta < edge_stream[ptr_r].time) {
                if (edge_stream[ptr_l].time != pre_time) {
                    deletes.emplace_back();
                    pre_time = edge_stream[ptr_l].time;
                }
                deletes.back().push_back({edge_stream[ptr_l].from, edge_stream[ptr_l].to});
                ptr_l++;
            }
            for (auto &v : deletes) {
                delete0(v);
            }
            int pre_ptr_r = ptr_r;
            while (ptr_r < edge_stream.size() && edge_stream[ptr_r].time == edge_stream[pre_ptr_r].time) {
                inserts.push_back({edge_stream[ptr_r].from, edge_stream[ptr_r].to});
                ptr_r++;
            }
            insert0(inserts);
            // std::cout << "insert: ";
            // for (auto e : inserts) {
            //     std::cout << '(' << e.first << ',' << e.second << ')';
            // }
            // std::cout << '\n';
            // std::cout << "delete: ";
            // for (auto v : deletes) {
            //     for (auto e : v) {
            //         std::cout << '(' << e.first << ',' << e.second << ')';
            //     }
            //     std::cout << ' ';
            // }
            // std::cout << '\n';
        }
        return res;
    }

    StreamGraph &graph_;
};
