#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
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

auto print_test_data(std::vector<std::set<int>> const& sets) -> void
{
  std::cout << "[\n";
  for (auto const& set : sets)
  {
    std::cout << "  {";
    for (auto const element : set)
    {
      std::cout << element << ',';
    }
    std::cout << "},\n";
  }
  std::cout << "]\n";
}

[[nodiscard]] auto define_test_data() noexcept -> std::vector<std::set<int>>
{
  // Test data parameters
  auto set_count = 10;
  auto set_size = 10;
  auto max_element = 100;

  auto test_data = std::vector<std::set<int>>{};
  auto seed = uint64_t(0);
  auto mt19937 = std::mt19937(seed);
  auto dist = std::uniform_int_distribution(0, max_element);

  for (auto i = 0; i < set_count; ++i)
  {
    auto& set = test_data.emplace_back();
    for (auto j = 0; j < set_size; ++j)
    {
      set.emplace(dist(mt19937));
    }
  }
  return test_data;
}

auto single_pass_union_size(std::vector<std::set<int>> const& sets)
{
  auto working_set = std::set<int>{};
  for (auto const& set : sets)
  {
    std::set_union(
      std::begin(working_set),
      std::end(working_set),
      std::begin(set),
      std::end(set),
      std::inserter(working_set, std::begin(working_set))
    );
  }
  return working_set.size();
}

int main()
{
  auto sets = define_test_data();
  print_test_data(sets);
  std::cout << "single_pass_union_size = " << single_pass_union_size(sets) << std::endl;
  auto result = run_performance_trial(baseline_func, sets);
  std::cout << "baseline performance (ns) = " << result.count() << '\n';

  result = run_performance_trial(single_pass_union_size, sets);
  std::cout << "single_pass_union_size perf (ns) = " << result.count() << '\n';
}
