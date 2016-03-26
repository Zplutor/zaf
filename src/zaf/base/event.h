#pragma once

#include <functional>
#include <tuple>
#include <vector>

namespace zaf {

/**
 Provides a template to generate event types.	
 */
template<typename... ArgTypes>
class Event {
public:
	/**
	 Type of the listener id.

	 The listener id is used to remove the specified listener from an event.
	 */
	typedef int ListenerId;

	/**
	 Type of the listener.

	 A listener is a callback function. The prorotype is defined by the template
	 argument list ArgTypes.
	 */
	typedef std::function<void(ArgTypes...)> ListenerType;

	/**
	 Provides interfaces to access listeners in an event.
	 */
	class Proxy {
	public:
		/**
		 Construct the instance with specified event.
		 */
		explicit Proxy(Event& event) : event_(event) { }

		/**
		 Add a listener to the event.
		 */
		ListenerId AddListener(const ListenerType& listener) {
			return event_.AddListener(listener);
		}

		/**
		 Add a listener associating with a tag to the event.
		 */
		ListenerId AddListener(const ListenerType& listener, void* tag) {
			return event_.AddListener(listener, tag);
		}

		/**
		 Remove a listener with specified listener id.
		 */
		void RemoveListener(ListenerId listener_id) {
			event_.RemoveListener(listener_id);
		}

		/**
		 Remove all listeners associate with specified tag.
		 */
		void RemoveListeners(void* tag) {
			event_.RemoveListeners(tag);
		}

	private:
		Event& event_;
	};

public:
	/**
	 Construct the instance.
	 */
	Event() : listener_id_seed_() { }

	/**
	 Construct the instance from another.
	 */
	Event(const Event&& other) :
		listener_id_seed_(other.listener_id_seed_),
		listeners_(std::move(other.listeners_)) {

	}

	/**
	 Assign the instance from another.
	 */
	Event& operator=(const Event&& other) {
		listener_id_seed_ = other.listener_id_seed_;
		listeners_ = std::move(other.listeners_);
		return *this;
	}

	/**
	 Add a listener to the event.

	 @return
	 Return a listener id identifies the listener being added.
	 */
	ListenerId AddListener(const ListenerType& listener) {
		return AddListener(listener, nullptr);
	}

	/**
	 Add a listener associating with a tag to the event.

	 @return
	 Return a listener id identifies the listener being added.
	 */
	ListenerId AddListener(const ListenerType& listener, void* tag) {

		ListenerId id = ++listener_id_seed_;
		listeners_.push_back(std::make_tuple(id, listener, tag));
		return id;
	}

	/**
	 Remove a listener with specified listener id.
	 */
	void RemoveListener(ListenerId listener_id) {

		for (auto iterator = listeners_.begin(); iterator != listeners_.end(); ++iterator) {
			if (std::get<0>(*iterator) == listener_id) {
				listeners_.erase(iterator);
				break;
			}
		}
	}

	/**
	 Remove all listeners associate with specified tag.
	 */
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

	/**
	 Trigger the event.

	 All listeners would be called. The later a listener is added, 
	 the later it is called.
	 */
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