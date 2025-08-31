#pragma once

#include "hdbg/protocol/Packets.h"
#include "hk/Result.h"
#include "hk/types.h"
#include "hk/util/Vec.h"
#include <span>

namespace hdbg::protocol {
    struct Handler {
        virtual void setBreakpoint(SetBreakpoint& packet);
        virtual void deleteBreakpoint(DeleteBreakpoint& packet);
        virtual void updateSubscription(UpdateSubscription& packet);
        virtual void deleteSubscription(DeleteSubscription& packet);
    };

    template <typename R>
    inline hk::Result process(class Handler* handler, R&& readerFunc) {
        hk::util::Vec<u8, 512> bytes;
        while (true) {
            Header header;
            HK_TRY(readerFunc(std::span<u8>(cast<u8*>(&header), sizeof(Header))));

            bytes.clear();
            bytes.reserve(header.size);
            readerFunc(std::span<u8>(bytes.begin(), bytes.end()));

            switch (header.packetType) {
            case 0:
                handler->setBreakpoint(*cast<SetBreakpoint*>(bytes.begin()));
                break;
            case 1:
                handler->deleteBreakpoint(*cast<DeleteBreakpoint*>(bytes.begin()));
                break;
            case 10:
                handler->updateSubscription(*cast<UpdateSubscription*>(bytes.begin()));
                break;
            case 11:
                handler->deleteSubscription(*cast<DeleteSubscription*>(bytes.begin()));
                break;
            }
        }
    }

    template <typename W, typename T>
    inline void writeValue(W&& writer, T&& value) {
        writer(std::span<u8>(cast<u8*>(&value), sizeof(T)));
    }
    template <typename W>
    inline void breakpointHit(W&& writer, u32 logicalId) {
        writeValue(writer, Header {
                               .size = sizeof(BreakpointHit),
                               .packetType = u16(FromSwitch::BreakpointHit),
                           });

        writeValue(writer, BreakpointHit { .logicalId = logicalId });
    }
    struct Change {
        u32 offset;
        std::span<u8> bytes;
    };
    template <typename W>
    inline void subscriptionUpdated(W&& writer, u64 address, std::span<Change> changes) {
        u32 size = sizeof(SubscriptionUpdated);
        for (auto& change : changes) {
            size += sizeof(SubscriptionChange);
            size += change.bytes.size();
        }

        writeValue(writer, Header {
                               .size = size,
                               .packetType = u16(FromSwitch::BreakpointHit),
                           });

        writeValue(writer, SubscriptionUpdated {
                               .address = address,
                               .changeCount = u16(changes.size()),
                           });

        for (auto change : changes) {
            writeValue(writer, SubscriptionChange { .offset = change.offset, .byte_count = u16(change.bytes.size()) });
            writer(change.bytes);
        }
    }
}
