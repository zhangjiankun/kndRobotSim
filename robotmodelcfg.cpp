#include<QtGui>
#include <QtXml>
#include <iostream>

#include "robotmodelcfg.h"
#include "debug.h"

RobotModelCfg::RobotModelCfg()
{
    AxisNum = 7;
    const char *CFGDATArootName[] = {"base","S","L","U","R","B","T"};//world理解为 arm.
    const char *CFGDATAnodeAxis[] = {"baseAxis","SAxis","LAxis","UAxis","RAxis","BAxis","TAxis"};

    //上一个电机移动到当前电机坐标时，转动轴的方向
    float CFGDATAaxisRotation[][3] = { {0,0,1},
                                    {0,0,1},{0,1,0},{0,-1,0},
                                    {1,0,0},{0,1,0},{1,0,0}};
    //表中当前项坐标减去上一个坐标的值，表示当前坐标相对上一个坐标的位移。
    float CFGDATAaxisPosition[][3] = {{0.f,0.f,0.f},
                                     {0.f,0.f,-2.9f},{1.4f,1.3f,0.0f},{1.5f,1.3f,8.8f},
                                     {4.6f,0.f,10.5f},{11.7f,-1.2f,10.5f},{13.3f,0.f,10.5f}};

    const char *CFGDATAfilename[] = {
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-0.stl",
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-1.stl",
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-2.stl",
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-3.stl",
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-4.stl",
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-5.stl",
        "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-6.stl",
    };

    for(unsigned int i = 0; i < AxisNum; i++)
    {
        rootName[i] = CFGDATArootName[i];

        nodeAxisName[i] = CFGDATAnodeAxis[i];
        m_filename[i] = CFGDATAfilename[i];
        for(unsigned int j = 0; j < sizeof(CFGDATAaxisRotation)/(AxisNum * sizeof(float)); j++)
        {
            axisRotation[i][j] = CFGDATAaxisRotation[i][j];
            axisPosition[i][j] = CFGDATAaxisPosition[i][j];
        }
    }

}

void RobotModelCfg::updateCfgFromXml(const char *xmlFilename)
{
    Q_ASSERT(NULL != xmlFilename);

    if (true != readfile(xmlFilename))
    {
        return ;
    }
    //xml文件加载
    //模型验证
    //更新模型数据
    qDebug("RobotModelCfg update CFG %s", xmlFilename);

}

bool RobotModelCfg::readfile(const char *filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(0, QObject::tr("DOM Parser"),
                             QObject::tr("Error: Cannot read file %1: %2")
                             .arg(qPrintable(filename))
                             .arg(qPrintable(file.errorString())));
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine,
                        &errorColumn))
    {
        QMessageBox::warning(0, QObject::tr("DOM Parser"),
                             QObject::tr("Error: Parse error at line %1, column %2:%3")
                             .arg(errorLine).arg(errorColumn).arg(qPrintable(errorStr)));
        return false;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "robot_definition")
    {
        QMessageBox::warning(0, QObject::tr("DOM Parser"),
                             QObject::tr("Error: Not a robot_definition file"));
        return false;
    }

    if (!parseRootElement(root, filename))
    {
        return false;
    }

    return true;
}

bool RobotModelCfg::parseRootElement(const QDomElement &element, const QString &filename)
{
    QDomElement elementTmp;
    QDomNode child = element.firstChild();
    bool ret = false;
    QStringList filenameList;
    while (!child.isNull())
    {
        elementTmp = child.toElement();
        if (elementTmp.tagName() == "robot_dk")
        {
        }
        else if (elementTmp.tagName() == "robot_geometry")
        {
            parseGeometryElement(filenameList, elementTmp);
            ret = isGeometryLegal(filenameList, filename);
            if (!ret)
            {
                break;
            }
        }
        else if (elementTmp.tagName() == "CAD_base")
        {
        }
        else if (elementTmp.tagName() == "CAD_scale")
        {
        }
        else
        {
        }

        child = child.nextSibling();
    }

    return ret;
}

bool RobotModelCfg::isGeometryLegal(QStringList &filenameList, QString infilename)
{
    QFileInfo xml(infilename);
    QString filepath = xml.absolutePath() + QDir::separator();

    if (filenameList.length() > MAX_LEN)
    {
        QMessageBox::warning(0,QObject::tr("isGeometryLegal"),QObject::tr("length is %1 > %2")
                             .arg(filenameList.length()).arg(MAX_LEN));
        return false;
    }

    foreach(QString filename, filenameList)
    {
        qDebug()<<filepath + filename;
        if(!QFile::exists(filepath + filename))
        {
            QMessageBox::warning(0, QObject::tr("DOM Parser"),
                                 QObject::tr("Error: %1 file not exits!")
                                 .arg(filepath + filename));
            return false;
        }
    }

    quint8 index =0;
    foreach(QString filename, filenameList)
    {
        //set_filename(qPrintable(filepath + filename), index);
        index++;
    }

    return true;
}

void RobotModelCfg::parseGeometryElement(QStringList &filenameList, const QDomElement &element)
{
    QDomNode child = element.firstChild();

    while (!child.isNull())
    {
        QDomElement elementTmp = child.toElement();
        if (elementTmp.tagName() == "geometry")
        {
            QString filename = /*filepath +*/ elementTmp.attribute("geo");
            filenameList.append(filename);
        }
        child = child.nextSibling();
    }

    return ;
}
