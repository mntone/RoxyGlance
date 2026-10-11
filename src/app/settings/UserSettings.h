#pragma once
#include "Rule.h"

#include "../logging/Logger.h"

namespace roxyg::settings {

class UserSettings final {
public:
  explicit UserSettings(c4::yml::NodeRef node, logging::Logger* logger);

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
  UserSettingsDocument() noexcept = default;

  void load(std::string_view yaml, logging::Logger* logger = nullptr);

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
