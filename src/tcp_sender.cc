#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <cmath>

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  return seq_in_flight_;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return con_retran_;
}

void TCPSender::check_outstanding() {
  uint64_t as = 0;
  uint64_t ck = reader().bytes_popped();
  if (msg_.ackno) {
    Wrap32 ackno = msg_.ackno.value();
    as = ackno.unwrap(isn_, ck);
  }
  while (!queue_.empty()) {
    const auto msg = queue_.top();
    // std::cout << "as_end: " << msg.as_end << " recever as:" << as << std::endl;
    if (msg.as_end <= as) {
      queue_.pop();
      seq_in_flight_ -= msg.sequence_length();
      reset();
    } else break;
  }
}

void TCPSender::push( const TransmitFunction& transmit )
{
  // if (sequence_numbers_in_flight()) {
  //   check_outstanding();
  //   return;
  // }
  // Your code here.
  check_outstanding();
  while(window_size_ > seq_in_flight_) {
    TCPSenderMessage s_msg = make_empty_message();
    uint64_t as = last_as_;
    // uint64_t ck = reader().bytes_popped();
    // if (msg_.ackno) {
    //   Wrap32 ackno = msg_.ackno.value();
    //   as = ackno.unwrap(isn_, ck);
    //   std::cout << "ack absolute no: " << as << std::endl;
    // }
    if (as == 0) s_msg.SYN = 1;


    s_msg.seqno = Wrap32::wrap(as, isn_);
    const auto buf = reader().peek();
    uint64_t can_get = std::min(buf.size(), window_size_ - seq_in_flight_);
    can_get = std::min(can_get, (uint64_t)TCPConfig::MAX_PAYLOAD_SIZE);
    s_msg.payload = buf.substr(0, can_get);
    // if (as > ck) input_.reader().pop(as-1-ck);
    input_.reader().pop(can_get);


    if (!fin_send_ && input_.writer().is_closed() && can_get + s_msg.SYN < window_size_ - seq_in_flight_ && input_.reader().bytes_buffered() == 0) s_msg.FIN=1;

    s_msg.as = as;
    s_msg.as_end = as + s_msg.sequence_length();

    last_as_ = s_msg.as_end;

    queue_.push(s_msg);
    

    std::cout << "push: " << s_msg.DebugString() << " seq_in_flight: " << seq_in_flight_  <<" last_as: " << last_as_ << std::endl;

    if (s_msg.payload.size() == 0 && syn_send_ && !s_msg.FIN) {
      return;
    }

    seq_in_flight_ += s_msg.sequence_length();

    std::cout << "2 push: " << s_msg.DebugString() << std::endl;

    if (s_msg.sequence_length() > 0) {
      transmit(s_msg);
    } 
    if (s_msg.SYN) syn_send_ = true;
    if (s_msg.FIN) fin_send_ = true;
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  // Your code here.
  TCPSenderMessage s_msg;
  uint64_t as = last_as_;
  if (reader().has_error()) {
    s_msg.RST = true;
  }
  // uint64_t ck = reader().bytes_popped();
  // if (msg_.ackno) {
  //   Wrap32 ackno = msg_.ackno.value() + 1;
  //   as = ackno.unwrap(isn_, ck);
  // }
  // if (as == 0) s_msg.SYN = 1;
  s_msg.seqno = Wrap32::wrap(as, isn_); 
  std::cout << "make_empty_message: " << s_msg.DebugString() << std::endl;

  return s_msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  std::cout << "receive0: " << msg.DebugString() << std::endl;
  if (msg.RST) {
    writer().set_error();
  }
  if (msg.ackno.has_value()) {
    Wrap32 ackno = msg.ackno.value();
    uint64_t ck = reader().bytes_popped();
    uint64_t as = ackno.unwrap(isn_, ck);
    std::cout << "check receive: " << as << " " << last_as_ << std::endl;
    if (as > last_as_) {
      std::cout << "receive unposible ack " << std::endl;
      return;
    }

    if(as < msg_.as) {
      std::cout << "as < msg_as: " << as << " " << msg_.as << std::endl;
      return ;
    }

    msg_ = msg;
    msg_.as = as;
  } else {
    msg_ = msg;
  } 
  std::cout << "receive: " << msg.DebugString() << std::endl;
  window_size_ = std::max(msg.window_size, (uint16_t)1);
  zero_ws_ = msg.window_size > 0 ? false : true;
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // Your code here.

  check_outstanding();
  std::cout << "ms_since_last_tick: " << ms_since_last_tick << "time_ms_ " << time_ms_ << " ROT_ms_: " << RTO_ms_ << std::endl;
  time_ms_ += ms_since_last_tick;
  if (time_ms_ >= RTO_ms_ && queue_.size() > 0) {
    const auto top = queue_.top();
    std::cout << "tick: " << top.DebugString() << std::endl;
    debug_queue();
    transmit(top);
    con_retran_ += 1;
    if (!zero_ws_) RTO_ms_ *= 2;
    time_ms_ = 0;
  }
}
