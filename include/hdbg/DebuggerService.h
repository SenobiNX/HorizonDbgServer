#pragma once

#include "hdbg/DebugSession.h"
#include "hk/util/Arena.h"
#include "hk/util/Singleton.h"

namespace hdbg {

    class DebuggerService {
        HK_SINGLETON(DebuggerService)

        hk::util::Arena<DebugSession> mSessions;

    public:
    };

} // namespace hdbg
