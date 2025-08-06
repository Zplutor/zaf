#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/application.h>

namespace zaf::rx::internal {

RxRuntime& RxRuntime::GetInstance() {
    return Application::Instance().GetRxRuntime();
}


RxRuntime::RxRuntime() {

    main_thread_scheduler_.reset(new rx::MainThreadScheduler{ thread_manager_.GetMainThread() });
}

}