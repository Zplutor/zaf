#pragma once

#include <atomic>
#include <memory>

namespace zaf {
namespace internal {

class CancelData {
public:
    std::atomic<bool> is_cancelled{ false };
};

}


class CancelToken {
public:
    CancelToken(const std::shared_ptr<internal::CancelData>& cancel_data) :
        cancel_data_(cancel_data) {

    }

    bool IsCancelled() const {

        if (cancel_data_) {
            return cancel_data_->is_cancelled.load();
        }
        return false;
    }

private:
    std::shared_ptr<internal::CancelData> cancel_data_;
};


class CancelTokenSource {
public:
    CancelTokenSource() : cancel_data_(std::make_shared<internal::CancelData>()) {

    }

    CancelToken GetToken() const {
        return CancelToken{ cancel_data_ };
    }

    void Cancel() {

        if (cancel_data_) {
            cancel_data_->is_cancelled.store(true);
        }
    }

private:
    std::shared_ptr<internal::CancelData> cancel_data_;
};

}