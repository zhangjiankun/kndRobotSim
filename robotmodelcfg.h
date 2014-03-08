#ifndef ROBOTMODELCFG_H
#define ROBOTMODELCFG_H
class QDomElement;
class RobotModelCfg
{
public:
    RobotModelCfg();
    void updateCfgFromXml(const char *xmlFilename);
    unsigned int getAxisNum(){ return AxisNum; }
    const char * get_rootName(unsigned int index){ return rootName[index]; }
    const char * get_nodeAxisName(unsigned int index){ return nodeAxisName[index]; }
    float * get_axisRotation(unsigned int index){ return axisRotation[index];}
    float * get_axisPosition(unsigned int index){ return axisPosition[index];}
    const char * get_filename(unsigned int index){ return m_filename[index];}
    void set_filename(const char *inFilename, unsigned int index) { m_filename[index] = inFilename; }
private:
    bool readfile(const char *fileName);
    bool parseRootElement(const QDomElement &element, const QString &filename);
    void parseGeometryElement(QStringList &filenameList, const QDomElement &element);
    bool isGeometryLegal(QStringList &filenameList, QString infilename);
private:
    enum { MAX_LEN =10 };
    unsigned int AxisNum;
    const char *rootName[10];
    const char *nodeAxisName[10];
    const char *m_filename[10];
    float axisRotation[MAX_LEN][3];
    float axisPosition[MAX_LEN][3];
};
#endif // ROBOTMODELCFG_H
