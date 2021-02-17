# Install script for directory: /home/peppi/zephyrproject/zephyr/subsys

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/debug/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/logging/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/fs/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/ipc/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/mgmt/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/random/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/storage/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/fb/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/power/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/stats/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/testsuite/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/tracing/cmake_install.cmake")
  include("/mnt/c/Users/fplun/GoogleDrive/Schule/2020_21/DIC/DIC-Lezuo/2.Semester-serial_crypto/DIC-serial-crypto-Programm/Programm/build/zephyr/subsys/canbus/cmake_install.cmake")

endif()

