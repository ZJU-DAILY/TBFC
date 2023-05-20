/**
 *@description: Core file
 *@author: Xinwei Cai
 *@date: Apr. 2023
 */

#pragma once

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

const int TYPE_SIZE = 6;

const int BF_TYPE_0 = 0;
const int BF_TYPE_1 = 1;
const int BF_TYPE_2 = 2;
const int BF_TYPE_3 = 3;
const int BF_TYPE_4 = 4;
const int BF_TYPE_5 = 5;

const int64_t I64_MAX = std::numeric_limits<int64_t>::max();
const int64_t I64_MIN = std::numeric_limits<int64_t>::min();

using Time = std::chrono::steady_clock;