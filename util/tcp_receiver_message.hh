#pragma once

#include "wrapping_integers.hh"

#include <optional>
#include <sstream>

/*
 * The TCPReceiverMessage structure contains the information sent from a TCP receiver to its sender.
 *
 * It contains three fields:
 *
 * 1) The acknowledgment number (ackno): the *next* sequence number needed by the TCP Receiver.
 *    This is an optional field that is empty if the TCPReceiver hasn't yet received the Initial Sequence Number.
 *
 * 2) The window size. This is the number of sequence numbers that the TCP receiver is interested
 *    to receive, starting from the ackno if present. The maximum value is 65,535 (UINT16_MAX from
 *    the <cstdint> header).
 *
 * 3) The RST (reset) flag. If set, the stream has suffered an error and the connection should be aborted.
 */

struct TCPReceiverMessage
{
  std::optional<Wrap32> ackno {};
  uint16_t window_size {};
  bool RST {};

  uint64_t as = 0;

  std::string DebugString() const {
    std::ostringstream oss;
    std::string ackno_str = "None";
    if (ackno.has_value()) {
      Wrap32 an = ackno.value();
      ackno_str = std::to_string(an.raw_value());
    }
    oss << "ackno: " << ackno_str << " window_size: " << window_size <<
      " RST: " << RST;
    return oss.str();
  }
};
