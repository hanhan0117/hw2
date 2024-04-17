#include "Initiator.h"
#include <systemc>
using namespace sc_core;

Initiator::Initiator(sc_module_name n) : sc_module(n), i_skt("i_skt") {}

int Initiator::read_from_socket(unsigned long int addr, unsigned char mask[],
                                unsigned char rdata[], int dataLen)
{
    // Set up the payload fields. Assume everything is 4 bytes.
    trans.set_read();
    trans.set_address((sc_dt::uint64)addr);

    trans.set_byte_enable_length((const unsigned int)dataLen);
    trans.set_byte_enable_ptr((unsigned char *)mask);

    trans.set_data_length((const unsigned int)dataLen);
    trans.set_data_ptr((unsigned char *)rdata);

    // Transport.
    do_trans(trans);

    /* For now just simple non-zero return code on error */
    return trans.is_response_ok() ? 0 : -1;

} // read_from_socket()

int Initiator::write_to_socket(unsigned long int addr, unsigned char mask[],
                               unsigned char wdata[], int dataLen)
{
    // Set up the payload fields. Assume everything is 4 bytes.
    trans.set_write();
    trans.set_address((sc_dt::uint64)addr);

    trans.set_byte_enable_length((const unsigned int)dataLen);
    trans.set_byte_enable_ptr((unsigned char *)mask);

    trans.set_data_length((const unsigned int)dataLen);
    trans.set_data_ptr((unsigned char *)wdata);

    // Transport.
    do_trans(trans);

    /* For now just simple non-zero return code on error */
    return trans.is_response_ok() ? 0 : -1;

} // writeUpcall()

void Initiator::do_trans(tlm::tlm_generic_payload &trans)
{
    m_qk.set_global_quantum(sc_time(4, SC_NS));
    m_qk.reset();
    // sc_core::sc_time dummyDelay = sc_core::SC_ZERO_TIME;
    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
    // Call the transport and wait for no time, which allows the thread to yield
    // and others to get a look in!

    i_skt->b_transport(trans, delay);
    // wait(sc_core::SC_ZERO_TIME);
    // wait(dummyDelay);

    m_qk.inc(delay);
    if (m_qk.need_sync())
        m_qk.sync();

} // do_trans()
