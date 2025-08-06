#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/application.h>

namespace zaf::rx::internal {

RxRuntime& RxRuntime::GetInstance() {
    return Application::Instance().GetRxRuntime();
}


RxRuntime::RxRuntime() {

    main_thread_scheduler_.reset(new rx::MainThreadScheduler{ thread_manager_.GetMainThread() });
}


TimerManager& RxRuntime::GetTimerManager() {

    std::call_once(timer_manager_once_flag_, [this]() {
        timer_manager_ = std::make_unique<TimerManager>();
    });

    return *timer_manager_;
}

}