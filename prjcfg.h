#ifndef PRJCFG_H
#define PRJCFG_H
#include "cfg.h"
class PrjCfg : public Cfg
{
public:
    PrjCfg();
    bool readCfg(QString filename);
    bool saveCfg(QString filename);
    bool checkCfg();
    QString getRobotfilename() {return m_robotCfgFilename;}
    void setRobotFilename(QString &filename) { m_robotCfgFilename = filename; }
private:
    QString m_robotCfgFilename;
};

#endif // PRJCFG_H
