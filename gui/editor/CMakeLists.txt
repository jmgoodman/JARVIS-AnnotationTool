add_subdirectory(visualizationwindow)


add_library(editor
  editorwidget.hpp
  editorwidget.cpp
  keypointwidget.hpp
  keypointwidget.cpp
  imageviewer.hpp
  imageviewer.cpp
  reprojectionwidget.hpp
  reprojectionwidget.cpp
  reprojectionchartwidget.hpp
  reprojectionchartwidget.cpp
  reprojectionchartview.hpp
  reprojectionchartview.cpp
  bonelengthchartwidget.hpp
  bonelengthchartwidget.cpp
  bonelengthchartview.hpp
  bonelengthchartview.cpp
  datasetcontrolwidget.hpp
  datasetcontrolwidget.cpp
  loaddatasetwindow.hpp
  loaddatasetwindow.cpp
)

target_include_directories(editor
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ../
    visualizationwindow
)

target_link_libraries(editor
  Qt::Widgets
  Qt::Charts
  newdatasetwindow
	src
  visualizationwindow
)
