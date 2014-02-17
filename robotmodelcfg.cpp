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

        nodeAxis[i] = CFGDATAnodeAxis[i];
        filename[i] = CFGDATAfilename[i];
        for(unsigned int j = 0; j < sizeof(CFGDATAaxisRotation)/(AxisNum * sizeof(float)); j++)
        {
            axisRotation[i][j] = CFGDATAaxisRotation[i][j];
            axisPosition[i][j] = CFGDATAaxisPosition[i][j];
        }
    }

}

RobotModelCfg::RobotModelCfg(const char *xmlfilename)
{

}
