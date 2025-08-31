#pragma once

#include "hk/types.h"

namespace hdbg::processor {
  extern void initialize();
  extern void processData(const u8* data, size length);
}
