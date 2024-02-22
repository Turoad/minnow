#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if (message.RST) {
    reassembler_.reader().set_error();
  }

  // Your code here.

  // convert to absolute
  if (message.SYN) {
    zero_point_ = message.seqno;
    ack_ = true;
  }
  uint64_t ck = reassembler_.writer().bytes_pushed();
  uint64_t as = message.seqno.unwrap(zero_point_, ck);
  if (message.FIN) {
    tot_need_push_ = as + message.payload.size();
  }
  if (message.SYN==false) as -= 1;
  reassembler_.insert(as, message.payload, message.FIN);
  fin_ = fin_ | message.FIN;
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage trm;
  if (ack_) {
    uint32_t fin = reassembler_.writer().is_closed();
    trm.ackno = Wrap32::wrap(reassembler_.writer().bytes_pushed() + 1 + fin, zero_point_);
  }
  trm.window_size = min(reassembler_.writer().available_capacity(), (1ul<<16)-1);
  trm.RST = reassembler_.writer().has_error();
  return trm;
}
