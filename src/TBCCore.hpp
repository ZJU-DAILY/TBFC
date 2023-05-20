/**
 *@description: Core function for counting
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "DataStructure.hpp"
#include "Util.hpp"

struct Wedge {
    int st, mid, en;
    int64_t t1, t2;
};
using Interval = std::pair<int64_t, int64_t>;
using Block = std::vector<Interval>;
using Set = std::pair<Block, Block>;
using Sequence = std::vector<Set>;

// function for TBC
void check_wedge_pair_n_add(std::vector<int64_t>& count, int64_t delta, int layer, const Wedge& wedge_i, const Wedge& wedge_j) {
    if (wedge_j.mid == wedge_i.mid)
        return;
    if (wedge_i.t1 == wedge_i.t2 || wedge_j.t1 == wedge_j.t2 ||
        wedge_i.t1 == wedge_j.t1 || wedge_i.t1 == wedge_j.t2 ||
        wedge_i.t2 == wedge_j.t1 || wedge_i.t2 == wedge_j.t2)
        return;
    int maxn = std::max({wedge_i.t1, wedge_i.t2, wedge_j.t1, wedge_j.t2});
    int minn = std::min({wedge_i.t1, wedge_i.t2, wedge_j.t1, wedge_j.t2});
    if (maxn - minn > delta)
        return;
    int type = [&]() {
        // layer: IS_LOWER = 0, IS_UPPER = 1
        // BF_TYPE_0 = 0; BF_TYPE_1 = 1; BF_TYPE_2 = 2; BF_TYPE_3 = 3; BF_TYPE_4 = 4; BF_TYPE_6 = 5;
        if (wedge_i.t1 < wedge_i.t2 && wedge_i.t2 < wedge_j.t1 && wedge_j.t1 < wedge_j.t2)
            return BF_TYPE_0 ^ layer;
        if (wedge_j.t1 < wedge_j.t2 && wedge_j.t2 < wedge_i.t1 && wedge_i.t1 < wedge_i.t2)
            return BF_TYPE_0 ^ layer;
        if (wedge_i.t2 < wedge_i.t1 && wedge_i.t1 < wedge_j.t2 && wedge_j.t2 < wedge_j.t1)
            return BF_TYPE_0 ^ layer;
        if (wedge_j.t2 < wedge_j.t1 && wedge_j.t1 < wedge_i.t2 && wedge_i.t2 < wedge_i.t1)
            return BF_TYPE_0 ^ layer;

        if (wedge_i.t1 < wedge_j.t1 && wedge_j.t1 < wedge_i.t2 && wedge_i.t2 < wedge_j.t2)
            return BF_TYPE_1 ^ layer;
        if (wedge_j.t1 < wedge_i.t1 && wedge_i.t1 < wedge_j.t2 && wedge_j.t2 < wedge_i.t2)
            return BF_TYPE_1 ^ layer;
        if (wedge_i.t2 < wedge_j.t2 && wedge_j.t2 < wedge_i.t1 && wedge_i.t1 < wedge_j.t1)
            return BF_TYPE_1 ^ layer;
        if (wedge_j.t2 < wedge_i.t2 && wedge_i.t2 < wedge_j.t1 && wedge_j.t1 < wedge_i.t1)
            return BF_TYPE_1 ^ layer;

        if (wedge_i.t1 < wedge_j.t1 && wedge_j.t1 < wedge_j.t2 && wedge_j.t2 < wedge_i.t2)
            return BF_TYPE_2 ^ layer;
        if (wedge_j.t1 < wedge_i.t1 && wedge_i.t1 < wedge_i.t2 && wedge_i.t2 < wedge_j.t2)
            return BF_TYPE_2 ^ layer;
        if (wedge_i.t2 < wedge_j.t2 && wedge_j.t2 < wedge_j.t1 && wedge_j.t1 < wedge_i.t1)
            return BF_TYPE_2 ^ layer;
        if (wedge_j.t2 < wedge_i.t2 && wedge_i.t2 < wedge_i.t1 && wedge_i.t1 < wedge_j.t1)
            return BF_TYPE_2 ^ layer;

        if (wedge_i.t1 < wedge_i.t2 && wedge_i.t2 < wedge_j.t2 && wedge_j.t2 < wedge_j.t1)
            return BF_TYPE_3 ^ layer;
        if (wedge_j.t1 < wedge_j.t2 && wedge_j.t2 < wedge_i.t2 && wedge_i.t2 < wedge_i.t1)
            return BF_TYPE_3 ^ layer;
        if (wedge_i.t2 < wedge_i.t1 && wedge_i.t1 < wedge_j.t1 && wedge_j.t1 < wedge_j.t2)
            return BF_TYPE_3 ^ layer;
        if (wedge_j.t2 < wedge_j.t1 && wedge_j.t1 < wedge_i.t1 && wedge_i.t1 < wedge_i.t2)
            return BF_TYPE_3 ^ layer;

        if (wedge_i.t1 < wedge_j.t2 && wedge_j.t2 < wedge_i.t2 && wedge_i.t2 < wedge_j.t1)
            return BF_TYPE_4 ^ layer;
        if (wedge_j.t1 < wedge_i.t2 && wedge_i.t2 < wedge_j.t2 && wedge_j.t2 < wedge_i.t1)
            return BF_TYPE_4 ^ layer;
        if (wedge_i.t2 < wedge_j.t1 && wedge_j.t1 < wedge_i.t1 && wedge_i.t1 < wedge_j.t2)
            return BF_TYPE_4 ^ layer;
        if (wedge_j.t2 < wedge_i.t1 && wedge_i.t1 < wedge_j.t1 && wedge_j.t1 < wedge_i.t2)
            return BF_TYPE_4 ^ layer;

        if (wedge_i.t1 < wedge_j.t2 && wedge_j.t2 < wedge_j.t1 && wedge_j.t1 < wedge_i.t2)
            return BF_TYPE_5 ^ layer;
        if (wedge_j.t1 < wedge_i.t2 && wedge_i.t2 < wedge_i.t1 && wedge_i.t1 < wedge_j.t2)
            return BF_TYPE_5 ^ layer;
        if (wedge_i.t2 < wedge_j.t1 && wedge_j.t1 < wedge_j.t2 && wedge_j.t2 < wedge_i.t1)
            return BF_TYPE_5 ^ layer;
        if (wedge_j.t2 < wedge_i.t1 && wedge_i.t1 < wedge_i.t2 && wedge_i.t2 < wedge_j.t1)
            return BF_TYPE_5 ^ layer;
        abort();
        return -1;
    }();
    count[type]++;
}

// function for TBC+
void set_cross_lb(std::vector<int64_t>& count, int layer, int64_t delta, Set& lhs, Set& rhs) {
    auto query_sam = [&](const Interval& itv, const ListBucket& list) {
        for (auto iter = list.begin(); iter != list.end(); iter++) {
            const auto& vec = iter->second;
            if (iter->first > itv.second) {
                count[BF_TYPE_0 ^ layer] += vec.size();
            } else if (iter->first < itv.second) {
                count[BF_TYPE_1 ^ layer] += vec.end() - std::upper_bound(vec.begin(), vec.end(), itv.second);
                count[BF_TYPE_2 ^ layer] += std::lower_bound(vec.begin(), vec.end(), itv.second) - vec.begin();
            }
        }
    };
    auto query_dif = [&](const Interval& itv, const ListBucket& list) {
        for (auto iter = list.begin(); iter != list.end(); iter++) {
            const auto& vec = iter->second;
            if (iter->first > itv.second) {
                count[BF_TYPE_3 ^ layer] += vec.size();
            } else if (iter->first < itv.second) {
                count[BF_TYPE_4 ^ layer] += vec.end() - std::upper_bound(vec.begin(), vec.end(), itv.second);
                count[BF_TYPE_5 ^ layer] += std::lower_bound(vec.begin(), vec.end(), itv.second) - vec.begin();
            }
        }
    };
    auto& la = lhs.first;
    auto& ld = lhs.second;
    auto& ra = rhs.first;
    auto& rd = rhs.second;
    Set bkt_tmp;
    ListBucket list_la, list_ld, list_ra, list_rd;
    int ptr_la = 0, ptr_ld = 0, ptr_ra = 0, ptr_rd = 0;
    while (ptr_la < la.size() || ptr_ld < ld.size() || ptr_ra < ra.size() || ptr_rd < rd.size()) {
        int64_t maxn = -1;
        if (ptr_la < la.size())
            maxn = std::max(maxn, la[ptr_la].first);
        if (ptr_ld < ld.size())
            maxn = std::max(maxn, ld[ptr_ld].first);
        if (ptr_ra < ra.size())
            maxn = std::max(maxn, ra[ptr_ra].first);
        if (ptr_rd < rd.size())
            maxn = std::max(maxn, rd[ptr_rd].first);
        int pre_ptr_la = ptr_la, pre_ptr_ld = ptr_ld, pre_ptr_ra = ptr_ra, pre_ptr_rd = ptr_rd;
        // delete dissatisfy interval
        delete_itv(maxn + delta, list_la);
        delete_itv(maxn + delta, list_ld);
        delete_itv(maxn + delta, list_ra);
        delete_itv(maxn + delta, list_rd);
        // query
        while (ptr_la < la.size() && la[ptr_la].first == maxn) {
            query_sam(la[ptr_la], list_ra);
            query_dif(la[ptr_la], list_rd);
            ptr_la++;
        }
        while (ptr_ld < ld.size() && ld[ptr_ld].first == maxn) {
            query_sam(ld[ptr_ld], list_rd);
            query_dif(ld[ptr_ld], list_ra);
            ptr_ld++;
        }
        while (ptr_ra < ra.size() && ra[ptr_ra].first == maxn) {
            query_sam(ra[ptr_ra], list_la);
            query_dif(ra[ptr_ra], list_ld);
            ptr_ra++;
        }
        while (ptr_rd < rd.size() && rd[ptr_rd].first == maxn) {
            query_sam(rd[ptr_rd], list_ld);
            query_dif(rd[ptr_rd], list_la);
            ptr_rd++;
        }
        // merge & insert
        while (pre_ptr_la < ptr_la || pre_ptr_ra < ptr_ra) {
            if (pre_ptr_ra == ptr_ra ||
                (pre_ptr_ra < ptr_ra && pre_ptr_la < ptr_la && la[pre_ptr_la].second <= ra[pre_ptr_ra].second)) {
                insert_itv(la[pre_ptr_la], list_la);
                bkt_tmp.first.push_back(la[pre_ptr_la]);
                pre_ptr_la++;
                continue;
            }
            if (pre_ptr_la == ptr_la ||
                (pre_ptr_la < ptr_la && pre_ptr_ra < ptr_ra && ra[pre_ptr_ra].second <= la[pre_ptr_la].second)) {
                insert_itv(ra[pre_ptr_ra], list_ra);
                bkt_tmp.first.push_back(ra[pre_ptr_ra]);
                pre_ptr_ra++;
                continue;
            }
        }
        while (pre_ptr_ld < ptr_ld || pre_ptr_rd < ptr_rd) {
            if (pre_ptr_rd == ptr_rd ||
                (pre_ptr_rd < ptr_rd && pre_ptr_ld < ptr_ld && ld[pre_ptr_ld].second <= rd[pre_ptr_rd].second)) {
                insert_itv(ld[pre_ptr_ld], list_ld);
                bkt_tmp.second.push_back(ld[pre_ptr_ld]);
                pre_ptr_ld++;
                continue;
            }
            if (pre_ptr_ld == ptr_ld ||
                (pre_ptr_ld < ptr_ld && pre_ptr_rd < ptr_rd && rd[pre_ptr_rd].second <= ld[pre_ptr_ld].second)) {
                insert_itv(rd[pre_ptr_rd], list_rd);
                bkt_tmp.second.push_back(rd[pre_ptr_rd]);
                pre_ptr_rd++;
                continue;
            }
        }
    }
    lhs = bkt_tmp;
};

// function for TBC++
void set_cross_rbtree(std::vector<int64_t>& count, int layer, int64_t delta, Set& lhs, Set& rhs) {
    auto query_sam = [&](const Interval& itv, RBTree_G& mst_g, RBTree_S& mst_s) {
        count[BF_TYPE_0 ^ layer] += count_greater(itv.second, mst_s);
        count[BF_TYPE_1 ^ layer] += count_greater(itv.second, mst_g) - count_ge(itv.second, mst_s);
        count[BF_TYPE_2 ^ layer] += count_less(itv.second, mst_g);
    };
    auto query_dif = [&](const Interval& itv, RBTree_G& mst_g, RBTree_S& mst_s) {
        count[BF_TYPE_3 ^ layer] += count_greater(itv.second, mst_s);
        count[BF_TYPE_4 ^ layer] += count_greater(itv.second, mst_g) - count_ge(itv.second, mst_s);
        count[BF_TYPE_5 ^ layer] += count_less(itv.second, mst_g);
    };
    auto& la = lhs.first;
    auto& ld = lhs.second;
    auto& ra = rhs.first;
    auto& rd = rhs.second;
    Set bkt_tmp;
    RBTree_G mst_g_la, mst_g_ld, mst_g_ra, mst_g_rd;
    RBTree_S mst_s_la, mst_s_ld, mst_s_ra, mst_s_rd;
    int ptr_la = 0, ptr_ld = 0, ptr_ra = 0, ptr_rd = 0;
    while (ptr_la < la.size() || ptr_ld < ld.size() || ptr_ra < ra.size() || ptr_rd < rd.size()) {
        int64_t maxn = -1;
        if (ptr_la < la.size())
            maxn = std::max(maxn, la[ptr_la].first);
        if (ptr_ld < ld.size())
            maxn = std::max(maxn, ld[ptr_ld].first);
        if (ptr_ra < ra.size())
            maxn = std::max(maxn, ra[ptr_ra].first);
        if (ptr_rd < rd.size())
            maxn = std::max(maxn, rd[ptr_rd].first);
        int pre_ptr_la = ptr_la, pre_ptr_ld = ptr_ld, pre_ptr_ra = ptr_ra, pre_ptr_rd = ptr_rd;
        // delete dissatisfy interval
        delete_itv(maxn + delta, mst_g_la, mst_s_la);
        delete_itv(maxn + delta, mst_g_ld, mst_s_ld);
        delete_itv(maxn + delta, mst_g_ra, mst_s_ra);
        delete_itv(maxn + delta, mst_g_rd, mst_s_rd);
        // query
        while (ptr_la < la.size() && la[ptr_la].first == maxn) {
            query_sam(la[ptr_la], mst_g_ra, mst_s_ra);
            query_dif(la[ptr_la], mst_g_rd, mst_s_rd);
            ptr_la++;
        }
        while (ptr_ld < ld.size() && ld[ptr_ld].first == maxn) {
            query_sam(ld[ptr_ld], mst_g_rd, mst_s_rd);
            query_dif(ld[ptr_ld], mst_g_ra, mst_s_ra);
            ptr_ld++;
        }
        while (ptr_ra < ra.size() && ra[ptr_ra].first == maxn) {
            query_sam(ra[ptr_ra], mst_g_la, mst_s_la);
            query_dif(ra[ptr_ra], mst_g_ld, mst_s_ld);
            ptr_ra++;
        }
        while (ptr_rd < rd.size() && rd[ptr_rd].first == maxn) {
            query_sam(rd[ptr_rd], mst_g_ld, mst_s_ld);
            query_dif(rd[ptr_rd], mst_g_la, mst_s_la);
            ptr_rd++;
        }
        // insert interval & merge bucket
        for (int i = pre_ptr_la; i < ptr_la; ++i) {
            insert_itv(la[i], i, mst_g_la, mst_s_la);
            bkt_tmp.first.push_back(la[i]);
        }
        for (int i = pre_ptr_ld; i < ptr_ld; ++i) {
            insert_itv(ld[i], i, mst_g_ld, mst_s_ld);
            bkt_tmp.second.push_back(ld[i]);
        }
        for (int i = pre_ptr_ra; i < ptr_ra; ++i) {
            insert_itv(ra[i], i, mst_g_ra, mst_s_ra);
            bkt_tmp.first.push_back(ra[i]);
        }
        for (int i = pre_ptr_rd; i < ptr_rd; ++i) {
            insert_itv(rd[i], i, mst_g_rd, mst_s_rd);
            bkt_tmp.second.push_back(rd[i]);
        }
    }
    lhs = bkt_tmp;
}

//
// void check_interval_pair_n_add_1(std::vector<int64_t>& count, int64_t delta, int layer, const Interval& itv_i, const Interval& itv_j) {
//     if (itv_i.first == itv_j.first || itv_i.first == itv_j.second ||
//         itv_i.second == itv_j.first || itv_i.second == itv_j.second)
//         return;
//     int maxn = std::max(itv_i.second, itv_j.second);
//     int minn = std::min(itv_i.first, itv_j.first);
//     if (maxn - minn > delta)
//         return;
//     int type = [&]() {
//         // layer: IS_LOWER = 0, IS_UPPER = 1
//         // BF_TYPE_0 = 0; BF_TYPE_1 = 1; BF_TYPE_2 = 2; BF_TYPE_3 = 3; BF_TYPE_4 = 4; BF_TYPE_6 = 5;
//         if (itv_i.first < itv_i.second && itv_i.second < itv_j.first && itv_j.first < itv_j.second)
//             return BF_TYPE_0 ^ layer;
//         if (itv_j.first < itv_j.second && itv_j.second < itv_i.first && itv_i.first < itv_i.second)
//             return BF_TYPE_0 ^ layer;

//         if (itv_i.first < itv_j.first && itv_j.first < itv_i.second && itv_i.second < itv_j.second)
//             return BF_TYPE_1 ^ layer;
//         if (itv_j.first < itv_i.first && itv_i.first < itv_j.second && itv_j.second < itv_i.second)
//             return BF_TYPE_1 ^ layer;

//         if (itv_i.first < itv_j.first && itv_j.first < itv_j.second && itv_j.second < itv_i.second)
//             return BF_TYPE_2 ^ layer;
//         if (itv_j.first < itv_i.first && itv_i.first < itv_i.second && itv_i.second < itv_j.second)
//             return BF_TYPE_2 ^ layer;

//         abort();
//         return -1;
//     }();
//     count[type]++;
// }

//
// void check_interval_pair_n_add_2(std::vector<int64_t>& count, int64_t delta, int layer, const Interval& itv_i, const Interval& itv_j) {
//     if (itv_i.first == itv_j.first || itv_i.first == itv_j.second ||
//         itv_i.second == itv_j.first || itv_i.second == itv_j.second)
//         return;
//     int maxn = std::max(itv_i.second, itv_j.second);
//     int minn = std::min(itv_i.first, itv_j.first);
//     if (maxn - minn > delta)
//         return;
//     int type = [&]() {
//         // layer: IS_LOWER = 0, IS_UPPER = 1
//         // BF_TYPE_0 = 0; BF_TYPE_1 = 1; BF_TYPE_2 = 2; BF_TYPE_3 = 3; BF_TYPE_4 = 4; BF_TYPE_6 = 5;
//         if (itv_i.first < itv_i.second && itv_i.second < itv_j.first && itv_j.first < itv_j.second)
//             return BF_TYPE_3 ^ layer;
//         if (itv_j.first < itv_j.second && itv_j.second < itv_i.first && itv_i.first < itv_i.second)
//             return BF_TYPE_3 ^ layer;

//         if (itv_i.first < itv_j.first && itv_j.first < itv_i.second && itv_i.second < itv_j.second)
//             return BF_TYPE_4 ^ layer;
//         if (itv_j.first < itv_i.first && itv_i.first < itv_j.second && itv_j.second < itv_i.second)
//             return BF_TYPE_4 ^ layer;

//         if (itv_i.first < itv_j.first && itv_j.first < itv_j.second && itv_j.second < itv_i.second)
//             return BF_TYPE_5 ^ layer;
//         if (itv_j.first < itv_i.first && itv_i.first < itv_i.second && itv_i.second < itv_j.second)
//             return BF_TYPE_5 ^ layer;

//         abort();
//         return -1;
//     }();
//     count[type]++;
// }