# include "../../graphics-origin/extlibs/thrust/sort.h"
# include "../../graphics-origin/extlibs/thrust/system/omp/execution_policy.h"
# include <chrono>
# include <random>
# include <vector>

double unit_random()
{
  static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
  static std::uniform_real_distribution<double> distribution(double(0), double(1));
  return distribution(generator);
}

int main(int argc, char* argv[])
{
  constexpr size_t input_size = 100000;
  std::vector< double > input(input_size, 0);
  for ( size_t i = 0; i < input_size; ++i)
  {
    input[i] = unit_random() * 1000;
  }

  //thrust::sort(thrust::omp::par, input.begin(), input.end());
  thrust::sort( input.begin(), input.end()); // require to compile with /DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP

  return 0;
}