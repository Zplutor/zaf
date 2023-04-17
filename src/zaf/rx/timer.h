#pragma once

#include <chrono>
#include <zaf/rx/observable.h>

namespace zaf::rx {

Observable<int> Timer(std::chrono::steady_clock::duration delay);

Observable<int> Timer(
    std::chrono::steady_clock::duration delay,
    std::shared_ptr<Scheduler> scheduler);

Observable<int> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval);

Observable<int> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler);

Observable<int> Interval(std::chrono::steady_clock::duration interval);

Observable<int> Interval(
    std::chrono::steady_clock::duration interval, 
    std::shared_ptr<Scheduler> scheduler);

}