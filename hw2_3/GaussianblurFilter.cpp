#include <cmath>
#include <iomanip>

#include "GaussianblurFilter.h"

GaussianblurFilter::GaussianblurFilter(sc_module_name n) : sc_module(n), t_skt("t_skt"), base_offset(0)
{
  SC_THREAD(do_filter);

  t_skt.register_b_transport(this, &GaussianblurFilter::blocking_transport);
}

GaussianblurFilter::~GaussianblurFilter() = default;

void GaussianblurFilter::do_filter()
{
  unsigned char grey[5][5] = {0};
  while (true)
  {

    for (unsigned int x = 0; x < MASK_X; ++x)
    {
      grey[x][MASK_Y - 1] = (i_r.read() + i_g.read() + i_b.read()) / 3;
    }

    for (unsigned int i = 0; i < MASK_N; ++i)
    {
      val[i] = 0;
    }
    for (unsigned int v = 0; v < MASK_Y; ++v)
    {
      for (unsigned int u = 0; u < MASK_X; ++u)
      {
        for (unsigned int i = 0; i != MASK_N; ++i)
        {
          val[i] += grey[u][v] * mask[i][u][v];
          if (v > 0 && v < MASK_Y)
          {
            grey[u][v - 1] = grey[u][v];
          }
        }
      }
    }

    double total = 0;
    for (unsigned int i = 0; i != MASK_N; ++i)
    {
      total += val[i];
    }
    int result = (int)(total / 273);
    o_result.write(result);

    // wait(10 * CLOCK_PERIOD, SC_NS); // May cause system to hang
  }
}

void GaussianblurFilter::blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay)
{
  sc_dt::uint64 addr = payload.get_address();
  addr = addr - base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;
  switch (payload.get_command())
  {
  case tlm::TLM_READ_COMMAND:
    switch (addr)
    {
    case GAUSSIANBLUR_FILTER_RESULT_ADDR:
      buffer.uint = o_result.read();
      break;
    case GAUSSIANBLUR_FILTER_CHECK_ADDR:
      buffer.uint = o_result.num_available();
      break;
    default:
      std::cerr << "Error! GaussianblurFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      break;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    delay = sc_time(4, SC_NS);
    break;

  case tlm::TLM_WRITE_COMMAND:
    switch (addr)
    {
    case GAUSSIANBLUR_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff)
      {
        i_r.write(data_ptr[0]);
      }
      if (mask_ptr[1] == 0xff)
      {
        i_g.write(data_ptr[1]);
      }
      if (mask_ptr[2] == 0xff)
      {
        i_b.write(data_ptr[2]);
      }
      delay = sc_time(1, SC_NS);
      break;
    default:
      std::cerr << "Error! GaussianblurFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      break;
    }
    break;

  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  default:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  }
  payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}