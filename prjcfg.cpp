#include "prjcfg.h"
#include <QFile>
#include <QMessageBox>

PrjCfg::PrjCfg(): m_robotCfgFilename("")
{
}

bool PrjCfg::saveCfg(QString filename)
{

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0, QObject::tr("Project Writer"),
                             QObject::tr("Error: Cannot write file %1: %2")
                             .arg(qPrintable(filename))
                             .arg(qPrintable(file.errorString())));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_1);
    out << m_robotCfgFilename;

    return true;
}

bool PrjCfg::readCfg(QString filename)
{

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(0, QObject::tr("Project Reader"),
                             QObject::tr("Error: Cannot read file %1: %2")
                             .arg(qPrintable(filename))
                             .arg(qPrintable(file.errorString())));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_1);
    in >> m_robotCfgFilename;

    if (!checkCfg())
    {
            QMessageBox::warning(0, QObject::tr("Project Reader"),
                                 QObject::tr("Project file  %1 error or robot model file %2 not exist!")
                                 .arg(filename).arg(m_robotCfgFilename));
            return false;
    }

    return true;
}


bool PrjCfg::checkCfg()
{
    if (!m_robotCfgFilename.isEmpty() && !QFile::exists(m_robotCfgFilename))
    {
        return false;
    }
    return true;
}
