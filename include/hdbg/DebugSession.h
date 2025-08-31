#pragma once

#include "hk/ValueOrResult.h"
#include "hk/os/Thread.h"
#include "hk/types.h"
#include "hk/util/Tuple.h"

namespace hdbg {

    class DebugSession {
        enum class ProcessState {
            WaitForCreate,
            Running,
            Exited,
        };

        ProcessState mState = ProcessState::WaitForCreate;
        hk::svc::Handle mDebugHandle = 0;
        u64 mProcessId = 0;
        u64 mProgramId = 0;
        char mProcessName[0xC] { '\0' };

        hk::os::Thread* mEventThread = nullptr;
        bool mRunEventThread = true;
        bool mDoContinue = false;

        DebugSession(hk::svc::Handle debugHandle, u64 processId, u64 programId = 0);

        NON_COPYABLE(DebugSession);

        void eventThread();
        static void runEventThread(DebugSession* session) { session->eventThread(); }

        void handleDebugEvents();
        void handleDebugEvent(const hk::svc::DebugEventInfo& info);
        void handleException(const hk::svc::DebugInfoException& info);

        hk::Result continueImpl();

    public:
        hk_noinline DebugSession(DebugSession&& other)
            : mDebugHandle(other.mDebugHandle)
            , mEventThread(other.mEventThread) {
            other.mDebugHandle = 0;
            other.mEventThread = nullptr;
        }
        ~DebugSession();

        static hk::ValueOrResult<DebugSession> make(u64 processId);
        static hk::ValueOrResult<DebugSession> makeFromProgramId(u64 programId);

        void continue_() { mDoContinue = true; }
        ProcessState getState() const { return mState; }

        hk::Result writeSlow(ptr addr, const void* buffer, size size) {
            return hk::svc::WriteDebugProcessMemory(mDebugHandle, buffer, addr, size);
        }

        hk::Result readSlow(ptr addr, void* buffer, size size) {
            return hk::svc::ReadDebugProcessMemory(buffer, mDebugHandle, addr, size);
        }

        hk::ValueOrResult<hk::Tuple<hk::svc::MemoryInfo, u32>> queryMemory(ptr addr) {
            return hk::svc::QueryDebugProcessMemory(mDebugHandle, addr);
        }
    };

} // namespace hdbg
