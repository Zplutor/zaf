#pragma once

#include <Windows.h>
#include <chrono>
#include <memory>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>

namespace zaf {

class TimerTriggerInfo;

/**
 Represent a timer.
 */
class Timer : public std::enable_shared_from_this<Timer> {
public:
    /**
     Defines the timer's working mode.
     */
    enum class Mode {

        /**
         The timer is repeated, which timeout begin counted after the previous trigger 
         event has been processed.
         */
        DeferredRepeated,

        /**
         The timer is repeated, which timeout begin counted immediately withou waiting 
         for processing the previous trigger event.
         */
        ImmediatelyRepeated,

        /**
         The timer is one-shot.
         */
        OneShot,
    };

    /**
     The type of interval.
     */
    typedef std::chrono::milliseconds Interval;

public:
    /**
     The minimum interval value.
     */
    static const Interval MinimumInterval;

    /**
     The maximum interval value.
     */
    static const Interval MaximumInterval;

public:
    /**
     Initialize the timer that has specified working mode.

     The working mode is immutable after creating the timer.
     The interval has a minimum interval value by default.
     */
    Timer(Mode mode);

    /**
     Destroy the timer.

     The timer is stopped when it is destroyed.
     */
    ~Timer();

    Mode GetMode() const {
        return mode_;
    }

    /**
     Get the timer's interval.
     */
    const Interval GetInterval() const {
        return interval_;
    }

    /**
     Set the timer's interval.

     Setting the interval will restart the timer while it is running.
     */
    void SetInterval(const Interval& interval);

    /**
     Get the trigger event.
     */
    Observable<TimerTriggerInfo> TriggerEvent() {
        return trigger_event_.AsObservable();
    }

    /**
     Get a value indicating that whether the timer is running.
     */
    bool IsRunning() const {
        return is_running_;
    }

    /**
     Start the timer.
     */
    void Start();

    /**
     Stop the timer.
     */
    void Stop();

private:
    static void CALLBACK TimerProcedure(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

    void StartSystemTimer();
    void StopSystemTimer();
    void SystemTimerTrigger();
    void RaiseEvent();

private:
    Mode mode_;
    Interval interval_;
    Subject<TimerTriggerInfo> trigger_event_;
    bool is_running_;
};


class TimerTriggerInfo {
public:
    std::shared_ptr<Timer> timer;
};

}