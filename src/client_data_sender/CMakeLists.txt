project(client_data_sender VERSION 0.1 LANGUAGES CXX)

set(sources
  ../shared/structs/somestruct.h
  gui/netsettingsdialog.cpp
  gui/netsettingsdialog.h
  gui/widget.cpp
  gui/widget.h
  main.cpp
  tcpsmartclient.cpp
  tcpsmartclient.h
)

# TODO: Как это выразить в cmake-syntax:
#DISTFILES += \
#    json_files/network.json \
#    json_files/network_agent.json \
#    json_files/some_struct.json

qt_add_executable(client_data_sender
  ${sources}
)

target_link_libraries(client_data_sender PRIVATE
  Qt::Core
  Qt::Gui
  Qt::Network
  Qt::Widgets
)
