#include <chrono>
#include <iostream>
#include <set>

using perf_trial_result = std::chrono::nanoseconds;

template <
  typename Func,
  typename... Args
>
[[nodiscard]] auto run_performance_trial(Func&& func, Args&&... args) noexcept -> perf_trial_result
{
  auto start_time = std::chrono::steady_clock::now();
  func(args...);
  auto end_time = std::chrono::steady_clock::now();
  auto duration = end_time - start_time;
  return duration;
}

auto baseline_func(std::vector<std::set<int>> const& sets) -> int64_t
{
  return 0Ui64;
}

int main()
{
  auto sets = std::vector<std::set<int>>{};
  auto result = run_performance_trial(baseline_func, sets);
  std::cout << "baseline performance (ns) = " << result.count();
}
