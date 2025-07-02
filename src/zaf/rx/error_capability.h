#pragma once

#include <zaf/rx/observer_functions.h>

namespace zaf::rx {

template<typename OBSERVABLE, typename OBSERVER>
class ErrorCapability {
public:
    OBSERVABLE DoOnError(OnError on_error) {
        return static_cast<OBSERVABLE*>(this)->Do(OBSERVER::Create(std::move(on_error)));
    }

    template<typename E>
    OBSERVABLE DoOnError(std::function<void(const E&)> handler) {

        auto on_error = [handler = std::move(handler)](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const E& error) {
                handler(error);
            }
            catch (...) {

            }
        };
        return static_cast<OBSERVABLE*>(this)->Do(OBSERVER::Create(std::move(on_error)));
    }

    OBSERVABLE Catch(std::function<OBSERVABLE(const std::exception_ptr&)> handler) {

        const auto& core = static_cast<OBSERVABLE*>(this)->Core();
        auto new_core = core->Catch(
            [handler = std::move(handler)](const std::exception_ptr& error) {
                return handler(error).Core();
            });
        return OBSERVABLE{ std::move(new_core) };
    }
};

}