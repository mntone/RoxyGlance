#pragma once

namespace roxyg::settings {

enum class ParseErrorReason: uint8_t {
  kUnknown = 0,
};

enum class KeyId: uint8_t {
  kNone = 0,
  kName,
  kWhen,
  kWhere,
  kThen,

  kProcessImageName = 11,
  kWindowClass,
  kWindowTitle,

  kActionType = 21,
  kPositionX,
  kPositionY,
  kWidth,
  kHeight,
  kMonitorId,
  kMonitorName,
};

struct ParseError final {
  ParseErrorReason reason : 3;
  KeyId key_id : 5;
  char content[15];
};

}
