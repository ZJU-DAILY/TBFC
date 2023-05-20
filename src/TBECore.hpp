/**
 *@description: Core function for enumeration
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "TBCCore.hpp"

struct Butterfly {
    int u, w, v, x;
    int64_t t_uv, t_wv, t_ux, t_wx;
    Butterfly(int _u, int _w, int _v, int _x, int _tuv, int _twv, int _tux, int _twx):
        u(_u), w(_w), v(_v), x(_x), t_uv(_tuv), t_wv(_twv), t_ux(_tux), t_wx(_twx) {}
};
using IntervalM = std::tuple<int64_t, int64_t, int>;
using BlockM = std::vector<IntervalM>;
using SetM = std::pair<BlockM, BlockM>;
using SequenceM = std::vector<SetM>;

std::ofstream& operator<<(std::ofstream& out, Butterfly& bf) {
    out << bf.u << ' ' << bf.w << ' ';
    out << bf.v << ' ' << bf.x << ' ';
    out << bf.t_uv << ' ' << bf.t_wv << ' ';
    out << bf.t_ux << ' ' << bf.t_wx << '\n';
    return out;
}

// function for TBE
void check_wedge_pair_n_enu(bool print, std::vector<int64_t>& counts, std::vector<std::ofstream>& butterflies, int64_t delta, int layer, const Wedge& wedge_i, const Wedge& wedge_j) {
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
    Butterfly bf(wedge_i.st, wedge_i.en, wedge_i.mid, wedge_j.mid, wedge_i.t1, wedge_i.t2, wedge_j.t1, wedge_j.t2);
    if (print) butterflies[type] << bf;
    counts[type]++;
}

// function for TBE+
void set_cross_lb_enu(bool print, std::vector<int64_t>& counts, std::vector<std::ofstream>& butterflies, int layer, int64_t delta, SetM& lhs, SetM& rhs, int start, int end) {
    auto query_sam = [&](const IntervalM& itv, const ListBucketM& list, bool tag1, bool tag2) {
        for (auto iter = list.begin(); iter != list.end(); iter++) {
            const auto& vec = iter->second;
            if (iter->first > std::get<1>(itv)) {
                for (int i = 0; i < vec.size(); ++i) {
                    Butterfly bf(start, end, std::get<2>(itv), std::get<1>(vec[i]), std::get<0>(itv), std::get<1>(itv), iter->first, std::get<0>(vec[i]));
                    if (tag1) std::swap(bf.t_uv, bf.t_wv);
                    if (tag2) std::swap(bf.t_ux, bf.t_wx);
                    if (print) butterflies[BF_TYPE_0 ^ layer] << bf;
                    counts[BF_TYPE_0 ^ layer]++;
                }
            } else if (iter->first < std::get<1>(itv)) {
                for (int i = (int) vec.size() - 1; i >= 0; --i) {
                    if (std::get<0>(vec[i]) <= std::get<1>(itv)) break;
                    Butterfly bf(start, end, std::get<2>(itv), std::get<1>(vec[i]), std::get<0>(itv), std::get<1>(itv), iter->first, std::get<0>(vec[i]));
                    if (tag1) std::swap(bf.t_uv, bf.t_wv);
                    if (tag2) std::swap(bf.t_ux, bf.t_wx);
                    if (print) butterflies[BF_TYPE_1 ^ layer] << bf;
                    counts[BF_TYPE_1 ^ layer]++;
                }
                for (int i = 0; i < vec.size(); ++i) {
                    if (std::get<0>(vec[i]) >= std::get<1>(itv)) break;
                    Butterfly bf(start, end, std::get<2>(itv), std::get<1>(vec[i]), std::get<0>(itv), std::get<1>(itv), iter->first, std::get<0>(vec[i]));
                    if (tag1) std::swap(bf.t_uv, bf.t_wv);
                    if (tag2) std::swap(bf.t_ux, bf.t_wx);
                    if (print) butterflies[BF_TYPE_2 ^ layer] << bf;
                    counts[BF_TYPE_2 ^ layer]++;
                }
            }
        }
    };
    auto query_dif = [&](const IntervalM& itv, const ListBucketM& list, bool tag1, bool tag2) {
        for (auto iter = list.begin(); iter != list.end(); iter++) {
            const auto& vec = iter->second;
            if (iter->first > std::get<1>(itv)) {
                for (int i = 0; i < vec.size(); ++i) {
                    Butterfly bf(start, end, std::get<2>(itv), std::get<1>(vec[i]), std::get<0>(itv), std::get<1>(itv), iter->first, std::get<0>(vec[i]));
                    if (tag1) std::swap(bf.t_uv, bf.t_wv);
                    if (tag2) std::swap(bf.t_ux, bf.t_wx);
                    if (print) butterflies[BF_TYPE_3 ^ layer] << bf;
                    counts[BF_TYPE_3 ^ layer]++;
                }
            } else if (iter->first < std::get<1>(itv)) {
                for (int i = (int) vec.size() - 1; i >= 0; --i) {
                    if (std::get<0>(vec[i]) <= std::get<1>(itv)) break;
                    Butterfly bf(start, end, std::get<2>(itv), std::get<1>(vec[i]), std::get<0>(itv), std::get<1>(itv), iter->first, std::get<0>(vec[i]));
                    if (tag1) std::swap(bf.t_uv, bf.t_wv);
                    if (tag2) std::swap(bf.t_ux, bf.t_wx);
                    if (print) butterflies[BF_TYPE_4 ^ layer] << bf;
                    counts[BF_TYPE_4 ^ layer]++;
                }
                for (int i = 0; i < vec.size(); ++i) {
                    if (std::get<0>(vec[i]) >= std::get<1>(itv)) break;
                    Butterfly bf(start, end, std::get<2>(itv), std::get<1>(vec[i]), std::get<0>(itv), std::get<1>(itv), iter->first, std::get<0>(vec[i]));
                    if (tag1) std::swap(bf.t_uv, bf.t_wv);
                    if (tag2) std::swap(bf.t_ux, bf.t_wx);
                    if (print) butterflies[BF_TYPE_5 ^ layer] << bf;
                    counts[BF_TYPE_5 ^ layer]++;
                }
            }
        }
    };
    auto& la = lhs.first;
    auto& ld = lhs.second;
    auto& ra = rhs.first;
    auto& rd = rhs.second;
    SetM bkt_tmp;
    ListBucketM list_la, list_ld, list_ra, list_rd;
    int ptr_la = 0, ptr_ld = 0, ptr_ra = 0, ptr_rd = 0;
    while (ptr_la < la.size() || ptr_ld < ld.size() || ptr_ra < ra.size() || ptr_rd < rd.size()) {
        int64_t maxn = -1;
        if (ptr_la < la.size())
            maxn = std::max(maxn, std::get<0>(la[ptr_la]));
        if (ptr_ld < ld.size())
            maxn = std::max(maxn, std::get<0>(ld[ptr_ld]));
        if (ptr_ra < ra.size())
            maxn = std::max(maxn, std::get<0>(ra[ptr_ra]));
        if (ptr_rd < rd.size())
            maxn = std::max(maxn, std::get<0>(rd[ptr_rd]));
        int pre_ptr_la = ptr_la, pre_ptr_ld = ptr_ld, pre_ptr_ra = ptr_ra, pre_ptr_rd = ptr_rd;
        // delete dissatisfy interval
        delete_itv(maxn + delta, list_la);
        delete_itv(maxn + delta, list_ld);
        delete_itv(maxn + delta, list_ra);
        delete_itv(maxn + delta, list_rd);
        // query
        while (ptr_la < la.size() && std::get<0>(la[ptr_la]) == maxn) {
            query_sam(la[ptr_la], list_ra, false, false);
            query_dif(la[ptr_la], list_rd, false, true);
            ptr_la++;
        }
        while (ptr_ld < ld.size() && std::get<0>(ld[ptr_ld]) == maxn) {
            query_sam(ld[ptr_ld], list_rd, true, true);
            query_dif(ld[ptr_ld], list_ra, true, false);
            ptr_ld++;
        }
        while (ptr_ra < ra.size() && std::get<0>(ra[ptr_ra]) == maxn) {
            query_sam(ra[ptr_ra], list_la, false, false);
            query_dif(ra[ptr_ra], list_ld, false, true);
            ptr_ra++;
        }
        while (ptr_rd < rd.size() && std::get<0>(rd[ptr_rd]) == maxn) {
            query_sam(rd[ptr_rd], list_ld, true, true);
            query_dif(rd[ptr_rd], list_la, true, false);
            ptr_rd++;
        }
        // merge & insert
        while (pre_ptr_la < ptr_la || pre_ptr_ra < ptr_ra) {
            if (pre_ptr_ra == ptr_ra ||
                (pre_ptr_ra < ptr_ra && pre_ptr_la < ptr_la && std::get<1>(la[pre_ptr_la]) <= std::get<1>(ra[pre_ptr_ra]))) {
                insert_itv(la[pre_ptr_la], list_la);
                bkt_tmp.first.push_back(la[pre_ptr_la]);
                pre_ptr_la++;
                continue;
            }
            if (pre_ptr_la == ptr_la ||
                (pre_ptr_la < ptr_la && pre_ptr_ra < ptr_ra && std::get<1>(ra[pre_ptr_ra]) <= std::get<1>(la[pre_ptr_la]))) {
                insert_itv(ra[pre_ptr_ra], list_ra);
                bkt_tmp.first.push_back(ra[pre_ptr_ra]);
                pre_ptr_ra++;
                continue;
            }
        }
        while (pre_ptr_ld < ptr_ld || pre_ptr_rd < ptr_rd) {
            if (pre_ptr_rd == ptr_rd ||
                (pre_ptr_rd < ptr_rd && pre_ptr_ld < ptr_ld && std::get<1>(ld[pre_ptr_ld]) <= std::get<1>(rd[pre_ptr_rd]))) {
                insert_itv(ld[pre_ptr_ld], list_ld);
                bkt_tmp.second.push_back(ld[pre_ptr_ld]);
                pre_ptr_ld++;
                continue;
            }
            if (pre_ptr_ld == ptr_ld ||
                (pre_ptr_ld < ptr_ld && pre_ptr_rd < ptr_rd && std::get<1>(rd[pre_ptr_rd]) <= std::get<1>(ld[pre_ptr_ld]))) {
                insert_itv(rd[pre_ptr_rd], list_rd);
                bkt_tmp.second.push_back(rd[pre_ptr_rd]);
                pre_ptr_rd++;
                continue;
            }
        }
    }
    lhs = bkt_tmp;
};