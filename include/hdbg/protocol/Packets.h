#pragma once

#include "hk/types.h"
#include <span>
#define HDBG_PACKED __attribute__((packed))

namespace hdbg::protocol {

    struct HDBG_PACKED Handshake {
        u32 version;
    };
    struct HDBG_PACKED Header {
        u32 size;
        u16 packetType;
    };

    enum class FromSwitch: u16 {
        BreakpointHit = 0,
        SubscriptionUpdated = 2
    };

    enum class BreakpointKind : u8 {
        Soft,
        Hard,
        Watch,
    };
    struct HDBG_PACKED SetBreakpoint {
        u32 logicalId;
        u64 address;
        BreakpointKind kind;
    };
    struct HDBG_PACKED DeleteBreakpoint {
        u32 logicalId;
    };
    struct HDBG_PACKED BreakpointHit {
        u32 logicalId;
    };

    struct HDBG_PACKED UpdateSubscription {
        u32 id;
        u64 address;
        u32 size;
    };
    struct HDBG_PACKED DeleteSubscription {
        u32 id;
    };
    struct HDBG_PACKED SubscriptionChange {
        u32 offset;
        u16 byte_count;
    };
    struct HDBG_PACKED SubscriptionUpdated {
        u64 address;
        u16 changeCount;
    };
}

#undef HDBG_PACKED
