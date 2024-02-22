#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if (is_last_substring) {
    received_last_ = true;
    tot_need_push_ = first_index + data.size();
  }
  uint64_t cap = output_.writer().available_capacity();
  // assign
  for (size_t i = 0; i < data.size(); ++i) {
    if (first_index + i >= last_index_ + cap) break;
    if (first_index + i < last_index_) continue;
    uint64_t cur = first_index + i - last_index_;
    str_.set_data(cur, data[i]);
  }

  // check push
  if (str_.get_exist(0)) {
    string push_data = str_.GetAndUpdate();
    last_index_ += push_data.size();
    output_.writer().push(push_data);
  }

  // check close
  if (received_last_ && output_.writer().bytes_pushed() >= tot_need_push_) {
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return str_.get_exist_size();
}
