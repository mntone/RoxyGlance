# AI Agent Guidelines (C++ / Windows Native & WinUI 3)

This repository contains C++ projects targeting Windows (Win32, WinRT, and WinUI 3).  
All AI coding assistants (GitHub Copilot, Cursor, Claude Code, etc.) MUST strictly 
adhere to the following rules.

## 1. Primary Objectives & Communication
- **Artifacts & Public Content**: MUST be written strictly in **English**:
  - Code comments (C++, XAML, etc.)
  - Git commit messages and PR titles/descriptions
  - API documentation, inline docs, and logs

  *(Note: Interactive conversation/chat responses can be in any language requested 
  by the user's environment.)*
- **Documentation Comment Style**:
  - Use **XML documentation comments** (`/// <summary>...</summary>`) for function 
    and API documentation (IntelliSense compatible). Do NOT use Doxygen-style 
    (`@brief`, `/** */`).
  - Example:
    ```cpp
    /// <summary>
    /// Initializes the application window with specified parameters.
    /// </summary>
    /// <param name="width">The initial width of the window.</param>
    /// <returns>S_OK on success, or an HRESULT error code.</returns>
    HRESULT InitializeWindow(int width) noexcept;
    ```

## 2. C++ & Architecture Standards

### C++ Language Level & Feature Availability
- **Base Standard**: Target **C++20** compatibility by default.
- **C++23 Usage**: Optional C++23 features MUST be guarded with feature-test macros 
  (e.g., `#if defined(__cpp_attribute_assume) && __cpp_attribute_assume >= 202207L`).
  Always provide a fallback path compatible with C++20.

### Type Explicitly (`auto` Usage)
- **Avoid Overusing `auto`**: Prefer explicit types for variables, parameter types,
  and function returns to maintain clarity.
- **Allowed `auto` Exceptions**:
  - Complex template types (e.g., iterators, lambda types, complex `std::ranges`
    view chains).
  - When repeating the type creates obvious redundancy
    (e.g., `auto ptr = std::make_unique<MyClass>();`).

### Modern Features & Binary Size Optimization
- **Active Modernization**: Actively use `constexpr`, `concepts`, and modern 
  standard library algorithms for compile-time computation and safety.
- **No C-Style Casts**: Avoid C-style casts (e.g., `(int)x` or `(HWND)ptr`).
  Explicitly use C++ named casts (`static_cast`, `reinterpret_cast`, `const_cast`)
  or `winrt::cast_to`.
- **Prevent Binary Bloat**:
  - Avoid overly complex `std::ranges` or template chains if they cause code 
    bloat/binary inflation.
  - Prefer non-template monolithic helper functions or common base implementations 
    where possible, rather than instantiating heavy templates for every type variation.
  - Keep binary size small and compile times reasonable.

### Memory & Resource Management
- Strictly follow RAII for all resource handling (handles, COM/WinRT objects, locks).
- Avoid naked `new`/`delete` or raw owning pointers. Favor stack allocation and 
  lightweight RAII wrappers.
- **Allocation Failure Strategy**:
  - For small/critical object allocation failures where recovery is impossible,
    log the error and invoke **Fast Fail** via `roxyg::utility::fastfail()`.
  - For large/recoverable allocations, map failures directly to `E_OUTOFMEMORY`
    (HRESULT).

### Error Handling (Exception-Free & HRESULT-Driven)
- **No Exception Propagation**: The codebase operates in an **exception-free** 
  API design. Do NOT allow C++ exceptions (`std::exception`) to propagate across 
  function boundaries.
- **`try-catch` for HRESULT Conversion**: Catching exceptions via `try-catch` 
  IS allowed exclusively to convert unexpected runtime exceptions into 
  `HRESULT` values for error handling used for error handling, such as logging 
  or returning them from an API.
- **Aggressive `noexcept` Usage**: Mark functions (especially destructors, move 
  constructors, swap functions, and internal helpers) as `noexcept` whenever
  possible to minimize binary size and allow aggressive compiler optimizations.
- **HRESULT as Primary Error Type**: Use `HRESULT` or `winrt::hresult` as the 
  return type for error propagation across API boundaries.
- **Win32 / WinRT Integration**: Process system errors using `HRESULT` /
  `winrt::hresult` and Win32 error macros without letting exceptions propagate.

## 3. Windows / Win32 / WinRT / WinUI 3 Conventions
- **WinRT & C++/WinRT**:
  - Prefer C++/WinRT headers (`winrt/Windows...`) over older C++/CX or raw 
    COM interfaces.
  - Use `winrt::com_ptr<T>` for managing raw COM/WinRT interface pointers.
  - **Asynchronous Operations**: Handle async tasks pragmatically (callbacks, 
    handlers, or C++20 coroutines like `co_await` / `winrt::fire_and_forget` 
    when genuinely necessary). Do not force coroutines where simpler patterns 
    suffice.

- **WinUI 3 & MVVM / Code-Behind Strategy**:
  - **Pragmatic MVVM**: Prefer implementing UI logic in ViewModel classes 
  where practical.
  - **Code-Behind Allowance**: Code-behind is fully acceptable when pure MVVM 
    in C++/WinRT would require excessive IDL/boilerplate overhead or create 
    unneeded complexity (e.g., complex event handlers, direct control manipulation).
  - Prioritize developer velocity and code simplicity over dogmatic 
    code-behind-less MVVM.

- **Win32 API & WIL (Windows Implementation Libraries)**:
  - **Pragmatic Resource Wrapping**: Use RAII wrappers for raw Win32 handles
    (custom RAII smart handles or WIL where appropriate).
  - **Direct API Preference**: Prefer direct Win32/COM API calls over WIL 
    helpers if WIL abstractions interfere with fine-grained control
    (e.g., custom timeouts, retry logic, or error handling).

## 4. Code Style & Formatting
The codebase generally follows the **Google C++ Style Guide** with custom 
naming conventions. Adhere strictly to the following rules:

### Indentation & Spacing
- **Indent**: Use **2 spaces** for code blocks (no tabs).
- **Access Specifiers**: Place access specifiers (`public:`, `protected:`,
  `private:`) at the **same indentation level as the class declaration**
  (0 spaces / no extra indent).

### Const Placement (East Const)
- **Use East Const**: Always place the `const` qualifier **after** the type
  (e.g., `WindowManager const&`, `WindowState const*`). Do NOT use West Const
  (`const WindowManager&`).

### Naming Conventions
- **Types / Classes / Structs**: `PascalCase` (e.g., `WindowManager`)
- **Interfaces**: `IPascalCase` with an `I` prefix (e.g., `INotifyPropertyChanged`,
  `IAppService`)
- **Template-only Types / Structs**: `snake_case` (e.g., `type_traits_helper`,
  `buffer_view`)
- **Member Variables**: 
  - Standard variables: `snake_case_and_underscore_` with a trailing underscore
    (e.g., `user_name_`, `is_initialized_`).
  - **WinRT / ViewModel 1:1 Property Backing Variables**: Private member variables 
    that map directly 1:1 to a `PascalCase` WinRT/XAML property MUST match the 
    property's name in `PascalCase` with a trailing underscore (e.g., `ViewModel_` 
    for property `ViewModel()`, `Title_` for property `Title()`).
- **Global / Free Functions**: `PascalCase` (e.g., `InitializeApplication()`)
  - *Exception*: Low-level system-like utility functions may use `snake_case`
    (e.g., `roxyg::utility::fastfail()`).
- **Member Functions & Getters/Setters**:
  - **Internal / C++ Implementation**: Use `camelCase` for member functions and 
    getters (e.g., `processData()`, `userName()`), and `setPascalCase` for 
    setters (e.g., `setUserName()`).
  - **WinRT Projected APIs & ViewModels (MIDL / XAML Binding)**: Functions, 
    properties, and methods exposed via MIDL (`.idl`) or mapped for XAML bindings 
    MUST use **`PascalCase`** (e.g., `UserName()`, `SetUserName()`, `MyCommand()`) 
    to match Windows Metadata (.NET) conventions.
- **Local Variables & Parameters**: `snake_case` (e.g., `index`, `item_count`)
- **Constants**: `kPascalCase` with a `k` prefix (e.g., `kMaxBufferLength`, `kDefaultTimeoutMs`)

### Class Member Layout & Structure
Class members MUST be declared strictly in the following top-to-bottom order:
1. **NotCopyable Declarations**: If making a class non-copyable, place deleted 
   copy constructors/assignment operators at the **very top of the class definition**
   before any explicit access specifiers (in the implicit private region).
2. **Constructors**
3. **Destructor**
4. **Member Functions**
5. **Accessors** (Getters/Setters)
6. **Member Variables**: Always precede member variables with an explicit `private:`
   access specifier, **even if the preceding section was already `private:`**.

#### Example Class Layout:
```cpp
class WindowManager {
  // 1. NotCopyable (before any public/private specifiers)
  WindowManager(WindowManager const&) = delete;
  WindowManager& operator=(WindowManager const&) = delete;

public:
  // 2. Constructors
  WindowManager() noexcept;

  // 3. Destructor
  ~WindowManager() noexcept;

  // 4. Member Functions
  HRESULT initialize() noexcept;

private:
  // 4. Private Member Functions
  void internalCleanup() noexcept;

// 5. Accessors
public:
  constexpr bool isInitialized() const noexcept { return is_initialized_; }

  // WinRT Projected Property
  constexpr winrt::MainViewModel ViewModel() const noexcept { return ViewModel_; }

// 6. Member Variables (Explicitly re-declare private: here)
private:
  // Standard C++ internal member variable
  bool is_initialized_;

  // 1:1 backing variable for WinRT PascalCase property
  winrt::MainViewModel ViewModel_;
};
```

### Formatting Note
- Keep formatting clean and consistent even without an explicit `.clang-format` 
  file in the repository.

## 5. Build, Verification & Static Analysis
Before concluding tasks or finalizing code modifications, ensure the code compiles 
cleanly with zero warnings:

- **Build Tool**: Use **MSBuild** (`MSBuild.exe` / `msbuild`) targeting Visual Studio 
  solution files (**`.slnx`**) or project files (`.vcxproj`). Do NOT use CMake.
- **Compiler Warning Levels**:
  - **App & WinUI 3 Projects**: Code MUST compile cleanly under MSVC compiler 
    warning level **`/W4`**.
  - **Test Projects**: Code MUST compile under MSVC compiler warning level 
    **`/W3`**.
- **Warnings treated as Errors**: The App and WinUI 3 projects enforce strict 
  warnings-as-errors for specific MSVC warning IDs (`TreatSpecificWarningsAsErrors`).
  Ensure code passes without triggering any of them:
  - **C4062**, **C4244**, **C4263**, **C4264**, **C4265**, **C4311**, **C4312**,
    **C4640**, **C4826**, **C4905**, **C4906**, **C5038**
  - *(Key coverage: enum switch completeness, narrowing conversions, virtual 
    function overrides, missing virtual destructors [C4265], pointer conversions,
    thread-safe static initialization, wide string cast safety, and member 
    initialization order [C5038]).*

## 6. Git & Commit Conventions
When generating commit messages or PR descriptions:
- **Format**: Follow [Conventional Commits](https://www.conventionalcommits.org/) 
  format with mandatory/preferred scopes:
  - `<type>(<scope>): <short description>`
- **Scope Conventions**:
  - **App Library**: Use the namespace after `roxyg::` (e.g., `feat(engine): ...`,
    `fix(win32): ...`).
  - **WinUI 3 Project**: Use `winui3` as the scope (e.g., `feat(winui3): ...`).
- **Multiple Changes**: If a commit addresses **two or more distinct items**,
  list them in the commit body as bullet points.  
  Example:
  ```text
  feat(winui3): update navigation bar and title bar styling

  - add custom Acrylic background to navigation view
  - fix alignment issue on title bar controls
  ```
- **Language**: Strictly use **English** for all Git commit messages and PR descriptions.
- **Style Constraints**:
  - Use imperative mood in the subject line (e.g., "add" instead of "added").
  - Start bullet points in the commit body with **lowercase** letters.
  - Keep the subject line under 72 characters.
- **Atomic Commits**: Ensure commits are atomic and focus on a single logical change.
