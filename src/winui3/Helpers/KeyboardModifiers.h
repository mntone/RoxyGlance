#pragma once

namespace roxyg::winui3 {

namespace detail {
static ROXYG_ALWAYS_INLINE uint32_t ReadModifierKeyStatesForCurrentThread() {
  using IKS = ::winrt::Microsoft::UI::Input::InputKeyboardSource;
  using VK = ::winrt::Windows::System::VirtualKey;

  uint32_t states = 0;
  states |= static_cast<uint32_t>(IKS::GetKeyStateForCurrentThread(VK::Shift));
  states |= static_cast<uint32_t>(IKS::GetKeyStateForCurrentThread(VK::Control)) << 8;
  states |= static_cast<uint32_t>(IKS::GetKeyStateForCurrentThread(VK::Menu)) << 16;
  states |= (static_cast<uint32_t>(IKS::GetKeyStateForCurrentThread(VK::LeftWindows))
             | static_cast<uint32_t>(IKS::GetKeyStateForCurrentThread(VK::RightWindows))) << 24;
  return states;
}
}

struct KeyboardModifiers final {
  inline KeyboardModifiers()
    : packed_states_(detail::ReadModifierKeyStatesForCurrentThread()) {
  }

  ROXYG_ALWAYS_INLINE bool isNoModifierPressed() const noexcept {
    return (packed_states_ & 0x01010101) == 0;
  }

  ROXYG_ALWAYS_INLINE bool areNonShiftModifiersReleased() const noexcept {
    return (packed_states_ & 0x01010100) == 0;
  }

  ROXYG_ALWAYS_INLINE bool isShiftPressed() const noexcept {
    return (packed_states_ & 0x00000001) == 1;
  }

  ROXYG_ALWAYS_INLINE bool isShiftOnlyPressed() const noexcept {
    return (packed_states_ & 0x01010101) == 1;
  }

  ROXYG_ALWAYS_INLINE bool isCtrlOnlyPressed() const noexcept {
    return (packed_states_ & 0x01010101) == (1 << 8);
  }

  ROXYG_ALWAYS_INLINE bool isAltOnlyPressed() const noexcept {
    return (packed_states_ & 0x01010101) == (1 << 16);
  }

  ROXYG_ALWAYS_INLINE bool isWinOnlyPressed() const noexcept {
    return (packed_states_ & 0x01010101) == (1 << 24);
  }

private:
  uint32_t packed_states_;
  // win[31:24] | alt[23:16] | ctrl[15:8] | shift[7:0]
  // reserved[7:2] | locked[1] | down[0]
  // - CoreVirtualKeyStates::Down   = 1
  // - CoreVirtualKeyStates::Locked = 2
};

}
