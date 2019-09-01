#-------------------------------------------------
#
# Project created by QtCreator 2019-01-11T05:21:18
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Liliths_Throne_Mod_Tool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    clothingwindow.cpp \
    clothing/clothingmod.cpp \
    clothing/treehandler.cpp \
    clothing/dialoguewidget.cpp \
    common/colour.cpp \
    common/colourswidget.cpp \
    common/tagswidget.cpp \
    common/datacommon.cpp \
    common/enchantmentwidget.cpp \
    common/effectlisthandler.cpp \
    clothing/treecomboboxdelegate.cpp \
    weaponwindow.cpp \
    weapon/weaponmod.cpp \
    utility.cpp \
    weapon/damagecomboboxdelegate.cpp \
    common/basecomboboxdelegate.cpp \
    clothing/slotcomboboxdelegate.cpp \
    common/listviewhandler.cpp \
    weapon/spellcomboboxdelegate.cpp \
    weapon/texteditdelegate.cpp \
    tattoowindow.cpp \
    tattoo/tattoomod.cpp \
    tattoo/availabilitycomboboxdelegate.cpp \
    # outfitwindow.cpp \ DISABLED FOR BUILD
    # outfit/outfitmod.cpp DISABLED FOR BUILD

HEADERS += \
        mainwindow.h \
    clothingwindow.h \
    clothing/clothingmod.h \
    clothing/treehandler.h \
    clothing/dialoguewidget.h \
    common/colour.h \
    common/colourswidget.h \
    common/tagswidget.h \
    common/datacommon.h \
    common/enchantmentwidget.h \
    common/effectlisthandler.h \
    clothing/treecomboboxdelegate.h \
    weaponwindow.h \
    weapon/weaponmod.h \
    utility.h \
    weapon/damagecomboboxdelegate.h \
    common/basecomboboxdelegate.h \
    clothing/slotcomboboxdelegate.h \
    common/listviewhandler.h \
    weapon/spellcomboboxdelegate.h \
    weapon/texteditdelegate.h \
    tattoowindow.h \
    tattoo/tattoomod.h \
    tattoo/availabilitycomboboxdelegate.h \
    # outfitwindow.h \ DISABLED FOR BUILD
    # outfit/outfitmod.h DISABLED FOR BUILD

FORMS += \
        mainwindow.ui \
    clothingwindow.ui \
    clothing/dialoguewidget.ui \
    common/colourswidget.ui \
    common/tagswidget.ui \
    common/enchantmentwidget.ui \
    weaponwindow.ui \
    tattoowindow.ui \
    # outfitwindow.ui DISABLED FOR BUILD

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
