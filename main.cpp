#include "hashfactoryui.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>


void setTheme()
{
    QFile f(":qdarkstyle/light/lightstyle.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());

    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HashFactoryUI w;
    setTheme();
    w.show();
    return a.exec();
}


