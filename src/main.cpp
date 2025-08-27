#include "hk/diag/diag.h"
#include "hk/services/sm.h"
#include "hk/services/socket/address.h"
#include "hk/services/socket/config.h"
#include "hk/services/socket/service.h"
#include "hk/types.h"
#include "hk/util/Tuple.h"
#include <cstdio>

constexpr hk::socket::ServiceConfig cConfig;
alignas(hk::cPageSize) u8 buffer[cConfig.calcTransferMemorySize() + 0x20000];

extern "C" u32 test(u32 a, u32 b);
extern "C" void hkMain() {
    hk::sm::ServiceManager::initialize()->registerClient();

    auto* socket = hk::socket::Socket::initialize(cConfig, buffer);

    // HK_ABORT("balls: %d", *(new int(4)));

    auto [err, errno] = socket->socket(hk::socket::AddressFamily::Ipv4, hk::socket::Type::Stream, hk::socket::Protocol(0));

    s32 fd = err;
    HK_ABORT_UNLESS(fd >= 0, "fd: %d, errno: %d", fd, errno);

    tie(err, errno) = socket->bind(fd, hk::socket::SocketAddrIpv4::parse<"0.0.0.0">(8008));
    HK_ABORT_UNLESS(err >= 0, "err: %d, errno: %d", err, errno);

    tie(err, errno) = socket->listen(fd, 1);
    HK_ABORT_UNLESS(err >= 0, "err: %d, errno: %d", err, errno);

    while (1) {
        s32 clientFd;
        hk::socket::SocketAddrIpv4 client;
        tie(clientFd, errno, client) = socket->accept(fd);
        HK_ABORT_UNLESS(clientFd >= 0, "clientFd: %d, errno: %d", clientFd, errno);

        char buffer[256] = {};
        s32 size = std::sprintf(buffer, "awesome sauce %u\n", test(3, 4));

        tie(err, errno) = socket->send(clientFd, std::span<const char>(buffer, size), 0);
        socket->close(clientFd);
    }
    // HK_ABORT("bye %d %d", err, errno);
}
