#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  // (void)n;
  // (void)zero_point;
  // return Wrap32 { 0 };
  return zero_point + n;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  // (void)zero_point;
  // (void)checkpoint;
  // return {};
  uint64_t zp = zero_point.raw_value();
  uint64_t mod = 1ll << 32;
  uint64_t rv = (raw_value_ + mod - zp)%mod;
  uint64_t div = checkpoint / mod;

  uint64_t ret = 0;

  uint64_t base = div * mod + rv;

  if (base < checkpoint) {
    ret = base + mod - checkpoint > checkpoint - base ? base : base +mod; 
  } else {
    if (div == 0) ret = base;
    else {
      ret = checkpoint - (base - mod) > base - checkpoint ? base : base - mod; 
    }
  }
  return ret;
}
