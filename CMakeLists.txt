project(kdevverapp)

cmake_minimum_required(VERSION 2.8.12)

if (NOT ${CMAKE_VERSION} VERSION_LESS "3.3.0")
    cmake_policy(SET CMP0063 NEW)
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 90)
set(VERSION_PATCH 90)
# KDevplatform dependency version
set(KDEVPLATFORM_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

find_package (ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMAddTests)
include(FeatureSummary)

find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)
find_package(KF5 REQUIRED COMPONENTS ItemModels)
find_package(Qt5 REQUIRED Core Widgets Test)

include_directories(
    ${kdevverapp_BINARY_DIR}
    ${kdevverapp_SOURCE_DIR}
    ${kdevverapp_SOURCE_DIR}/rules
)

set(kdevverapp_SRCS
    plugin.cpp
    problemmodel.cpp

    config/globalconfigpage.cpp
    config/projectconfigpage.cpp
)

ki18n_wrap_ui(kdevverapp_SRCS
    config/globalconfigpage.ui
    config/projectconfigpage.ui
)

qt5_add_resources(kdevverapp_SRCS
    kdevverapp.qrc
)

kconfig_add_kcfg_files(kdevverapp_CONFIG_SRCS
    config/globalsettings.kcfgc
    config/projectsettings.kcfgc
)

add_library(kdevverapp_core STATIC
    debug.cpp
    job.cpp
    parameters.cpp
    utils.cpp

    rules/rules.cpp
    rules/rules_db.cpp

    ${kdevverapp_CONFIG_SRCS}
)

target_link_libraries(kdevverapp_core
    KDev::Project
    KDev::Language
    KDev::Shell
)

kdevplatform_add_plugin(kdevverapp
    JSON kdevverapp.json
    SOURCES ${kdevverapp_SRCS}
)

target_link_libraries(kdevverapp
    kdevverapp_core
)

add_subdirectory(tests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
