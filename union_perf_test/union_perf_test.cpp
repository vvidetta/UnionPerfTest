#include <chrono>
#include <iostream>
#include <set>

using perf_trial_result = std::chrono::milliseconds;
using union_size_func = int64_t(std::vector<std::set<int>> const&);

template <
  typename Func,
  typename... Args
>
[[nodiscard]] auto run_performance_trial(Func&& func, Args&&... args) noexcept -> perf_trial_result
{
  return {};
}

auto baseline_func(std::vector<std::set<int>> const& sets) -> int64_t
{
  return 0Ui64;
}

int main()
{
  auto sets = std::vector<std::set<int>>{};
  auto result = run_performance_trial(baseline_func, sets);
  std::cout << "baseline performance (ms) = " << result.count();
}
