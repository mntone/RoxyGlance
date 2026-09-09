#pragma once
#include "Rule.h"

namespace roxyg::settings {

class UserSettings final {
public:
  explicit UserSettings(c4::yml::NodeRef node);

  [[nodiscard]] constexpr std::vector<Rule>& rules() noexcept {
    return rules_;
  }
  [[nodiscard]] constexpr std::vector<Rule> const& rules() const noexcept {
    return rules_;
  }

private:
  c4::yml::NodeRef node_;
  std::vector<Rule> rules_;
};

class UserSettingsDocument final {
public:
  inline UserSettingsDocument() noexcept = default;

  void load(std::string_view yaml);

  [[nodiscard]] constexpr UserSettings* root() noexcept {
    return root_.get();
  }
  [[nodiscard]] constexpr UserSettings const* root() const noexcept {
    return root_.get();
  }

private:
  c4::yml::Tree tree_;
  std::unique_ptr<UserSettings> root_;
};

}
