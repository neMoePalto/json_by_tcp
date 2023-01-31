
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serverDataReceiver
TEMPLATE = app

OBJECTS_DIR = temp/obj
MOC_DIR = temp/moc
DESTDIR = ../../bin/
INCLUDEPATH += src/


#QMAKE_CXX = clang-14
CONFIG += c++20
QMAKE_CXXFLAGS += -std=c++20
# Нужны оба флага! Проверял на этом проекте:
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_LFLAGS   += -stdlib=libc++ # - без него libc++ не видно после вызова ldd

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_WARNING_OUTPUT
#DEFINES += QT_NO_DEBUG_OUTPUT


SOURCES += \
        src/averagetime.cpp \
        src/main.cpp \
        src/tcpserver.cpp \
        src/widget.cpp

HEADERS += \
        src/all_struct_parser/abstractp.h \
        src/all_struct_parser/datahandler.h \
        src/all_struct_parser/datahandler_imlp.h \
        src/all_struct_parser/structparserlight.h \
        src/all_struct_parser/structparserlight_impl.h \
        src/averagetime.h \
        src/headerdescription.h \
        src/headerdescription_impl.h \
        src/parsers/abstractparser_impl.h \
        src/parsers/abstractparsersignalsslots.h \
        src/parsers/jsonparser_impl.h \
        src/parsers/structparser_impl.h \
        src/parsersmanager_impl.h \
        src/parsers/abstractparser.h \
        src/parsers/jsonparser.h \
        src/parsers/structparser.h \
        src/parsersmanager.h \
        src/structs/somestruct.h \
        src/structs/struct001.h \
        src/structs/dataheader.h \
        src/tcpserver.h \
        src/widget.h

