#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>

using perf_trial_result = std::chrono::nanoseconds;
using set = std::set<int>;
using set_size_t = std::set<int>::size_type;

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

auto baseline_func(std::vector<std::set<int>> const& sets) -> set_size_t
{
  return {};
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

[[nodiscard]] auto define_test_data() noexcept -> std::vector<set>
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

auto single_pass_union_size(std::vector<set> const& sets) -> set_size_t
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

auto next_multi_index(std::vector<int> multi_index, int set_count) -> std::vector<int>
{
  auto multi_index_size = multi_index.size();
  for (auto lwm = multi_index_size - 1; lwm >= 0; --lwm)
  {
    if (multi_index[lwm] >= set_count - (multi_index_size - 1 - lwm))
      continue;

    auto n = multi_index[lwm] + 1;
    for (auto i = lwm; i < multi_index.size(); ++i, ++n)
      multi_index[i] = n++;
    return multi_index;
  }
  return {};
}

auto inclusion_exclusion_union_size(std::vector<set> const& sets) -> set_size_t
{
  auto acc = set_size_t{ 0 };
  for (auto set_count = 1; set_count <= sets.size(); ++set_count)
  {
    auto set_count_is_even = set_count % 2 == 0;
    auto multi_index = std::vector<int>(set_count);
    for (auto i = 0; i < set_count; ++i)
      multi_index[i] = i;

    for (; !multi_index.empty(); multi_index = next_multi_index(multi_index, sets.size()))
    {
      // intersect the sets
      auto intersection = sets[multi_index[0]];
      for (auto i = 1; i < multi_index.size(); ++i)
      {
        auto next_intersection = set{};
        std::set_intersection(
          std::begin(intersection),
          std::end(intersection),
          std::begin(sets[multi_index[i]]),
          std::end(sets[multi_index[i]]),
          std::inserter(next_intersection, std::begin(next_intersection))
        );
        intersection = std::move(next_intersection);
      }
      if (set_count_is_even)
      {
        acc -= intersection.size();
      }
      else
      {
        acc += intersection.size();
      }
    }
  }
  return acc;
}

int main()
{
  auto sets = define_test_data();
  print_test_data(sets);
  auto result = run_performance_trial(baseline_func, sets);
  std::cout << "baseline performance (ns) = " << result.count() << '\n';

  std::cout << "single_pass_union_size = " << single_pass_union_size(sets) << std::endl;
  result = run_performance_trial(single_pass_union_size, sets);
  std::cout << "single_pass_union_size perf (ns) = " << result.count() << '\n';

  std::cout << "inclusion_exclusion_union_size = " << inclusion_exclusion_union_size(sets) << std::endl;
  result = run_performance_trial(inclusion_exclusion_union_size, sets);
  std::cout << "inclusion_exclusion_union_size perf (ns) = " << result.count() << '\n';
}
