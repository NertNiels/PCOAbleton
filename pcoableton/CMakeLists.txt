if(WIN32)
  function(configure_asio asio_sdk_path_OUT)
    # ASIO-related path/file variables
    set(asio_download_root "https://download.steinberg.net/sdk_downloads")
    set(asio_file_name "asiosdk_2.3.3_2019-06-14.zip")
    set(asio_dir_name "asiosdk_2.3.3_2019-06-14")
    set(asio_working_dir "${CMAKE_BINARY_DIR}/modules")
    set(asio_output_path "${asio_working_dir}/${asio_file_name}")

    message(STATUS "Downloading ASIO SDK")
    file(DOWNLOAD "${asio_download_root}/${asio_file_name}" ${asio_output_path})
    file(SHA1 ${asio_output_path} asio_zip_hash)
    message("   ASIO SDK SHA1: ${asio_zip_hash}")

    message("   Extracting ASIO SDK")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar "xf" ${asio_output_path} --format=zip
      WORKING_DIRECTORY ${asio_working_dir}
      INPUT_FILE ${asio_output_path}
    )

    # Set the ASIO SDK path for the caller
    set(${asio_sdk_path_OUT} "${asio_working_dir}/${asio_dir_name}" PARENT_SCOPE)
  endfunction()
endif()
configure_asio(asio_sdk_path)
add_definitions("/D_CRT_SECURE_NO_WARNINGS")

add_library(pcoableton STATIC include/pcoableton/pcoapi.h include/pcoableton/pcoapi.cpp include/pcoableton/metronome.cpp include/pcoableton/metronome.h include/pcoableton/metro.cpp include/pcoableton/metro.h include/pcoableton/ableton/AudioEngine.cpp include/pcoableton/ableton/AudioEngine.hpp include/pcoableton/ableton/AudioPlatform_Dummy.hpp ${asio_sdk_path}/common/asio.cpp ${asio_sdk_path}/host/asiodrivers.cpp ${asio_sdk_path}/host/pc/asiolist.cpp)

target_link_libraries(pcoableton PUBLIC cpr::cpr)
target_link_libraries(pcoableton PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(pcoableton PUBLIC Ableton::Link)

target_include_directories(pcoableton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)



target_include_directories(pcoableton PUBLIC ${asio_sdk_path}/common)
target_include_directories(pcoableton PUBLIC ${asio_sdk_path}/host)
target_include_directories(pcoableton PUBLIC ${asio_sdk_path}/host/pc)