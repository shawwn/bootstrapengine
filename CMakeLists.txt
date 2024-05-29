cmake_minimum_required(VERSION 3.14)
project(bootstrapengine)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/ProjectBX/DotNet)

include_directories(${CMAKE_SOURCE_DIR}/ProjectBX/Engine)
include_directories(${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Common)
include_directories(${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Graphics)

include_directories("${CMAKE_SOURCE_DIR}/Halo 51/Core")


file(GLOB bootstrapengine_BCIF_BCIEncode_BCIEncode___cpp "${CMAKE_SOURCE_DIR}/BCIF/BCIEncode/BCIEncode/*.cpp")
file(GLOB bootstrapengine_BCIF_BCIEncode_libjpeg___c "${CMAKE_SOURCE_DIR}/BCIF/BCIEncode/libjpeg/*.c")
file(GLOB bootstrapengine_BCIF_BCIEncode_libpng___c "${CMAKE_SOURCE_DIR}/BCIF/BCIEncode/libpng/*.c")
file(GLOB bootstrapengine_BCIF_BCIEncode_zlib___c "${CMAKE_SOURCE_DIR}/BCIF/BCIEncode/zlib/*.c")
file(GLOB bootstrapengine_BCIF_Backup_BCIEncode___cpp "${CMAKE_SOURCE_DIR}/BCIF/Backup/BCIEncode/*.cpp")
file(GLOB bootstrapengine_Halo_51_Core_Client___cpp "${CMAKE_SOURCE_DIR}/Halo 51/Core/Client/*.cpp")
file(GLOB bootstrapengine_Halo_51_Core_Editor___cpp "${CMAKE_SOURCE_DIR}/Halo 51/Core/Editor/*.cpp")
file(GLOB bootstrapengine_Halo_51_Core_Game___cpp "${CMAKE_SOURCE_DIR}/Halo 51/Core/Game/*.cpp")
file(GLOB bootstrapengine_ProjectBX_DotNet_Bootstrap___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/DotNet/Bootstrap/*.cpp")
file(GLOB bootstrapengine_ProjectBX_DotNet_EditLib___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/DotNet/EditLib/*.cpp")
file(GLOB bootstrapengine_ProjectBX_DotNet_EngineManaged___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/DotNet/EngineManaged/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_Common___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Common/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_Common_IrrXML___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Common/IrrXML/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_FileSystem___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/FileSystem/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_Graphics___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Graphics/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_Graphics_NVTriStrip___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Graphics/NVTriStrip/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_Test___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/Test/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Engine_UberTexture___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Engine/UberTexture/*.cpp")
file(GLOB bootstrapengine_ProjectBX_Samples_Win32___cpp "${CMAKE_SOURCE_DIR}/ProjectBX/Samples/Win32/*.cpp")

add_library( # Sets the name of the library.
        bootstrapengine

        # Sets the library as a shared library.
        SHARED

        ${bootstrapengine_BCIF_BCIEncode_BCIEncode___cpp}
        ${bootstrapengine_BCIF_BCIEncode_libjpeg___c}
        ${bootstrapengine_BCIF_BCIEncode_libpng___c}
        ${bootstrapengine_BCIF_BCIEncode_zlib___c}
        ${bootstrapengine_BCIF_Backup_BCIEncode___cpp}
        ${bootstrapengine_Halo_51_Core_Client___cpp}
        ${bootstrapengine_Halo_51_Core_Editor___cpp}
        ${bootstrapengine_Halo_51_Core_Game___cpp}
        ${bootstrapengine_ProjectBX_DotNet_Bootstrap___cpp}
        ${bootstrapengine_ProjectBX_DotNet_EditLib___cpp}
        ${bootstrapengine_ProjectBX_DotNet_EngineManaged___cpp}
        ${bootstrapengine_ProjectBX_Engine_Common___cpp}
        ${bootstrapengine_ProjectBX_Engine_Common_IrrXML___cpp}
        ${bootstrapengine_ProjectBX_Engine_FileSystem___cpp}
        ${bootstrapengine_ProjectBX_Engine_Graphics___cpp}
        ${bootstrapengine_ProjectBX_Engine_Graphics_NVTriStrip___cpp}
        ${bootstrapengine_ProjectBX_Engine_Test___cpp}
        ${bootstrapengine_ProjectBX_Engine_UberTexture___cpp}
        ${bootstrapengine_ProjectBX_Samples_Win32___cpp}
        )

target_link_libraries( # Specifies the target library.
        bootstrapengine
        )
