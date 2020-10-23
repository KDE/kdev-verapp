cmake_minimum_required(VERSION 3.0)

project(kdevverapp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package (ECM "5.14.0" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

set(QT_MIN_VERSION "5.5.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets Test)
set(KF5_DEP_VERSION "5.15.0")
find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS I18n)
set(KDEVPLATFORM_DEP_VERSION "5.2.40")
find_package(KDevPlatform ${KDEVPLATFORM_DEP_VERSION} CONFIG)
set_package_properties(KDevPlatform PROPERTIES
    TYPE REQUIRED
)

include_directories(
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

ecm_qt_declare_logging_category(kdevverapp_LOG_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_VERAPP
    CATEGORY_NAME "kdevelop.analyzers.verapp"
)

kconfig_add_kcfg_files(kdevverapp_CONFIG_SRCS
    config/globalsettings.kcfgc
    config/projectsettings.kcfgc
)

add_library(kdevverapp_core STATIC
    job.cpp
    parameters.cpp
    utils.cpp

    rules/rules.cpp
    rules/rules_db.cpp

    ${kdevverapp_CONFIG_SRCS}
    ${kdevverapp_LOG_SRCS}
)

target_link_libraries(kdevverapp_core
    KDev::Project
    KDev::Language
    KDev::Shell
    KF5::I18n
)

kdevplatform_add_plugin(kdevverapp
    JSON kdevverapp.json
    SOURCES ${kdevverapp_SRCS}
)

target_link_libraries(kdevverapp
    kdevverapp_core
)

add_subdirectory(tests)

# kdebugsettings file
install(FILES kdevverapp.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
