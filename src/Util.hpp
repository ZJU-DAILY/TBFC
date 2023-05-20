/**
 *@description: Custom utility
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include "Core.h"

struct Iterator {
    int iter;
    friend int operator*(const Iterator& self) { return self.iter; }
    friend Iterator& operator++(Iterator& self) { return self.iter++, self; }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        return lhs.iter != rhs.iter;
    }
};
struct Range {
    Iterator begin() { return {0}; }
    Iterator end() { return {end_}; }
    int end_{};
};
Range range(int end) {
    return {end};
}

template <class T>
void unique(std::vector<T>& v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
}

template <class T>
std::ostream& operator<<(std::ostream& out, std::vector<T>& vec) {
    out << vec.size();
    for (int i = 0; i < vec.size(); ++i) {
        out << ' ' << vec[i];
    }
    return out;
}

template <class T>
std::istream& operator>>(std::istream& in, std::vector<T>& vec) {
    int sz;
    in >> sz;
    vec.resize(sz);
    for (auto& e : vec)
        in >> e;
    return in;
}

int64_t time_cost(const Time::time_point& st, const Time::time_point& en) {
    int64_t cost = std::chrono::duration_cast<std::chrono::milliseconds>(en - st).count();
    std::cout << "time cost: " << cost << "(ms)\n";
    return cost;
}

// auto mem = get_mem(getpid());
int get_mem(int p) {
    FILE* fd;
    int vmrss;
    char file[64] = {0};
    char line_buff[256] = {0};
    char rdun1[32];
    char rdun2[32];

    sprintf(file, "/proc/%d/statm", p);
    fd = fopen(file, "r");
    char* ret = fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d %s", rdun1, &vmrss, rdun2);
    fclose(fd);

    return vmrss;
}

double_t mem_cost(int mem_st, int mem_en) {
    double cost = 1. * (mem_en - mem_st) * 4 / 1024;
    std::cout << "mem cost: " << cost << "(MB)\n";
    return cost;
}