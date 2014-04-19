#ifndef CFG_H
#define CFG_H

#include <QObject>

class Cfg : public QObject
{
    Q_OBJECT
public:
    explicit Cfg(QObject *parent = 0);
    virtual bool saveCfg(QString filename) = 0;
    virtual bool readCfg(QString filename) = 0;
    virtual bool checkCfg() = 0;
    virtual QString getRobotfilename() = 0;
    virtual void setRobotFilename(QString &filename) = 0;
signals:
    
public slots:
    
};

#endif // CFG_H
