#pragma once

#include "wrapping_integers.hh"

#include <string>
#include <sstream>

/*
 * The TCPSenderMessage structure contains the information sent from a TCP sender to its receiver.
 *
 * It contains five fields:
 *
 * 1) The sequence number (seqno) of the beginning of the segment. If the SYN flag is set, this is the
 *    sequence number of the SYN flag. Otherwise, it's the sequence number of the beginning of the payload.
 *
 * 2) The SYN flag. If set, this segment is the beginning of the byte stream, and the seqno field
 *    contains the Initial Sequence Number (ISN) -- the zero point.
 *
 * 3) The payload: a substring (possibly empty) of the byte stream.
 *
 * 4) The FIN flag. If set, the payload represents the ending of the byte stream.
 *
 * 5) The RST (reset) flag. If set, the stream has suffered an error and the connection should be aborted.
 */

struct TCPSenderMessage
{
  Wrap32 seqno { 0 };

  bool SYN {};
  std::string payload {};
  bool FIN {};

  bool RST {};

  // How many sequence numbers does this segment use?
  size_t sequence_length() const { return SYN + payload.size() + FIN; }

  uint64_t as {};
  uint64_t as_end{};

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "seqno: " << seqno.raw_value() << " SYN: " << SYN << " FIN: " << FIN <<
      " RST: " << RST << " payload: " << payload.size() << " as:" << as << " as_end: " << as_end;
    return oss.str();
  }

  friend bool operator<(const TCPSenderMessage &l, 
    const TCPSenderMessage& r) {
    return l.as_end > r.as_end || (l.as_end == r.as_end && l.as > r.as);
  }
};

