/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
QT_END_NAMESPACE
class GLWidget;
class RobotControlPanel;
class modelTree;
class position;
class RobotModelCfg;
class PrjCfg;
class Cfg;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
    static QString strippedName(const QString &fullFileName);
private slots:
    void newProject();
    void openProject();
    void closeProject();
    void saveProject();
    void saveAsProject();
    void openRecentProjectFile();

    void about();
    void showControlPanel();
    void showModelPanel();
    void showPositionPanel();
    void loadRobotModel();
    void saveRobotModel();
    bool registerCenterWidget(QWidget *centerWidget);
    bool registerCfg(Cfg * cfg);

private:
    enum { MAX_RECENT_PROJECT_NUM = 10 };

    void createActions();
    void createMenus();
    void createToolBars();
    void createContextMenu();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool save();
    bool saveAs();
    bool saveProjectFile(const QString &fileName);
    void setCurrentProjectFile(const QString &fileName);
    bool loadPrjFile(const QString &fileName);
    void closeEvent(QCloseEvent *event);


    QSlider *createSlider(const char *changedSignal, const char *setterSlot);
//    void setPixmap(const QPixmap &pixmap);
    QSize getSize();
    void updateRecentProjectFileActions();

//    QWidget *centralWidget;
    QScrollArea *m_glWidgetArea;

    QScrollArea *pixmapLabelArea;
    GLWidget *glWidget;
    QWidget *m_usrDefinedWidget;
    RobotModelCfg *m_modelCfgData;
    PrjCfg *m_prjCfg;


    RobotControlPanel *controlPanelUI;
    modelTree *modelPanelUI;
    position *positionPanelUI;

    int m_curRecentProjNum;
    QAction *m_recentProjectActions[MAX_RECENT_PROJECT_NUM];
    QAction *m_separatorAction;
    QString m_curProjectFile;
    QStringList m_recentProjectFiles;

    QMenu *fileMenu;
    QMenu *helpMenu;
    QMenu *modelMenu;
    QAction *m_openProject;
    QAction *m_closeProject;
    QAction *m_newProject;
    QAction *m_saveProjectAct;
    QAction *m_saveAsProjectAct;

    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *controlPanelAction;
    QAction *modelPanelAction;
    QAction *positionPanelAction;
    QAction *loadModelAct;
    QAction *saveModelAct;

    QToolBar *controlTool;
    QToolBar *modelTool;
    QToolBar *m_projectTool;

};

#endif
