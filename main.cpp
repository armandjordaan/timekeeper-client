#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <Windows.h>

QApplication* appref;
MainWindow* mw;

// Callback function for event hook
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND) {

        int cTxtLen = GetWindowTextLength(hwnd);

        wchar_t wnd_title[256];
        GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
        QString title = QString::asprintf("%d",cTxtLen) +  QString::fromWCharArray(wnd_title);
        mw->appendText(title);
        // mainLabel->setText(title.isEmpty() ? "No active window or access denied." : title);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    appref = &a;

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "timekeeper-client_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    mw = &w;
    w.show();

    // Set up the event hook
    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        NULL, HandleWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        );

    int result = a.exec();

    // Remove the hook after the application finishes
    UnhookWinEvent(hook);

    return result;
}
