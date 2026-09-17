# Borrowed from: https://github.com/isledecomp/isle-portable/blob/master/packaging/linux/CMakeLists.txt

# Injects the required variables into the Desktop and MetaInfo files
configure_file(polaniecd.desktop.in "${APP_ID}.desktop" @ONLY)
configure_file(polaniecd.metainfo.xml.in "${APP_ID}.metainfo.xml" @ONLY)

install(FILES "../icons/polanie.png" RENAME "${APP_ID}.png" DESTINATION "share/icons/hicolor/512x512/apps")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${APP_ID}.desktop" DESTINATION "share/applications")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${APP_ID}.metainfo.xml" DESTINATION "share/metainfo")