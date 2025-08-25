#include "hdbg/DebugSession.h"
#include "hk/svc/api.h"

namespace hdbg {

    // hk::ValueOrResult<DebugSession> DebugSession::makeSession(u64 processId) {
    //     hk::svc::Handle debugHandle;
    //     HK_TRY(debugHandle = hk::svc::DebugActiveProcess(processId));

    //     return move(DebugSession(debugHandle));
    // }

    DebugSession::DebugSession(hk::svc::Handle debugHandle)
        : mDebugHandle(debugHandle) { }

    DebugSession::~DebugSession() {
        if (mDebugHandle != 0)
            hk::svc::CloseHandle(mDebugHandle);
    }

} // namespace hdbg
