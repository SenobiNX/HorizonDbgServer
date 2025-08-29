#pragma once

#include "hk/ValueOrResult.h"
#include "hk/svc/types.h"
#include "hk/types.h"

namespace hdbg {

    class DebugSession {
        hk::svc::Handle mDebugHandle = 0;

        DebugSession(hk::svc::Handle debugHandle);
        ~DebugSession();

        NON_COPYABLE(DebugSession);

        DebugSession(DebugSession&& other)
            : mDebugHandle(other.mDebugHandle) {
            other.mDebugHandle = 0;
        }

    public:
        static hk::ValueOrResult<DebugSession> makeSession(u64 processId);
        static hk::ValueOrResult<DebugSession> makeSessionFromProgramId(u64 programId);
    };

} // namespace hdbg
