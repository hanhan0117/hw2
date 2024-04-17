#include <iostream>
#include <string>
#include <chrono>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "GaussianblurFilter.h"
#include "Testbench.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv)
{
  auto start_time = std::chrono::high_resolution_clock::now();
  if ((argc < 3) || (argc > 4))
  {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }

  // Create modules and signals
  Testbench tb("tb");
  GaussianblurFilter gaussianblur_filter("gaussianblur_filter");
  tb.initiator.i_skt(gaussianblur_filter.t_skt);

  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "Function runtime: " << duration.count() * 1e6 << " ns " << std::endl;

  return 0;
}
