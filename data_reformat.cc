/**
 *@description: Reformat KONECT(http://konect.cc/) data: Temporal Bipartite Graphs
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#include <bits/stdc++.h>

int main(int argc, char* argv[]) {
    if (argc != 2)
        return EXIT_FAILURE;
    std::string file_path = argv[1];
    std::ifstream in_file(file_path);
    if (in_file.fail()) {
        std::cerr << "OPEN " << file_path << " FAILED!\n";
    }
    std::vector<std::tuple<int, int, int64_t>> edges;
    std::string line;
    int U = 0, L = 0;
    int64_t E = 0;
    int64_t maxn_t = std::numeric_limits<int64_t>::min();
    int64_t minn_t = std::numeric_limits<int64_t>::max();
    while (std::getline(in_file, line)) {
        if (line.empty() || line[0] == '%')
            continue;
        std::istringstream iss(line);
        int from, to;
        std::string _;
        int64_t time;
        iss >> from >> to >> _ >> time;
        maxn_t = std::max(maxn_t, time);
        minn_t = std::min(minn_t, time);
        U = std::max(U, from);
        L = std::max(L, to);
        E++;
        edges.emplace_back(from - 1, to - 1, time);
    }
    in_file.close();
    std::ofstream out_file(file_path);
    out_file << U << ' ' << L << '\n';
    for (auto& [from, to, time] : edges) {
        out_file << from << ' ' << to << ' ' << time << '\n';
    }
    out_file.close();

    std::cout << "|U| = " << U << ", |L| = " << L;
    std::cout << ", |V| = " << U + L << ", |E| = " << E << '\n';
    std::cout << "TimeSpan(TS) = " << maxn_t - minn_t
              << "(timestamps) or = " << 1.0 * (maxn_t - minn_t) / 86400 << "(days)\n";
}