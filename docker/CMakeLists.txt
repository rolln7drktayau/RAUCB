cmake_minimum_required(VERSION 3.14)

project(client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trouver les packages Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Network Concurrent Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Network Concurrent Widgets)

# Module core-private si disponible
set(CORE_PRIVATE_FOUND FALSE)
if(${QT_VERSION_MAJOR} EQUAL 6)
    find_package(Qt6 COMPONENTS CorePrivate)
    if(Qt6CorePrivate_FOUND)
        set(CORE_PRIVATE_FOUND TRUE)
    endif()
else()
    find_package(Qt5 COMPONENTS CorePrivate)
    if(Qt5CorePrivate_FOUND)
        set(CORE_PRIVATE_FOUND TRUE)
    endif()
endif()

# Définir les sources
set(SOURCES
    main.cpp
    mainwindow.cpp
    network/networkmanager.cpp
    security/encryption.cpp
    security/tlsmanager.cpp
)

# Définir les headers
set(HEADERS
    mainwindow.h
    network/networkmanager.h
    security/encryption.h
    security/tlsmanager.h
)

# Définir les fichiers UI
set(FORMS
    mainwindow.ui
)

# Trouver OpenSSL - adapter pour la cross-compilation
if(UNIX AND NOT APPLE)
    # Configuration pour Linux cross-compilation
    find_package(OpenSSL)
    if(NOT OpenSSL_FOUND)
        # Chemin pour le sysroot dans l'environnement Docker
        set(OPENSSL_INCLUDE_DIR "/build/sysroot/usr/include")
        set(OPENSSL_SSL_LIBRARY "/build/sysroot/usr/lib/aarch64-linux-gnu/libssl.so")
        set(OPENSSL_CRYPTO_LIBRARY "/build/sysroot/usr/lib/aarch64-linux-gnu/libcrypto.so")
    endif()
else()
    # Configuration pour Windows
    find_package(OpenSSL)
    if(NOT OpenSSL_FOUND)
        set(OPENSSL_INCLUDE_DIR "C:/Program Files/OpenSSL-Win64/include")
        set(OPENSSL_SSL_LIBRARY "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD/libssl.lib")
        set(OPENSSL_CRYPTO_LIBRARY "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD/libcrypto.lib")
    endif()
endif()

# Ajouter les include paths
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${OPENSSL_INCLUDE_DIR})

# Créer l'exécutable
add_executable(client
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

# Lier avec les bibliothèques Qt
target_link_libraries(client PRIVATE 
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OPENSSL_SSL_LIBRARY}
    ${OPENSSL_CRYPTO_LIBRARY}
)

# Ajouter CorePrivate seulement s'il est disponible
if(CORE_PRIVATE_FOUND)
    target_link_libraries(client PRIVATE Qt${QT_VERSION_MAJOR}::CorePrivate)
    target_compile_definitions(client PRIVATE HAVE_QT_CORE_PRIVATE)
endif()

# Copier les DLLs pour Windows
if(WIN32)
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Core>
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Gui>
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Network>
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Concurrent>
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Widgets>
            $<TARGET_FILE_DIR:client>
    )
endif()