#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <format>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <string_view>
using namespace std;
namespace fs = std::filesystem;

/****************************************************************************************************
File Name : CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(t1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(t1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        myRes.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET t1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(t1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(t1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(t1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.t1)
endif()
set_target_properties(t1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS t1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(t1)
endif()

****************************************************************************************************/

int main(int argc, char* argv[], char* env[])
{
    if ( argc < 2 ) {
        cout << "[ERROR] : Missing arguments. " << endl
             << "Usage : qtprj2cmake <path>" << endl;
        return -1;
    }

    /*
       BuildTree
       CommonResource
       ConfigFileParser
       KeyboardDetech
       MultiThread
       i18n_In_Qt
       Lua-In-Qt
       LuaStackSimulator
       Code-Execute
       Fn4Dir
    */

    const vector<string> ignoreDirs {
       "BuildTree",
       "CommonResource",
       "ConfigFileParser",
       "KeyboardDetech",
       "MultiThread",
       "i18n_In_Qt",
       "Lua-In-Qt",
       "LuaStackSimulator",
       "Code-Execute",
       "Fn4Dir"
    };

    const vector<string> keepedFilesExt {
        ".cpp",
        ".h",
        ".ui",
        ".qrc",
    };


    const string targetExt(".pro");
    fs::path dir(".");
    vector<fs::path> projectsPath;
    //   Find *.pro files 
    for ( auto it  = fs::recursive_directory_iterator(dir); 
               it != fs::recursive_directory_iterator();
             ++it ) 
    {
        const auto& entry = *it;

        auto iDepthCnt = it.depth(); 
        ++iDepthCnt; (void)iDepthCnt;
        if (  entry.is_directory() ) {

        } else if ( entry.is_regular_file() ) {
            // cout << string(iDepthCnt,'-') << " files : " << entry.path().string() << endl;
            if ( entry.path().extension().string() == targetExt ) {
                auto p_path = entry.path().parent_path();

                auto bIsMatched = false;
                for( const auto& ignorePath : ignoreDirs ) {
                    auto s = p_path.string();
                    auto pos = s.find( ignorePath );
                    if ( pos != string::npos ) {
                        bIsMatched = true;
                        break;
                    }
                }

                if ( !bIsMatched ) {
                    projectsPath.push_back( p_path );
                }
            }
        }
    }
    cout << "size = " << projectsPath.size() << endl;

    unordered_map<string, vector<fs::path> > collections;
    // int idx = 1;
    for ( auto it = projectsPath.begin(); it != projectsPath.end(); ++it ) {
        int maxDepth = 0;
        for ( auto it1  = fs::recursive_directory_iterator( *it ); 
                   it1 != fs::recursive_directory_iterator();
                 ++it1 ) 
        {
            const auto& entry = *it1;
            auto iDepthCnt = it1.depth();
            if ( iDepthCnt > maxDepth ) {
                maxDepth = iDepthCnt;
            }

            if ( entry.is_regular_file() && maxDepth == 0 ) {
                auto strPath = it->string();
                auto fileNamePath = entry.path().filename();
                auto extName = entry.path().extension();

                auto bShouldKeep = false;
                for( const auto& ext : keepedFilesExt )  {
                    if ( ext == extName.string() ) {
                        bShouldKeep = true;
                        break;
                    }
                }

                if ( bShouldKeep ) {
                    auto foundIt = collections.find( strPath );
                    if ( foundIt == collections.end() ) {
                        collections[strPath] = vector<fs::path>{ fileNamePath  };
                    } else {
                        foundIt->second.push_back( fileNamePath );
                    }
                }

            }
        }
    }


    // idx = 1;
    for( const auto& pr : collections ) {
        auto newFileName = pr.first + string("/CMakeLists.txt");
        auto projectName = pr.first.substr( pr.first.rfind('/') + 1 );
        // cout << idx++ << ". " << pr.first.substr( pr.first.rfind('/') + 1 ) << endl;
        // int fIdx = 1;

        const string cstr_space8(8, ' ');

        string cpps; 
        string hs;
        string uis;
        string qrcFile;

        for( const auto& filepath : pr.second ) {
            // cout << "\t" << fIdx++ << ". " <<  filepath << endl;
            auto strExt = filepath.extension().string();
            if ( strExt ==  keepedFilesExt[0] ) { // cpp
                cpps += cstr_space8;
                cpps += filepath.string();
                cpps += "\n";
            } else if ( strExt ==  keepedFilesExt[1] ) { // h
                hs += cstr_space8;
                hs += filepath.string();
                hs += "\n";
            } else if ( strExt ==  keepedFilesExt[2] ) { // ui
                uis += cstr_space8;
                uis += filepath.string();
                uis += "\n";
            } else if ( strExt ==  keepedFilesExt[3] ) { // qrc
                qrcFile += cstr_space8;
                qrcFile = filepath.string();
            }
        }

        ofstream cmake_file(newFileName.c_str() , ios::out | ios::trunc);

        const string_view formater_str = R"xyz(
cmake_minimum_required(VERSION 3.16)

project({} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${{QT_VERSION_MAJOR}} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
{}
{}
{}
)

if(${{QT_VERSION_MAJOR}} GREATER_EQUAL 6)
    qt_add_executable({}
        MANUAL_FINALIZATION
        ${{PROJECT_SOURCES}}
        {}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET {} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${{CMAKE_CURRENT_SOURCE_DIR}}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library({} SHARED
            ${{PROJECT_SOURCES}}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${{CMAKE_CURRENT_SOURCE_DIR}}/android")
    else()
        add_executable({}
            ${{PROJECT_SOURCES}}
        )
    endif()
endif()

target_link_libraries({} PRIVATE Qt${{QT_VERSION_MAJOR}}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${{QT_VERSION}} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.tools.{})
endif()
set_target_properties({} PROPERTIES
    ${{BUNDLE_ID_OPTION}}
    MACOSX_BUNDLE_BUNDLE_VERSION ${{PROJECT_VERSION}}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${{PROJECT_VERSION_MAJOR}}.${{PROJECT_VERSION_MINOR}}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS {} 
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
    RUNTIME DESTINATION ${{CMAKE_INSTALL_BINDIR}}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable({})
endif()

)xyz";
        
        auto file_content = std::vformat(formater_str, 
            std::make_format_args(
                  projectName,     
                  cpps,
                  hs,
                  uis,
                  projectName,
                  qrcFile,
                  projectName,
                  projectName,
                  projectName,
                  projectName,
                  projectName,
                  projectName,
                  projectName,
                  projectName
            )
        );

        cmake_file.write( file_content.c_str(), file_content.size() );
        cmake_file.flush();
        cmake_file.close();
    }




    return 0;
}



