set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

# Flags CPU - enlever si le compilateur ne supporte pas
set(VCPKG_CXX_FLAGS "-O3 -march=cascadelake -mtune=cascadelake")
set(VCPKG_C_FLAGS "-O3 -march=cascadelake -mtune=cascadelake")

# Désactiver LTO pour isoler les problèmes
# set(VCPKG_LINKER_FLAGS "-flto=auto")

# Ne pas forcer CMAKE_SYSTEM_NAME pour build native
set(VCPKG_CMAKE_SYSTEM_NAME "Linux")
# set(VCPKG_CMAKE_SYSTEM_PROCESSOR "x86_64")