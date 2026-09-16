# --------------------------------------
# CPack configuration
# --------------------------------------

set(CPACK_PACKAGE_NAME "Polanie CD")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "PolanieCD-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_GENERATOR "ZIP")