#include "hk/diag/diag.h"
#include "hk/types.h"

extern "C" void rust_log(const char* text, size len) {
  hk::diag::hkLogSink(text, len);
}

extern "C" void hk_noreturn rust_panicked() {
  HK_ABORT("rust panicked", 0);
}
