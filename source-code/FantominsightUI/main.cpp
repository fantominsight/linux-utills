#include <QApplication>

#include "MainWindow.h"

#include "pages/Base64Page.h"
#include "pages/CpuUsagePage.h"
#include "pages/DisksPage.h"
#include "pages/DnsPage.h"
#include "pages/HashPage.h"
#include "pages/NetworkPage.h"
#include "pages/PasswordPage.h"
#include "pages/PingPage.h"
#include "pages/PortPage.h"
#include "pages/RamPage.h"
#include "pages/SysInfoPage.h"
#include "pages/TracePage.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;

    window.addGroup(QStringLiteral("🌐 Сеть"));
    window.addPage<NetworkPage>();
    window.addPage<PingPage>();
    window.addPage<DnsPage>();
    window.addPage<TracePage>();
    window.addPage<PortPage>();

    window.addGroup(QStringLiteral("💻 Система"));
    window.addPage<SysInfoPage>();
    window.addPage<CpuUsagePage>();
    window.addPage<RamPage>();
    window.addPage<DisksPage>();

    window.addGroup(QStringLiteral("🔧 Инструменты"));
    window.addPage<HashPage>();
    window.addPage<PasswordPage>();
    window.addPage<Base64Page>();

    window.show();
    return app.exec();
}
