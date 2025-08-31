#include "hdbg/DebugSession.h"
#include "hk/diag/diag.h"
#include "hk/svc/api.h"
#include "hk/svc/results.h"
#include "hk/svc/types.h"

using namespace hk;

namespace hdbg {

    ValueOrResult<DebugSession> DebugSession::make(u64 processId) {
        svc::Handle debugHandle;
        HK_ABORT_UNLESS_R(svc::DebugActiveProcess(&debugHandle, processId));

        return move(DebugSession(debugHandle, processId));
    }

    DebugSession::DebugSession(svc::Handle debugHandle, u64 processId, u64 programId)
        : mDebugHandle(debugHandle)
        , mProcessId(processId)
        , mProgramId(programId) {
        mEventThread = new os::Thread(runEventThread, this, 0, 32_KB);
        mEventThread->start();
    }

    DebugSession::~DebugSession() {
        mRunEventThread = false;
        if (mEventThread != nullptr) {
            mEventThread->join();
            delete mEventThread;
        }

        if (mDebugHandle != 0)
            svc::CloseHandle(mDebugHandle);
    }

    void DebugSession::eventThread() {
        diag::logLine("hi");
        while (mRunEventThread and mState != ProcessState::Exited) {
            HK_ABORT_UNLESS_R(hk::svc::WaitSynchronization(&mDebugHandle, 1, -1));

            handleDebugEvents();

            if (mDoContinue) {
                continueImpl();
                mDoContinue = false;
            }
        }
        diag::logLine("bye");
    }

    void DebugSession::handleDebugEvents() {
        while (svc::GetDebugEvent(mDebugHandle)
                .map([&](const svc::DebugEventInfo& info) {
                    handleDebugEvent(info);
                })
                .succeeded())
            ;
    }

    void DebugSession::handleDebugEvent(const svc::DebugEventInfo& info) {
        switch (info.type) {
        case svc::DebugEvent_CreateProcess: {
            const auto& createProcess = info.info.create_process;

            HK_ASSERT(mProcessId == createProcess.process_id);
            memcpy(mProcessName, createProcess.name, sizeof(mProcessName));

            if (mProgramId != 0)
                HK_ASSERT(mProgramId == createProcess.program_id);
            mProgramId = createProcess.program_id;

            mState = ProcessState::Running;
            break;
        }
        case svc::DebugEvent_CreateThread:
        case svc::DebugEvent_ExitThread:
            break;
        case svc::DebugEvent_ExitProcess:
            mState = ProcessState::Exited;
            mEventThread->join();
            mDebugHandle = 0;
            break;
        case svc::DebugEvent_Exception:
            handleException(info.info.exception);
            break;
        default:
            break;
        }
    }

    void DebugSession::handleException(const svc::DebugInfoException& info) {
        switch (info.type) {
        case svc::DebugException_UndefinedInstruction:
        case svc::DebugException_InstructionAbort:
        case svc::DebugException_DataAbort:
        case svc::DebugException_AlignmentFault:
        case svc::DebugException_UserBreak:
        case svc::DebugException_UndefinedSystemCall:
        case svc::DebugException_MemorySystemError:
            diag::logLine("ass");
            break;
        case svc::DebugException_DebuggerAttached:
            diag::logLine("attache");
            break;
        case svc::DebugException_BreakPoint:
            diag::logLine("breake");
            break;
        case svc::DebugException_DebuggerBreak:
            diag::logLine("broke");
            break;
        default:
            break;
        }
    }

    Result DebugSession::continueImpl() {
        for (s32 core = 0; core < 4; core++) {
            HK_ABORT_UNLESS_R(svc::SetThreadCoreMask(Handle(svc::PseudoHandle::CurrentThread), core, bit(core)));
            u64 tids[] { 0 };

            Result rc = svc::ContinueDebugEvent(mDebugHandle, svc::ContinueFlag_ContinueAll, tids, 1);
            while (rc == svc::ResultBusy()) {
                svc::DebugEventInfo info = HK_UNWRAP(svc::GetDebugEvent(mDebugHandle));
                // do nothing with it
                rc = svc::ContinueDebugEvent(mDebugHandle, svc::ContinueFlag_ContinueAll, tids, 1);
            }

            HK_TRY(rc);
        }

        return ResultSuccess();
    }

} // namespace hdbg
