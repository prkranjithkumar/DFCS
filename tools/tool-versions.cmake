# Tool version manifest (SRS section 5, NFR-010): single source of truth for pinned versions.
# Values are proposed pins; change them here only, never inline in CI or other CMake files.

set(DFCS_PIN_CMAKE "4.4.4")
set(DFCS_PIN_NINJA "1.13.2")
# Host compiler pin applies to GCC only (WinLibs MinGW-w64 UCRT on Windows); others are reported, not checked.
set(DFCS_PIN_GCC "16.2.0")

# Commit hash is authoritative; the tag is informational.
set(DFCS_PIN_GOOGLETEST_TAG "v1.17.0")
set(DFCS_PIN_GOOGLETEST_COMMIT "52eb8108c5bdec04579160ae17225d66034bd723")

set(DFCS_PIN_CI_PYTHON "3.13.7")
set(DFCS_PIN_WINLIBS_URL "https://github.com/brechtsanders/winlibs_mingw/releases/download/16.2.0posix-14.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-16.2.0-mingw-w64ucrt-14.0.0-r2.zip")
set(DFCS_PIN_WINLIBS_SHA256 "d5dbafc4a170e762ca6143151ec918fb9e2c72736fb14cd704abebc6bdd5276a")
set(DFCS_PIN_GITLEAKS "8.30.1")
set(DFCS_PIN_GITLEAKS_SHA256 "551f6fc83ea457d62a0d98237cbad105af8d557003051f41f3e7ca7b3f2470eb")
set(DFCS_PIN_CODEQL_URL "https://github.com/github/codeql-action/releases/download/codeql-bundle-v2.27.4/codeql-bundle-linux64.tar.gz")
set(DFCS_PIN_CODEQL_SHA256 "89ed56c8ec6bfccef6a0d2e7fe86da0469ca24b4db08b88016ec134b942c17a4")

# Not pinned yet (TBD): Arm GNU toolchain (ST-073), Cppcheck/Clang-Tidy (ST-003), Unity, pytest.
