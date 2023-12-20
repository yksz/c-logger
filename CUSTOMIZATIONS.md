# Customizations

## CMake

The `cmake_minimum_required` was raised from 2.8.11 to 2.8.12
to avoid the "Compatibility with CMake < 2.8.12 will be removed from a future version of
CMake." deprecation warning.

And unsure about the mechanism, but there is also a nice side-benefit that Visual Studio Code
won't complain about `extern "C" {` in the C headers.

## Visual Studio Code

Various files in `.vscode/` have been committed to support easy Visual Studio Code development.

## Android

### Android Development

To develop with Android, download and unpack the Android NDK. For this example I am using
Android NDK 23.1.7779620.

1. Run `CMake: Edit User-Local CMake Kits`.
2. Add an entry like:

   ```json
   {
        "name": "Android NDK 23.1.7779620",
        "compilers": {
            "C": "/some/path/android-sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android31-clang",
            "CXX": "/some/path/android-sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android31-clang++"
        },
        "isTrusted": true
   }
   ```
3. Run `CMake: Select a Kit` and choose your Android NDK.
4. Run `CMake: Configure`.
5. Run `CMake: Build`. The build may show an error that can be ignored:
   > `[proc] The command: x86_64-linux-android31-clang -v failed with error: Error: spawn x86_64-linux-android31-clang ENOENT`

The last three (3) commands you could do from the UI (both the toolbar and the CMake Panel).
