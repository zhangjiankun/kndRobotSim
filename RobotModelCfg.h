#ifndef ROBOTMODELCFG_H
#define ROBOTMODELCFG_H

class RobotModelCfg
{
public:
    RobotModelCfg();
    RobotModelCfg(const char *xmlfilename);
    unsigned int getAxisNum(){ return AxisNum; }
    const char  * get_rootName(unsigned int index){ return rootName[index]; }
    const char  * get_nodeAxisName(unsigned int index){ return nodeAxisName[index]; }
    float * get_axisRotation(unsigned int index){ return axisRotation[index];}
    float * get_axisPosition(unsigned int index){ return axisPosition[index];}
    const char  * get_filename(unsigned int index){ return filename[index];}
private:
    enum { MAX_LEN =10 };
    unsigned int AxisNum;
    const char *rootName[10];
    const char *nodeAxisName[10];
    const char *filename[10];
    float axisRotation[MAX_LEN][3];
    float axisPosition[MAX_LEN][3];
};
#endif // ROBOTMODELCFG_H
