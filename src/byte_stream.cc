#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), oss_(""), peek_("") {
}

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  size_t cap = available_capacity(); 
  if (cap > data.size()) {
    oss_ += data;
    bytes_buffered_ += data.size();
  }
  else {
    oss_ += data.substr(0, cap);
    bytes_buffered_ += cap;
  }
 // std::cout << "cap " << cap << " data " << data << std::endl;

  return;
}

void Writer::close()
{
  // Your code here.
  is_closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - oss_.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_buffered_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_closed_ && (bytes_buffered() == 0);
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  return oss_;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  size_t can_pop = len;
  if (len > oss_.size()) can_pop = oss_.size();
  oss_ = oss_.substr(can_pop, oss_.size() - can_pop);
  bytes_popped_ += can_pop;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return oss_.size();
}
