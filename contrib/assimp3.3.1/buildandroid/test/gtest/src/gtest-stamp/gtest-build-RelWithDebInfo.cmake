

set(command "C:/Program Files (x86)/CMake/bin/cmake.exe;--build;.;--config;${config}")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "D:/Box Sync/SDK/assimp3.3.1/buildandroid/test/gtest/src/gtest-stamp/gtest-build-out.log"
  ERROR_FILE "D:/Box Sync/SDK/assimp3.3.1/buildandroid/test/gtest/src/gtest-stamp/gtest-build-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  D:/Box Sync/SDK/assimp3.3.1/buildandroid/test/gtest/src/gtest-stamp/gtest-build-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "gtest build command succeeded.  See also D:/Box Sync/SDK/assimp3.3.1/buildandroid/test/gtest/src/gtest-stamp/gtest-build-*.log")
  message(STATUS "${msg}")
endif()
