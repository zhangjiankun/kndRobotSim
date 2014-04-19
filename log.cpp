#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <iostream>

void customMessageHandler(QtMsgType type, const char *msg)
{
        QString logmsg;
        logmsg = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss.zzz ");

        switch (type)
        {
            //调试信息提示
            case QtDebugMsg:
                logmsg += QString("[Debug]: %1").arg(msg);
                printf("%s\n",qPrintable(logmsg));
                return;

            //一般的warning提示
            case QtWarningMsg:
                logmsg += QString("[Warning]: %1").arg(msg);
                break;

            //严重错误提示
            case QtCriticalMsg:
                logmsg += QString("[Critical]: %1").arg(msg);
                break;

            //致命错误提示
            case QtFatalMsg:
                logmsg += QString("[Fatal]: %1").arg(msg);
                abort();
        }


        QFile outFile("log.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << logmsg << endl;
}
