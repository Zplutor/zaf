#pragma once

namespace zaf {
namespace internal {
class ControlUpdateLock;
}

class ControlUpdateGuard {
public:
	explicit ControlUpdateGuard(const std::shared_ptr<internal::ControlUpdateLock>& lock) :
		lock_(lock) {

	}

	ControlUpdateGuard(const ControlUpdateGuard&) = delete;
	ControlUpdateGuard& operator=(const ControlUpdateGuard&) = delete;

	ControlUpdateGuard(ControlUpdateGuard&&) = default;
	ControlUpdateGuard& operator=(ControlUpdateGuard&&) = default;

private:
	std::shared_ptr<internal::ControlUpdateLock> lock_;
};

}