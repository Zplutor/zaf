#pragma once

#include <functional>
#include <tuple>
#include <vector>

namespace zaf {

template<typename... ArgTypes>
class Event {
public:
	typedef int ListenerId;
	typedef std::function<void(ArgTypes...)> ListenerType;

	class Proxy {
	public:
		explicit Proxy(Event& event) : event_(event) { }

		ListenerId AddListener(const ListenerType& listener) {
			return event_.AddListener(listener);
		}

		ListenerId AddListener(const ListenerType& listener, void* tag) {
			return event_.AddListener(listener, tag);
		}

		void RemoveListener(ListenerId listener_id) {
			event_.RemoveListener(listener_id);
		}

		void RemoveListeners(void* tag) {
			event_.RemoveListeners(tag);
		}

	private:
		Event& event_;
	};

public:
	Event() : listener_id_seed_() { }

	Event(const Event& other) : 
		listener_id_seed_(other.listener_id_seed_), 
		listeners_(other.listeners_) {
	
	}

	Event(const Event&& other) :
		listener_id_seed_(other.listener_id_seed_),
		listeners_(std::move(other.listeners_)) {

	}

	Event& operator=(const Event& other) {
		if (this != &other) {
			listener_id_seed_ = other.listener_id_seed_;
			listeners_ = other.listeners_;
		}
		return *this;
	}

	ListenerId AddListener(const ListenerType& listener) {
		return AddListener(listener, nullptr);
	}

	ListenerId AddListener(const ListenerType& listener, void* tag) {

		ListenerId id = ++listener_id_seed_;
		listeners_.push_back(std::make_tuple(id, listener, tag));
		return id;
	}

	void RemoveListener(ListenerId listener_id) {

		for (auto iterator = listeners_.begin(); iterator != listeners_.end(); ++iterator) {
			if (std::get<0>(*iterator) == listener_id) {
				listeners_.erase(iterator);
				break;
			}
		}
	}

	void RemoveListeners(void* tag) {

		auto iterator = listeners_.begin();
		while (iterator != listeners_.end()) {

			if (std::get<2>(*iterator) == tag) {
				iterator = listeners_.erase(iterator);
			}
			else {
				++iterator;
			}
		}
	}

	void Trigger(const ArgTypes&... args) const {

		for (const auto& listener : listeners_) {
			std::get<1>(listener)(args...);
		}
	}

private:
	typedef std::tuple<ListenerId, ListenerType, void*> ListenerEntry;

private:
	ListenerId listener_id_seed_;
	std::vector<ListenerEntry> listeners_;
};

}