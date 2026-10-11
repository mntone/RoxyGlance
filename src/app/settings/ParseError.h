#pragma once

namespace roxyg::settings {

enum class ParseErrorReason: uint8_t {
  kUnknown = 0,
  kExpectedString,
  kExpectedNumber,
  kInvalidString,
  kInvalidNumber,
  kNumberOutOfRange,
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

  ParseError& setContent(int value) noexcept {
    char* const end{content + sizeof(content) - 1};
    return finalizeContent(::std::to_chars(content, end, value));
  }

  ParseError& setContent(long value) noexcept {
    char* const end{content + sizeof(content) - 1};
    return finalizeContent(::std::to_chars(content, end, value));
  }

  ParseError& setContent(float value) noexcept {
    char* const end{content + sizeof(content) - 1};
    return finalizeContent(::std::to_chars(content, end, value));
  }

private:
  ParseError& finalizeContent(::std::to_chars_result const result) noexcept {
    if (result.ec != std::errc{}) {
      content[0] = '\0';
      assert(false);  // The ParseError content buffer is too small.
    } else {
      *result.ptr = '\0';
    }
    return *this;
  }
};

}
