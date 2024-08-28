# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/elias/esp/v5.2.1/esp-idf/components/bootloader/subproject"
  "/home/elias/Ethernet_FOTA/build/bootloader"
  "/home/elias/Ethernet_FOTA/build/bootloader-prefix"
  "/home/elias/Ethernet_FOTA/build/bootloader-prefix/tmp"
  "/home/elias/Ethernet_FOTA/build/bootloader-prefix/src/bootloader-stamp"
  "/home/elias/Ethernet_FOTA/build/bootloader-prefix/src"
  "/home/elias/Ethernet_FOTA/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/elias/Ethernet_FOTA/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/elias/Ethernet_FOTA/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
