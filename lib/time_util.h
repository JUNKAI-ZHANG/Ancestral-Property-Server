//
// Created by haowendeng on 2023/3/2.
//

#ifndef TIME_UTIL_H
#define TIME_UTIL_H

time_t getTimeMS() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
}

#endif //TIME_UTIL_H
