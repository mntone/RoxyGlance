#pragma once

namespace roxyg::utility {

template<typename CB>
class CallbackBase {
public:
  constexpr void addCallback(CB callback, void* context) {
    callbacks_.emplace_back(callback, context);
  }

protected:
  std::vector<std::tuple<CB, void*>> callbacks_;
};

}

