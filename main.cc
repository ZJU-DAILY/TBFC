/**
 *@description: Main file
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#include "TBC.hpp"
#include "TBCPlus.hpp"
#include "STBC.hpp"
#include "STBCPlus.hpp"
#include "TBE.hpp"
#include "TBEPlus.hpp"
#include "MC.hpp"
/*
argv[0] = ./main
argv[1] = data_path
argv[2] = delta
argv[3] = algorithm :
         -10 : TBE (not print instance, for experiments)
         -11 : TBE (print instance)
         -20 : TBE$^+$ (not print instance, for experiments)
         -21 : TBE$^+$ (print instance)
          1 : TBC
          2 : TBC$^+$
          3 : TBC$^{++}$
          4 : STBC
          5 : STBC$^+$

(optional)
argv[4] = |Window| for algorithm 3,4 (default: 10000)
argv[5] = |Stride|/|window| for algorithm 3,4 (default: 0.05)
argv[6] = |Thread| for algorithm 4 (default: 8)
*/

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 7) {
        std::cerr << "Error: wrong args!\n";
        exit(EXIT_FAILURE);
    }
    std::string data_path = argv[1];
    int64_t delta = std::stoll(argv[2]);
    int algo_id = std::stoi(argv[3]);
    int window_size = 10000;
    if (argc >= 5) {
        window_size = std::stoi(argv[4]);
    }
    int stride_size = 0.05 * window_size;
    if (argc >= 6) {
        stride_size = std::stod(argv[5]) * window_size;
    }
    int thread_size = 8;
    if (argc == 7) {
        thread_size = std::stoi(argv[6]);
    }
    auto run_tbe_noprint = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBE tbe(g);
        auto ti_st = Time::now();
        auto res = tbe(delta);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_tbe = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBE tbe(g, true);
        auto ti_st = Time::now();
        auto res = tbe(delta);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_tbep_noprint = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBEPlus tbe(g);
        auto ti_st = Time::now();
        auto res = tbe(delta);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_tbep = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBEPlus tbe(g, true);
        auto ti_st = Time::now();
        auto res = tbe(delta);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_tbc = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBC tbc(g);
        auto ti_st = Time::now();
        auto res = tbc(delta);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_tbcp = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBCPlus tbcp(g);
        auto ti_st = Time::now();
        auto res = tbcp(delta, 0);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_tbcpp = [&]() {
        Graph g;
        int64_t TS = g.load(data_path);
        TBCPlus tbcpp(g);
        auto ti_st = Time::now();
        auto res = tbcpp(delta, 1);
        auto ti_en = Time::now();
        std::cout << res << '\n';
        time_cost(ti_st, ti_en);
    };
    auto run_stbc = [&]() {
        StreamGraph sg;
        int64_t TS = sg.load(data_path);
        STBC stbc(sg, delta);
        auto ti_st = Time::now();
        stbc.shift_window(window_size, stride_size);
        auto ti_en = Time::now();
        time_cost(ti_st, ti_en);
    };
    auto run_stbcp = [&]() {
        StreamGraph sg;
        int64_t TS = sg.load(data_path);
        STBCPlus stbcp(sg, delta, thread_size);
        auto ti_st = Time::now();
        stbcp.shift_window(window_size, stride_size);
        auto ti_en = Time::now();
        time_cost(ti_st, ti_en);
    };

    switch (algo_id) {
    case -10 : run_tbe_noprint(); break;
    case -11 : run_tbe(); break;
    case -20 : run_tbep_noprint(); break;
    case -21 : run_tbep(); break;
    case 1 : run_tbc(); break;
    case 2 : run_tbcp(); break;
    case 3 : run_tbcpp(); break;
    case 4 : run_stbc(); break;
    case 5 : run_stbcp(); break;
    default:
        std::cerr << "Error: wrong algorithm!\n";
        break;
    }
}