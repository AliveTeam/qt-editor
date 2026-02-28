if(UNIX)
  install(FILES "${CMAKE_SOURCE_DIR}/Source/rsc/icons/icon.png" DESTINATION share/pixmaps/ RENAME relive-qt-editor.png)
  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/relive-qt-editor.desktop" DESTINATION share/applications/)
endif()
