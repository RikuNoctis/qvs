#-------------------------------------------------
#
# Project created by QtCreator 2018-06-16T00:15:32
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

PROJECT_DIRECTORY = ../qvs
COMMON_DIRECTORY = ../qvs

VER_MAJ = 1
VERSION = $$VER_MAJ
QMAKE_TARGET_PRODUCT = 'Qvs'
QMAKE_TARGET_DESCRIPTION = 'Qvs'
QMAKE_TARGET_COMPANY = 'ema (egosub)'
QMAKE_TARGET_COPYRIGHT = $$QMAKE_TARGET_COMPANY

MOC_DIR = $${PROJECT_DIRECTORY}/generated/moc
UI_DIR = $${PROJECT_DIRECTORY}/generated/ui
RCC_DIR = $${PROJECT_DIRECTORY}/generated/rcc

RC_ICONS = $${COMMON_DIRECTORY}/res/icons/qvs.ico

OPENCV_LIB = $${PROJECT_DIRECTORY}/opencv/lib
OPENCV_HEADER = $${PROJECT_DIRECTORY}/opencv/include

FFMPEG_LIB = $${PROJECT_DIRECTORY}/ffmpeg/lib
FFMPEG_HEADER = $${PROJECT_DIRECTORY}/ffmpeg/include

INCLUDEPATH += $${OPENCV_HEADER}
INCLUDEPATH += $${OPENCV_HEADER}/opencv
INCLUDEPATH += $${OPENCV_HEADER}/opencv2
INCLUDEPATH += $${FFMPEG_HEADER}

CONFIG(debug, debug|release) {

    contains(QMAKE_COMPILER, gcc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-64bit-gcc
            TARGET = qvs-debug-64bit-gcc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-64bit-gcc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-32bit-gcc
            TARGET = qvs-debug-32bit-gcc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-32bit-gcc
        }

        QMAKE_CXXFLAGS += -O0
        QMAKE_CXXFLAGS += -g
        QMAKE_CXXFLAGS += -ggdb3
    }

    contains(QMAKE_COMPILER, msvc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-64bit-msvc
            TARGET = qvs-debug-64bit-msvc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-64bit-msvc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-32bit-msvc
            TARGET = qvs-debug-32bit-msvc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-32bit-msvc
        }
    }
#    LIBS += -L$${OPENCV_LIB} -lopencv_world341d

    LIBS += -L$${OPENCV_LIB} -lopencv_core342d
    LIBS += -L$${OPENCV_LIB} -lopencv_imgcodecs342d
    LIBS += -L$${OPENCV_LIB} -lopencv_imgproc342d
    LIBS += -L$${OPENCV_LIB} -lopencv_video342d
    LIBS += -L$${OPENCV_LIB} -lopencv_videoio342d

} else {

    contains(QMAKE_COMPILER, gcc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-64bit-gcc
            TARGET = qvs
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-64bit-gcc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-32bit-gcc
            TARGET = qvs-32bit
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-32bit-gcc
        }

        QMAKE_CXXFLAGS += -O2
        QMAKE_CXXFLAGS += -fexpensive-optimizations
        QMAKE_CXXFLAGS += -funit-at-a-time
    }

    contains(QMAKE_COMPILER, msvc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-64bit-msvc
            TARGET = qvs
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-64bit-msvc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-32bit-msvc
            TARGET = qvs-32bit
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-32bit-msvc
        }
    }
#    LIBS += -L$${OPENCV_LIB} -lopencv_world341

    LIBS += -L$${OPENCV_LIB} -lopencv_core342
    LIBS += -L$${OPENCV_LIB} -lopencv_imgcodecs342
    LIBS += -L$${OPENCV_LIB} -lopencv_imgproc342
    LIBS += -L$${OPENCV_LIB} -lopencv_video342
    LIBS += -L$${OPENCV_LIB} -lopencv_videoio342

    DEFINES += NDEBUG
}

S = $${DIR_SEPARATOR}

D = $$DESTDIR
D = $$replace(D, $$escape_expand(\\), $$S)
D = $$replace(D, /, $$S)

E = $$escape_expand(\n\t)

win32 {
    QT += winextras

    DEPLOY_COMMAND = windeployqt
    DEPLOY_TARGET = $$shell_quote($$shell_path($${D}/$${TARGET}.exe))
    QMAKE_POST_LINK += $${DEPLOY_COMMAND} --no-translations $${DEPLOY_TARGET} $${E}

    if($$ARCHITECTURE_64_BIT) {
        message("x86_64 build")
    } else {
        message("x86 build")
        contains(QMAKE_COMPILER, gcc) {
                QMAKE_LFLAGS += -Wl,--large-address-aware
        }
        contains(QMAKE_COMPILER, msvc) {
                QMAKE_LFLAGS += /LARGEADDRESSAWARE
        }
    }
}

#LIBS += -L$${FFMPEG_LIB} -lavcodec
#LIBS += -L$${FFMPEG_LIB} -lavdevice
#LIBS += -L$${FFMPEG_LIB} -lavfilter
#LIBS += -L$${FFMPEG_LIB} -lavformat
#LIBS += -L$${FFMPEG_LIB} -lavutil
#LIBS += -L$${FFMPEG_LIB} -lpostproc
#LIBS += -L$${FFMPEG_LIB} -lswresample
#LIBS += -L$${FFMPEG_LIB} -lswscale

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    com/common.cpp \
    com/preferences.cpp \
    com/timer.cpp \
    com/mail_box.cpp \
    com/app_instance_file_guard.cpp \
    job/job_creator.cpp \
    job/job_creator_help.cpp \
    job/job_chef.cpp \
    job/job_view_cmd.cpp \
    tools/muxer.cpp \
    tools/std_watcher.cpp \
    tools/std_manager.cpp \
    tools/demuxer.cpp \
    tools/audio_enc.cpp \
    script/script_player.cpp \
    com/config.cpp \
    tools/installer_dialog.cpp \
    preview/preview_dialog.cpp \
    preview/timeline_slider.cpp \
    preview/timeline_slider.cpp \
    tools/mediainfo_dialog.cpp \
    tools/mediainfo_loader.cpp \
    script/script_editor.cpp \
    script/syntax_highlighter.cpp \
    script/script_creator.cpp \
    script/number_matcher.cpp

HEADERS += \
    mainwindow.h \
    com/common.h \
    com/preferences.h \
    com/mail_box.h \
    com/timer.h \
    com/app_instance_file_guard.h \
    job/job_creator.h \
    job/job_creator_help.h \
    job/job_chef.h \
    job/job_cmdlist.h \
    job/job_item.h \
    job/job_view_cmd.h \
    tools/muxer.h \
    tools/std_watcher.h \
    tools/std_manager.h \
    tools/demuxer.h \
    tools/audio_enc.h \
    script/script_player.h \
    com/config.h \
    tools/installer_dialog.h \
    preview/preview_dialog.h \
    preview/timeline_slider.h \
    preview/timeline_slider.h \
    tools/mediainfo_dialog.h \
    tools/mediainfo_loader.h \
    script/script_editor.h \
    script/syntax_highlighter.h \
    script/script_creator.h \
    script/number_matcher.h \
    com/version.h

FORMS += \
    mainwindow.ui \
    com/preferences.ui \
    job/job_creator.ui \
    job/job_creator_help.ui \
    job/job_view_cmd.ui \
    tools/muxer.ui \
    tools/std_watcher.ui \
    tools/demuxer.ui \
    tools/audio_enc.ui \
    script/script_player.ui \
    tools/installer_dialog.ui \
    preview/preview_dialog.ui \
    tools/mediainfo_dialog.ui \
    script/script_creator.ui

RESOURCES += \
    res/resources.qrc
