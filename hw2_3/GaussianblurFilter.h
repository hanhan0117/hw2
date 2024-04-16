#ifndef GAUSSIANBLUR_FILTER_H_
#define GAUSSIANBLUR_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "tlm"
#include "tlm_utils/simple_target_socket.h"

#include "filter_def.h"

class GaussianblurFilter : public sc_module
{
public:
  tlm_utils::simple_target_socket<GaussianblurFilter> t_skt;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<int> o_result;
  unsigned char grey[5][5];

  SC_HAS_PROCESS(GaussianblurFilter);
  GaussianblurFilter(sc_module_name n);
  ~GaussianblurFilter();

private:
  void do_filter();
  int val[MASK_N];

  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay);
};
#endif
