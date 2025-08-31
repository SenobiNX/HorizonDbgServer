#include "hdbg/DebugSession.h"
#include "hk/diag/diag.h"
#include "hk/services/sm.h"
#include "hk/services/socket/address.h"
#include "hk/services/socket/config.h"
#include "hk/services/socket/service.h"
#include "hk/svc/results.h"
#include "hk/types.h"
#include "hk/util/Tuple.h"
#include <cstdio>

constexpr hk::socket::ServiceConfig cConfig;
alignas(hk::cPageSize) u8 buffer[cConfig.calcTransferMemorySize() + 0x20000];

extern "C" u32 test(u32 a, u32 b);
extern "C" void hkMain() {
    hk::sm::ServiceManager::initialize()->registerClient();

    auto* socket = hk::socket::Socket::initialize(cConfig, buffer);

    hdbg::DebugSession session = HK_UNWRAP(hdbg::DebugSession::make(84));

    ptr addr = 1;
    while (addr != 0) {
        auto [memoryInfo, page] = HK_UNWRAP(session.queryMemory(addr));

        hk::diag::logLine("%p %p %d %d", memoryInfo.base_address, memoryInfo.size, memoryInfo.permission, memoryInfo.state);
        addr = memoryInfo.base_address + memoryInfo.size;
    }

    hk::diag::logLine("awesome sauce %d", test(4, 2));
}
