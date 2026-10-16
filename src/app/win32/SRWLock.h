#pragma once

#ifdef __INTELLISENSE__
#include <synchapi.h>

#ifdef _DEBUG
#include <cassert>
#include <processthreadsapi.h>
#endif
#endif

namespace roxyg::win32 {

class SRWLock final {
  SRWLock(SRWLock const&) = delete;
  SRWLock& operator=(SRWLock const&) = delete;

public:
  inline SRWLock() noexcept
    : lock_(SRWLOCK_INIT)
#ifdef _DEBUG
    , owner_(0)
#endif
  {
  }

  void lock() noexcept {
    AcquireSRWLockExclusive(&lock_);

#ifdef _DEBUG
    owner_ = GetCurrentThreadId();
#endif
  }

  void unlock() noexcept {
#ifdef _DEBUG
    assert(owner_ == GetCurrentThreadId());
    owner_ = 0;
#endif

    ReleaseSRWLockExclusive(&lock_);
  }

private:
  SRWLOCK lock_;

#ifdef _DEBUG
  DWORD owner_;
#endif
};

}
