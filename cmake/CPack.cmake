# --------------------------------------
# CPack configuration
# --------------------------------------

DetectTargetCPUArchitectures(POLANIE_CPUS)
string(REPLACE ";" "-" POLANIE_CPUS_STRING "${POLANIE_CPUS}")
string(TOLOWER "${POLANIE_CPUS_STRING}" POLANIE_CPUS_STRING)

set(CPACK_PACKAGE_NAME "Polanie CD")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "PolanieCD-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${POLANIE_CPUS_STRING}")
set(CPACK_PACKAGE_CHECKSUM SHA256)
if(WIN32)
	set(CPACK_GENERATOR "ZIP")
elseif(APPLE AND NOT IOS)
	set(CPACK_GENERATOR "DragNDrop")
else()
	set(CPACK_GENERATOR "TGZ")
endif()