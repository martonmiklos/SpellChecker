QT      += core gui widgets

TARGET   = SpellChecker
TEMPLATE = app

SOURCES += main.cpp \
           dialog.cpp \
           spellchecker.cpp \
           spellcheckdialog.cpp

HEADERS += dialog.h \
           spellchecker.h \
           spellcheckdialog.h

FORMS   += dialog.ui \
           spellcheckdialog.ui

unix {
   LIBS += -lhunspell
}

win32 {
   INCLUDEPATH += C:/path/to/hunspell/include
   LIBS += C:/path/to/hunspell/Release/hunspell.lib
}
