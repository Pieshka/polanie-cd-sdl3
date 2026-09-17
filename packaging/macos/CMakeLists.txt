# Borrowed from: https://github.com/isledecomp/isle-portable/blob/master/packaging/macos/CMakeLists.txt
set(_icon_file AppIcon)
set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_ID})
set(MACOSX_BUNDLE_COPYRIGHT ${APP_SPDX})
set(POLANIE_TARGET_NAME polanie)
set(MACOSX_POLANIE_BUNDLE_NAME ${APP_NAME}) # Do note that it can be up to 15 characters long
set(MACOSX_POLANIE_BUNDLE_DISPLAY_NAME ${APP_NAME})
set(EDITOR_TARGET_NAME polanie-editor2)
set(MACOSX_EDITOR_BUNDLE_NAME "Level Editor") # Do note that it can be up to 15 characters long
set(MACOSX_EDITOR_BUNDLE_DISPLAY_NAME "${APP_NAME} - Level Editor")
set(MACOSX_BUNDLE_INFO_STRING ${PROJECT_VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}")

# TODO: darwin < 9
set(MACOSX_BUNDLE_REQUIRED_PLATFORM Carbon)

set(CPACK_DMG_VOLUME_NAME "Polanie CD Portable")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/polanie/Info.plist.in"
	"${CMAKE_CURRENT_BINARY_DIR}/polanie/Info.plist"
	@ONLY
)
set(RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/polanie/${_icon_file}.icns")
target_sources(${POLANIE_TARGET_NAME} PRIVATE ${RESOURCE_FILES})
set_target_properties(${POLANIE_TARGET_NAME} PROPERTIES
	MACOSX_BUNDLE TRUE
	MACOSX_BUNDLE_ICON_FILE "${_icon_file}.icns"
	MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/polanie/Info.plist"
	RESOURCE ${RESOURCE_FILES})
install(TARGETS ${POLANIE_TARGET_NAME} DESTINATION ./)
install(CODE "
	include(BundleUtilities)
	fixup_bundle(${CMAKE_BINARY_DIR}/bin/${POLANIE_TARGET_NAME}.app \"\" \"\")
	"
	COMPONENT Runtime)
install(CODE "
	execute_process(COMMAND /usr/bin/codesign
	  --force --deep --sign - --timestamp
	  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${POLANIE_TARGET_NAME}.app/Contents/MacOS/${POLANIE_TARGET_NAME}\")
	")
install(CODE "
file(RENAME
  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${POLANIE_TARGET_NAME}.app\"
  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${MACOSX_POLANIE_BUNDLE_DISPLAY_NAME}.app\")
")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/editor/Info.plist.in"
	"${CMAKE_CURRENT_BINARY_DIR}/editor/Info.plist"
	@ONLY
)
set(RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/editor/${_icon_file}.icns")
target_sources(${EDITOR_TARGET_NAME} PRIVATE ${RESOURCE_FILES})
set_target_properties(${EDITOR_TARGET_NAME} PROPERTIES
	MACOSX_BUNDLE TRUE
	MACOSX_BUNDLE_ICON_FILE "${_icon_file}.icns"
	MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/editor/Info.plist"
	RESOURCE ${RESOURCE_FILES})
install(TARGETS ${EDITOR_TARGET_NAME} DESTINATION ./)
install(CODE "
	include(BundleUtilities)
	fixup_bundle(${CMAKE_BINARY_DIR}/bin/${EDITOR_TARGET_NAME}.app \"\" \"\")
	"
	COMPONENT Runtime)

install(CODE "
	execute_process(COMMAND /usr/bin/install_name_tool
	  -add_rpath \"@executable_path/../Frameworks\"
	  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${EDITOR_TARGET_NAME}.app/Contents/MacOS/${EDITOR_TARGET_NAME}\")
	")
install(CODE "
	execute_process(COMMAND /usr/bin/codesign
	  --force --deep --sign - --timestamp
	  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${EDITOR_TARGET_NAME}.app/Contents/MacOS/${EDITOR_TARGET_NAME}\")
	")
install(CODE "
	file(RENAME
	  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${EDITOR_TARGET_NAME}.app\"
	  \"\$\{CMAKE_INSTALL_PREFIX\}/bin/${MACOSX_EDITOR_BUNDLE_DISPLAY_NAME}.app\")
	")