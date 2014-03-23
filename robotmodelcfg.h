#ifndef ROBOTMODELCFG_H
#define ROBOTMODELCFG_H
#include<QObject>
class QDomElement;
class RobotModelCfg:public QObject
{
    Q_OBJECT
public:
    RobotModelCfg();
    ~RobotModelCfg();
    bool updateCfgFromXml(const char *xmlFilename);
    bool saveCfgtoXml(const char *xmlFilename);
    bool clearCfg();
    qint32 getAxisNum(){ return AxisNum; }
    const char * get_rootName(unsigned int index) { return m_modelName[index]; }
    const char * get_nodeAxisName(unsigned int index) { return nodeAxisName[index]; }
    float * get_axisRotation(unsigned int index) { return axisRotation[index]; }
    float * get_axisPosition(unsigned int index) { return axisPosition[index]; }
    float * get_axisRotAttr(unsigned int index) { return m_axisRotAttr[index]; }
    const char * get_filename(unsigned int index);
    void debugInfo();
signals:
    void sigCfgChanged(RobotModelCfg *robotModelCfg);
private:
    bool readfile(const char *fileName);
    bool parseRootElement(const QDomElement &element, const QString &filename);
    void parseGeometryElement(QStringList &filenameList, const QDomElement &element, const QString &tagName, const QString &attriName);
    bool processGeometryData(const QStringList &fullFilenameList);
    bool parseRobotDKElement(const QDomElement &element);
    bool saveGeoFilenameToCfg(const QStringList &filenameList);
    void set_filename(const char *inFilename, unsigned int index);
    void set_modelName(const char *name, unsigned int index);
    void set_nodeAxisName(const char *name, unsigned int index);
    void set_axisRotation(float rx, float ry, float rz, unsigned int index);
    void set_axisPosition(float x, float y, float z, unsigned int index);
    void set_axisRotAttr(float min, float max, unsigned int index);

private:
    enum { MAX_LEN =10 };
    enum { MAX_FILENAME_LEN = 256 };
    int AxisNum;
    char *m_robotname; // 整个机器人的名字
    char *m_modelName[MAX_LEN]; //机器人模型中，每一个关节模型的名字
    char *nodeAxisName[MAX_LEN];//机器人模型中，每一个关节坐标轴的名字
    char *m_filename[MAX_LEN]; //机器人模型中，每一个关节模型的模型文件名
    float axisRotation[MAX_LEN][3];
    float axisPosition[MAX_LEN][3];
    float m_axisRotAttr[MAX_LEN][2];//转轴属性，0：最小值；1：最大值；
};
#endif // ROBOTMODELCFG_H
