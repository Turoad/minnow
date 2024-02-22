#pragma once

#include "byte_stream.hh"

#include <vector>
using namespace std;

class StrContainer {
public:
  StrContainer(uint64_t size): size_(size),
  data_(vector<char>(size)), exists_(vector<uint8_t>(size, 0)){} 

  void set_data(uint64_t idx, char c) {
    uint64_t real_idx = (idx + start_index_) % size_;
    data_[real_idx] = c; 
    if (exists_[real_idx] == 0) {
      exists_[real_idx] = 1;
      exist_size_++;
    }
  }

  string GetAndUpdate() {
    uint64_t idx = 0;
    string ret = "";
    while(1) {
      uint64_t real_idx = (idx + start_index_) % size_;
      if (exists_[real_idx] == 1) {
        ret += data_[real_idx];
        exists_[real_idx] = 0;
        exist_size_--;
      }
      else break;
      idx++;
    }
    start_index_ = (idx + start_index_) % size_;
    return ret;
  }

  uint64_t get_exist_size() const {
    return exist_size_;
  }

  bool get_exist(uint64_t idx) {
    return exists_[(idx + start_index_)%size_]; 
  }

  bool get_data(uint64_t idx) {
    return data_[(idx + start_index_)%size_]; 
  }


private:
  uint64_t size_;
  std::vector<char> data_;
  std::vector<uint8_t> exists_;
  uint64_t start_index_ = 0;
  
  uint64_t exist_size_ = 0;
};

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ),
    str_(StrContainer(output_.writer().available_capacity())) {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_; // the Reassembler writes to this ByteStream

  StrContainer str_;

  uint64_t last_index_ = 0;
  uint64_t tot_need_push_ = 0;
  bool received_last_ = false;
};
