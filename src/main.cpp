#include "hk/ValueOrResult.h"
#include "hk/diag/diag.h"
#include "hk/services/lm.h"
#include "hk/services/sm.h"
#include "hk/services/socket/address.h"
#include "hk/services/socket/config.h"
#include "hk/services/socket/service.h"
#include "hk/types.h"

constexpr hk::socket::ServiceConfig cConfig;
alignas(hk::cPageSize) u8 buffer[cConfig.calcTransferMemorySize() + 0x20000];

extern "C" void hkMain() {
    hk::sm::ServiceManager::initialize()->registerClient();

    auto* socket = hk::socket::Socket::initialize(cConfig, buffer);

    {
        /*auto logger = HK_UNWRAP(hk::lm::LogManager::initialize()->getLogger());

    logger.log("loger");

    HK_ABORT("asdfsd", 0);*/
    }

    auto [err, errno] = socket->socket(hk::socket::AddressFamily::Ipv4, hk::socket::Type::Stream, hk::socket::Protocol(0));

    s32 fd = err;
    HK_ABORT_UNLESS(fd >= 0, "fd: %d, errno: %d", fd, errno);

    tie(err, errno) = socket->bind(fd, hk::socket::SocketAddrIpv4::parse<"0.0.0.0">(2405));
    HK_ABORT_UNLESS(err >= 0, "err: %d, errno: %d", err, errno);

    hk::socket::SocketAddrIpv4 client;
    tie(err, errno, client) = socket->accept(fd);
    HK_ABORT_UNLESS(err >= 0, "err: %d, errno: %d", err, errno);

    constexpr auto shit = "PENIS!";
    tie(err, errno) = socket->send(fd, std::span<const char>(shit, sizeof("PENIS!")), 0);

    HK_ABORT("bye %d %d", err, errno);
}