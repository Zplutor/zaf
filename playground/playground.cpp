#include <zaf/application.h>
#include <zaf/control/button.h>
#include <zaf/window/window.h>
#include <zaf/rx/timer.h>
#include <zaf/base/timer.h>
#include <zaf/control/layout/linear_layouter.h>

void BeginRun(const zaf::BeginRunInfo& event_info);

constexpr std::size_t TimerInterval = 100;

class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        auto rx_timer_button = zaf::Create<zaf::Button>();
        rx_timer_button->SetFixedHeight(30);
        rx_timer_button->SetText(L"Start rx timer.");
        Subscriptions() += rx_timer_button->ClickEvent().Subscribe(std::bind([this]() {
            
            BeforeTimer();

            rx_timer_ = zaf::rx::Interval(std::chrono::milliseconds(TimerInterval)).Subscribe([this](int) {
                OnTimer();
            });
        }));
        this->RootControl()->AddChild(rx_timer_button);

        auto window_timer_button = zaf::Create<zaf::Button>();
        window_timer_button->SetFixedHeight(30);
        window_timer_button->SetText(L"Start window timer.");
        Subscriptions() += window_timer_button->ClickEvent().Subscribe(std::bind([this]() {

            BeforeTimer();

            window_timer_ = std::make_unique<zaf::Timer>(zaf::Timer::Mode::ImmediatelyRepeated);
            window_timer_->SetInterval(std::chrono::milliseconds(TimerInterval));
            Subscriptions() += window_timer_->TriggerEvent().Subscribe([this](const zaf::TimerTriggerInfo& event_info) {
                OnTimer();
            });
            window_timer_->Start();
        }));
        this->RootControl()->AddChild(window_timer_button);
    }

private:
    void BeforeTimer() {

        rx_timer_.Unsubscribe();
        window_timer_.reset();

        start_time_ = std::chrono::steady_clock::now();
        time_points_.clear();
        time_points_.reserve(100);
    }

    void OnTimer() {

        auto now = std::chrono::steady_clock::now();
        time_points_.push_back(now);

        auto debug_string = std::to_wstring(time_points_.size());
        debug_string += L"\r\n";
        OutputDebugString(debug_string.c_str());

        if (time_points_.size() <= 100) {
            return;
        }

        rx_timer_.Unsubscribe();
        window_timer_.reset();

        CalculateStatistic();
    }

    void CalculateStatistic() {

        if (time_points_.size() < 2) {
            return;
        }

        std::vector<std::size_t> elapsed_times;
        for (std::size_t index = 0; index < time_points_.size() - 1; ++index) {
            auto duration = time_points_[index + 1] - time_points_[index];
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            elapsed_times.push_back(elapsed);
        }

        auto total_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time_points_.back() - time_points_.front()).count();

        auto average = CalculateAverage(elapsed_times);
        auto min_max = CalculateMinMax(elapsed_times);
        auto range = min_max.second - min_max.first;
        auto variance = CalculateVariance(elapsed_times);
        auto standard_deviation = CalculateStandardDeviation(elapsed_times);

        std::wstring debug_string;
        debug_string += L"Total Elapsed: " + std::to_wstring(total_elapsed) + L" ms\r\n";
        debug_string += L"Average: " + std::to_wstring(average) + L" ms\r\n";
        debug_string += L"Min: " + std::to_wstring(min_max.first) + L" ms\r\n";
        debug_string += L"Max: " + std::to_wstring(min_max.second) + L" ms\r\n";
        debug_string += L"Range: " + std::to_wstring(range) + L" ms\r\n";
        debug_string += L"Standard Deviation: " + std::to_wstring(standard_deviation) + L" ms\r\n";
        debug_string += L"Variance: " + std::to_wstring(variance) + L" ms^2\r\n";
        OutputDebugString(debug_string.c_str());
    }

    double CalculateAverage(const std::vector<std::size_t>& elapsed_times) {

        if (elapsed_times.empty()) {
            return 0;
        }

        std::size_t total_elapsed = 0;
        for (const auto& elapsed : elapsed_times) {
            total_elapsed += elapsed;
        }
        double average_elapsed = static_cast<double>(total_elapsed) / (elapsed_times.size());
        return average_elapsed;
    }

    std::pair<std::size_t, std::size_t> CalculateMinMax(const std::vector<std::size_t>& elapsed_times) {

        if (elapsed_times.empty()) {
            return {};
        }

        auto min_max = std::minmax_element(elapsed_times.begin(), elapsed_times.end());
        return std::make_pair(*min_max.first, *min_max.second);
    }

    double CalculateVariance(const std::vector<std::size_t>& elapsed_times) {

        if (elapsed_times.empty()) {
            return 0;
        }
        double average_elapsed = CalculateAverage(elapsed_times);
        double variance = 0.0;
        for (const auto& elapsed : elapsed_times) {
            double diff = static_cast<double>(elapsed) - average_elapsed;
            variance += diff * diff;
        }
        variance /= elapsed_times.size();
        return variance;
    }

    double CalculateStandardDeviation(const std::vector<std::size_t>& elapsed_times) {
        if (elapsed_times.empty()) {
            return 0;
        }
        double variance = CalculateVariance(elapsed_times);
        return std::sqrt(variance);
    }

private:
    std::chrono::steady_clock::time_point start_time_;
    std::vector<std::chrono::steady_clock::time_point> time_points_;
    zaf::rx::Subscription rx_timer_;
    std::shared_ptr<zaf::Timer> window_timer_;
};


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::Instance();
    application.Subscriptions() += application.BeginRunEvent().Subscribe(BeginRun);

    application.Initialize({});
    application.Run();
}


void BeginRun(const zaf::BeginRunInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->SetSize(zaf::Size{ 1200, 600 });
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);

    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}