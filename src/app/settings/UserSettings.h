#pragma once
#include "Rule.h"
#include "BasicSettingsStore.h"

#include "../logging/Logger.h"

namespace roxyg::settings {

class UserSettings final {
public:
  explicit UserSettings(c4::yml::NodeRef node, logging::Logger* logger);

  [[nodiscard]] constexpr Rules& rules() noexcept {
    return rules_;
  }
  [[nodiscard]] constexpr Rules const& rules() const noexcept {
    return rules_;
  }

private:
  c4::yml::NodeRef node_;
  Rules rules_;
};

class UserSettingsDocument final {
public:
  UserSettingsDocument() noexcept = default;

  void parse(std::string_view yaml, logging::Logger* logger = nullptr);
  void reset() noexcept;
  std::string str() const;

  [[nodiscard]] constexpr UserSettings* root() noexcept {
    return root_.get();
  }
  [[nodiscard]] constexpr UserSettings const* root() const noexcept {
    return root_.get();
  }

  static std::wstring_view fileName() noexcept;

private:
  c4::yml::Tree tree_;
  std::unique_ptr<UserSettings> root_;
};

class UserSettingsStore final: public BasicSettingsStore<UserSettingsDocument> {
};

}
