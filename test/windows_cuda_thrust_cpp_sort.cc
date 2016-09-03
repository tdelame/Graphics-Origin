# include "../../graphics-origin/extlibs/thrust/sort.h"
# include "../../graphics-origin/extlibs/thrust/system/cpp/execution_policy.h"
# include <chrono>
# include <random>
# include <vector>
# include <iostream>

double unit_random()
{
  static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
  static std::uniform_real_distribution<double> distribution(double(0), double(1));
  return distribution(generator);
}

int main(int argc, char* argv[])
{
  {
    constexpr size_t input_size = 100000;
    std::vector< double > input(input_size, 0);
    for (size_t i = 0; i < input_size; ++i)
    {
      input[i] = unit_random() * 1000;
    }

    thrust::sort(thrust::cpp::par, input.begin(), input.end());

    assert(thrust::is_sorted(input.begin(), input.end()));
  }

  {
    int  keys[5] = { 5, 0, 2, 9, 1 };
    int input[5] = { 1, 2, 3, 4, 5 };

    // input should be { 2, 5, 3, 1, 4 }
    thrust::sort_by_key(thrust::cpp::par, keys, keys + 5, input);

    assert(input[0] == 2);
    assert(input[1] == 5);
    assert(input[2] == 3);
    assert(input[3] == 1);
    assert(input[4] == 4);

    assert(keys[0] == 0);
    assert(keys[1] == 1);
    assert(keys[2] == 2);
    assert(keys[3] == 5);
    assert(keys[4] == 9);
  }


  return 0;
}