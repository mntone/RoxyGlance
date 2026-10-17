#include "pch.h"
#include "RuleEditorControl.xaml.h"
#if __has_include("Controls/RuleEditorControl.g.cpp")
#include "Controls/RuleEditorControl.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Shell;
}

namespace winrt::Mntone::RoxyGlance::Controls::implementation {

DependencyProperty RuleEditorControl::RuleProperty_{
  DependencyProperty::Register(
    L"Rule",
    xaml_typename<RuleViewModel>(),
    xaml_typename<Controls::RuleEditorControl>(),
    PropertyMetadata{nullptr}
  )
};
DependencyProperty RuleEditorControl::MonitorsProperty_{
  DependencyProperty::Register(
    L"Monitors",
    xaml_typename<MonitorsViewModel>(),
    xaml_typename<Controls::RuleEditorControl>(),
    PropertyMetadata{nullptr}
  )
};

}
