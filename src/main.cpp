#include "hdbg/DebugSession.h"
#include "hk/diag/diag.h"
#include "hk/services/sm.h"
#include "hk/services/socket/address.h"
#include "hk/services/socket/config.h"
#include "hk/services/socket/service.h"
#include "hk/svc/results.h"
#include "hk/types.h"
#include "hk/util/Stream.h"
#include "hk/util/Tuple.h"
#include <algorithm>
#include <cstdio>
#include <ranges>

constexpr hk::socket::ServiceConfig cConfig;
alignas(hk::cPageSize) u8 buffer[cConfig.calcTransferMemorySize() + 0x20000];

void hk::diag::hkLogSink(const char* msg, size len) {
    static hk::Handle lightHandle = 0;
    if (!lightHandle) {
        auto res = svc::ConnectToNamedPort(&lightHandle, "hklog");
        if (res.failed())
            svc::Break(svc::BreakReason_User, nullptr, res.getValue());
    };

    hk::util::Stream stream(hk::svc::getTLS()->ipcMessageBuffer, hk::sf::cTlsBufferSize);
    stream.write(hk::sf::hipc::Header { .tag = 15, .sendBufferCount = 1, .dataWords = 8 });
    stream.write(hk::sf::hipc::Buffer(sf::hipc::BufferMode::Normal, u64(msg), len));
    auto res = svc::SendSyncRequest(lightHandle);
    if (res.failed())
        svc::Break(svc::BreakReason_User, nullptr, res.getValue());
}

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
}
