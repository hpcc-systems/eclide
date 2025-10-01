# ECL IDE Development Guide

## Project Overview

ECL IDE is a Windows-based **Win32 desktop application** for developing ECL (Enterprise Control Language) code for HPCC Systems clusters. Built using **WTL/ATL** (Windows Template Library) and **MFC** on top of the **Scintilla** editor, it provides a full-featured IDE with syntax highlighting, repository management, and workunit execution capabilities.

## Architecture

### Core Components

- **`eclide/`**: Main IDE application with MDI (Multiple Document Interface) frames, views, and dialogs
- **`wlib/`**: Reusable UI library containing repository/workspace tree views, syntax markers, and editor commands
- **`comms/`**: SOAP-based communication layer generated via gSOAP for HPCC ESP services
- **`EclEditor/`**: Custom Scintilla integration with ECL/SALT/KEL/ESDL/DUD lexers
- **`clib/`**: Core utility library with cache management, threading, and data structures
- **`EclLib/`**: ECL-specific parsing and metadata handling

### Communication Layer Pattern

The `comms/` directory contains **version-specific** SOAP proxy implementations (e.g., `3_08/`, `68200/`, `6_4_0/`). These are **auto-generated** from WSDL files using gSOAP:

```bash
# Generate SOAP stubs from WSDL (example from comms/6_4_0/gsoap_build.bat)
wsdl2h.exe -o Services.h WsAttributes.wsdl WsDfu.wsdl WsSMC.wsdl
soapcpp2.exe -C -x -i Services.h
```

All communication uses the `CSoapInitialize<T>` template wrapper (see `comms/gSoapUtil.h`) which adds authentication, timeout handling, and ClientID tagging.

### Smart Pointer Conventions

This codebase uses **three** smart pointer systems simultaneously:

1. **`StlLinked<T>`** (preferred): Custom ref-counted smart pointer for cached objects
2. **`CComPtr<T>`**: ATL/WTL COM-style smart pointer for UI tree nodes
3. **`boost::shared_ptr<T>`**: Used in some legacy threading code

When implementing interfaces (e.g., `IRepository`, `IAttribute`), objects are typically stored in `CacheT<>` containers and returned as `StlLinked<>` pointers.

## Build System

### vcpkg Integration

All dependencies are managed via **vcpkg** as a git submodule. The build uses a **custom bootstrap mechanism** that checks `vcpkg/scripts/vcpkgTools.xml` hash to determine if bootstrap is needed (see `CMakeLists.txt` lines 10-33).

**Critical vcpkg paths** (configured in root `CMakeLists.txt`):
- `VCPKG_FILES_DIR`: Build-folder-relative (not source-relative!)
- `VCPKG_INSTALLED_DIR`: `${VCPKG_FILES_DIR}/vcpkg_installed`

### Build Configuration

**Target Platform**: Win32 (x86) only - this is a 32-bit application

**Standard build workflow**:
```bash
mkdir build && cd build
cmake .. -A Win32                    # Or -G "Visual Studio 17 2022" -A Win32
cmake --build . --config RelWithDebInfo --parallel
cmake --build . --config RelWithDebInfo --target package  # Creates NSIS installer
```

**Supported configurations**: Debug, Release, RelWithDebInfo, MinSizeRel

### Precompiled Headers

Nearly every project uses MSVC precompiled headers via `ADD_MSVC_PRECOMPILED_HEADER()` macro:
- Header file: `stdafx.h` (includes common WTL/ATL/Boost headers)
- Source file: `stdafx.cpp` (empty file with `/Yc` flag)
- All other sources: Auto-prefixed with `/Yu` and `/FI` flags

When adding new `.cpp` files, ensure they're added to the source list AFTER the precompiled header setup in `CMakeLists.txt`.

## Language-Specific Features

### Custom Scintilla Lexers

ECL IDE includes **custom lexers** for HPCC-specific languages (see `EclEditor/Catalogue.cxx`):
- **lmECL**: ECL language
- **lmSALT**: SALT (Scalable Automated Linking Technology)
- **lmKEL**: KEL (Key Enforcement Language)
- **lmESDL**: ESDL (Enterprise Services Definition Language)
- **lmDUD**: DUD (Data Unpacking Definition)

These lexers are **statically linked** via `LINK_LEXER()` macro to prevent linker stripping.

### Syntax Error Handling

The `CSciSyntaxMarker` mixin (in `wlib/SciSyntaxMarker.h`) provides unified error annotation:
- Stores errors in `CSyntaxErrorVector` with line/column info
- Displays markers (MARKER_ERROR, MARKER_WARNING, MARKER_ARROW)
- Provides navigation (SyntaxSelNext/Previous)
- Integrates with `SyntaxView` for error list display

Syntax checking is **asynchronous** via `clib::thread` and communicates back via `CWM_SUBMITDONE` message.

## Common Patterns

### Repository/Workspace Tree Nodes

All tree nodes inherit from `CTreeNode` (in `wlib/TreeNode.h`) which wraps `WTL::CTreeItem`:
- **Repository trees**: `CRepositoryNode` → `CModuleNode` → `CAttributeNode`
- **Workspace trees**: `CWorkspaceNode` → `CBuilderWindowNode` → `CAttributeWindowNode`

Nodes implement **slot patterns** (`IRepositorySlot`, `IWorkspaceSlot`) for callbacks.

### ECL Command Mixin

The `CEclCommandMixin<T>` (in `wlib/EclCommand.h`) provides reusable command handlers:
- Syntax checking: `DoCheckSyntax()` → `EclCheckSyntax()` (threaded)
- Error navigation: `OnEclNextError()`, `OnEclPreviousError()`
- Find/replace: `DoEditFind()`, `DoEditFindReplace()`

Integrate via: `class MyView : public CEclCommandMixin<MyView>`

### Cache Pattern

Objects implementing `GetCacheID()` can be cached in `CacheT<KeyType, ObjectType>`:
```cpp
CacheT<CString, CAttribute> m_cache;
StlLinked<CAttribute> attr = m_cache.Get(new CAttribute(...));  // Deduplicates automatically
```

## Testing & Debugging

**No automated tests** exist in this codebase. Testing is manual via:
1. Building in Debug configuration
2. Running `eclide.exe` from `build/bin/Debug/`
3. Using BugTrap for crash reporting (integrated in all builds)

**Common debug commands**:
- Set breakpoints in `wlib/` for editor interactions
- Check `comms/` layer for ESP communication issues
- Monitor `SyntaxView` for error display problems

## Version Information

Version is defined in `version.cmake`:
- `HPCC_MAJOR`, `HPCC_MINOR`, `HPCC_POINT`: Semantic version
- `HPCC_MATURITY`: "release" or other (affects build number)
- `HPCC_SEQUENCE`: Build sequence (auto-incremented)

The `BUILD_TAG` macro is injected into all SOAP requests as ClientID for server-side tracking.

## Key Files to Understand

- `CMakeLists.txt`: vcpkg bootstrap, compiler flags, precompiled headers
- `comms/CMakeLists.txt`: gSOAP code generation from WSDL
- `wlib/RepositoryTreeNode.h`: Core repository object model
- `wlib/EclCommand.h`: Reusable ECL editor commands
- `clib/Cache.h`: Smart pointer caching pattern
- `EclEditor/Catalogue.cxx`: Lexer registration

## Common Gotchas

1. **Always use Win32 target** - x64 builds are not supported
2. **vcpkg paths are build-relative** - don't assume source-relative paths
3. **Three smart pointer types** - be mindful when converting between them
4. **Async syntax checks** - use message-based completion, not direct returns
5. **MSVC precompiled headers** - always include stdafx.h first (enforced by /FI)
6. **gSOAP namespaces** - each ESP service version has its own namespace (e.g., `WsAttributes`, `WsSMC`)
