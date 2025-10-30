// Qt headers
#include <QFile>
#include <QtWidgets/QApplication>
#include <QIcon>

// Project headers
#include "DashboardGeneratorGui.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QApplication::setWindowIcon(QIcon("./Resources/DashboardGeneratorGuiAppIcon.png"));

    // Set the app style sheet
    QFile styleSheetFile("./Resources/Combinear.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    app.setStyleSheet(styleSheet);

    DashboardGeneratorGui window;
    window.show();

    return app.exec();
}
