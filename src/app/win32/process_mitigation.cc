#include "pch.h"
#include "process_mitigation.h"

BOOL roxyg::win32::ConfigureProcessMitigations() noexcept {
  BOOL rc;

  // Block dynamic code generation (ACG).
  PROCESS_MITIGATION_DYNAMIC_CODE_POLICY dynamic_code_policy{
    0x1  /* ProhibitDynamicCode = 1 */
  };
  rc = SetProcessMitigationPolicy(
    ProcessDynamicCodePolicy,
    &dynamic_code_policy,
    sizeof(PROCESS_MITIGATION_DYNAMIC_CODE_POLICY)
  );
  if (rc == FALSE) {
    return rc;
  }

  // Treat invalid handle operations as fatal.
  PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY handle_check_policy{
    0x3  /*
          * RaiseExceptionOnInvalidHandleReference = 1,
          * HandleExceptionsPermanentlyEnabled     = 1,
          */
  };
  rc = SetProcessMitigationPolicy(
    ProcessStrictHandleCheckPolicy,
    &handle_check_policy,
    sizeof(PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY)
  );
  if (rc == FALSE) {
    return rc;
  }

  // Block FSCTL system calls.
  PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY system_call_disable_policy{
    0x4  /* DisallowFsctlSystemCalls = 1 */
  };
  rc = SetProcessMitigationPolicy(
    ProcessSystemCallDisablePolicy,
    &system_call_disable_policy,
    sizeof(PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY)
  );
  if (rc == FALSE) {
    return rc;
  }

  // Block non-system fonts.
  PROCESS_MITIGATION_FONT_DISABLE_POLICY font_disable_policy{
    0x1  /* DisableNonSystemFonts = 1 */
  };
  rc = SetProcessMitigationPolicy(
    ProcessFontDisablePolicy,
    &font_disable_policy,
    sizeof(PROCESS_MITIGATION_FONT_DISABLE_POLICY)
  );
  if (rc == FALSE) {
    return rc;
  }

  // Block remote and low-integrity images.
  PROCESS_MITIGATION_IMAGE_LOAD_POLICY image_load_policy{
    0x3  /* NoRemoteImages = 1, NoLowMandatoryLabelImages = 1 */
  };
  rc = SetProcessMitigationPolicy(
    ProcessImageLoadPolicy,
    &image_load_policy,
    sizeof(PROCESS_MITIGATION_IMAGE_LOAD_POLICY)
  );

  return rc;
}
