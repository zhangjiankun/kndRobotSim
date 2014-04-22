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

#include <QtGui>
#include <QtOpenGL>

#include "glwidget.h"
#include "mainwindow.h"
#include "ui/robotcontrolpanel.h"
#include "ui/modeltree.h"
#include "ui/position.h"
#include "robotmodelcfg.h"
#include "prjcfg.h"

MainWindow::MainWindow()
{
    //常量
    m_curRecentProjNum = MAX_RECENT_PROJECT_NUM / 2;

    //全局变量
    controlPanelUI = NULL;
    modelPanelUI = NULL;
    positionPanelUI = NULL;
    glWidget = NULL;
    m_modelCfgData = NULL;
    m_prjCfg = new PrjCfg;

    createActions();

    //窗体构造
    setWindowTitle(tr("KND Robot Simulator"));//标题
    setWindowIcon(QIcon(":/resource/imgs/knd.png"));//图标
    createMenus(); //菜单栏
    createToolBars();//工具栏
    createStatusBar();//状态栏

    //主窗口区
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget); //主窗口区顶层为centralWidget

    QGridLayout *centralLayout = new QGridLayout;
    centralWidget->setLayout(centralLayout); //centralWidget下挂GridLayout(不释放随进程消亡)
    m_glWidgetArea = new QScrollArea;
    centralLayout->addWidget(m_glWidgetArea, 0, 0); //layout的0，0位置为ScrollArea（不释放随进程消亡）。
    //glWidgetArea->setWidget(glWidget); //ScroolArea里面是OPENGL窗口
    m_glWidgetArea->setWidgetResizable(true);
    m_glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_glWidgetArea->setMinimumSize(50, 50);

    //通知工程失效。
    modelMenu->setEnabled(false);//菜单没办法隐藏
    controlTool->setHidden(true);
    modelTool->setHidden(true);
    m_saveProjectAct->setEnabled(false);

    resize(800, 480);
    readSettings();
}

MainWindow::~MainWindow()
{
    delete m_prjCfg;
}

void MainWindow::createActions()
{
    positionPanelAction = new QAction(tr("&position"), this);
    positionPanelAction->setIcon(QIcon(":/resource/imgs/position.png"));
    positionPanelAction->setShortcut(tr("Ctrl+P"));
    connect(positionPanelAction, SIGNAL(triggered()),
            this, SLOT(showPositionPanel()));

    modelPanelAction = new QAction(tr("&model"), this);
    modelPanelAction->setIcon(QIcon(":/resource/imgs/modelTree.png"));
    modelPanelAction->setShortcut(tr("Ctrl+M"));
    connect(modelPanelAction, SIGNAL(triggered()),
            this, SLOT(showModelPanel()));

    controlPanelAction = new QAction(tr("&New"), this);
    controlPanelAction->setIcon(QIcon(":/resource/imgs/controlPanel1.png"));
    controlPanelAction->setShortcut(tr("Ctrl+P"));
    controlPanelAction->setStatusTip(tr("Control panel"));
    connect(controlPanelAction, SIGNAL(triggered()),
            this, SLOT(showControlPanel()));

    m_newProject = new QAction(tr("&New"), this);
    m_newProject->setIcon(QIcon(":/resource/imgs/new.png"));
    m_newProject->setShortcut(tr("Ctrl+N"));
    connect(m_newProject, SIGNAL(triggered()),
            this, SLOT(newProject()));

    m_openProject = new QAction(tr("&Open..."), this);
    m_openProject->setIcon(QIcon(":/resource/imgs/open.png"));
    m_openProject->setShortcut(tr("Ctrl+O"));
    connect(m_openProject, SIGNAL(triggered()),
            this, SLOT(openProject()));

    m_closeProject = new QAction(tr("&Close"), this);
    m_closeProject->setShortcut(tr("Ctrl+C"));
    connect(m_closeProject, SIGNAL(triggered()),
            this, SLOT(closeProject()));

    m_saveProjectAct = new QAction(tr("&Save"), this);
    m_saveProjectAct->setIcon(QIcon(":/resource/imgs/save.png"));
    m_saveProjectAct->setShortcut(tr("Ctrl+S"));
    connect(m_saveProjectAct, SIGNAL(triggered()),
            this, SLOT(saveProject()));

    m_saveAsProjectAct = new QAction(tr("&Save As..."), this);
    connect(m_saveAsProjectAct, SIGNAL(triggered()),
            this, SLOT(saveAsProject()));

    for (int i = 0; i < m_curRecentProjNum; ++i)
    {
        m_recentProjectActions[i] = new QAction(this);
        m_recentProjectActions[i]->setVisible(false);
        connect(m_recentProjectActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentProjectFile()));
    }


    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setIcon(QIcon(":/resource/imgs/exit.png"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    loadModelAct = new QAction(tr("&Load Robot"),this);
    connect(loadModelAct, SIGNAL(triggered()), this, SLOT(loadRobotModel()) );

    saveModelAct = new QAction(tr("&Save Robot"),this);
    connect(saveModelAct, SIGNAL(triggered()), this, SLOT(saveRobotModel()) );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Project"));
    fileMenu->addAction(m_newProject);
    fileMenu->addAction(m_openProject);
    fileMenu->addAction(m_closeProject);
    fileMenu->addAction(m_saveProjectAct);
    fileMenu->addAction(m_saveAsProjectAct);
    m_separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < m_curRecentProjNum; ++i)
    {
        fileMenu->addAction(m_recentProjectActions[i]);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    modelMenu = menuBar()->addMenu(tr("&Robot"));
    modelMenu->addAction(loadModelAct);
    modelMenu->addAction(saveModelAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    m_projectTool = addToolBar(tr("&project"));
    m_projectTool->addAction(m_newProject);
    m_projectTool->addAction(m_openProject);
    m_projectTool->addAction(m_saveProjectAct);
    m_projectTool->addAction(exitAct);

    controlTool = addToolBar(tr("&controlPanel"));
    controlTool->addAction(controlPanelAction);

//    controlTool->addSeparator();
//    controlTool->addAction(exitAct);

    modelTool = addToolBar(tr("&modelTool"));
    modelTool->addAction(modelPanelAction);
    modelTool->addAction(positionPanelAction);
}

void MainWindow::createContextMenu()
{
    ;
}
void MainWindow::createStatusBar()
{
    QLabel *locationLabel = new QLabel(tr(" status: "));
    locationLabel->setAlignment(Qt::AlignHCenter);//By default, the contents of the label are left-aligned and vertically-centered
    locationLabel->setMinimumSize(locationLabel->sizeHint());//sizeHint窗口移动时的步进单位。

    QLabel *formulaLabel = new QLabel;
    formulaLabel->setIndent(3);//字体离对齐边界的缩进

    statusBar()->addWidget(locationLabel);
    statusBar()->addWidget(formulaLabel, 1);

}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About KND Robot Simulator"),
            tr("<h3>KND Robot Simulator version 0.10</h3>\n\n\n"
               "<p>The <font color=black>KND Robot Simulator</font> is 3D simulator for KND's Robots.</p>"
               "<p>Copyright (C) KND., 2004-2015.</p>"
               "<p>http://www.knd.net</p>"
               "Licensed under GNU GPL license version 3."));
}

void MainWindow::readSettings()
{
    QSettings settings("KND Inc.", "RobotSimulator");

//    restoreGeometry(settings.value("geometry").toByteArray());

    m_recentProjectFiles = settings.value("recentFiles").toStringList();
    updateRecentProjectFileActions();

//    bool showGrid = settings.value("showGrid", true).toBool();
//    showGridAction->setChecked(showGrid);

//    bool autoRecalc = settings.value("autoRecalc", true).toBool();
//    autoRecalcAction->setChecked(autoRecalc);
}
void MainWindow::writeSettings()
{
    QSettings settings("KND Inc.", "RobotSimulator");

//    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", m_recentProjectFiles);
//    settings.setValue("showGrid", showGridAction->isChecked());
//    settings.setValue("autoRecalc", autoRecalcAction->isChecked());
}
bool MainWindow::okToContinue()
{
    if (isWindowModified())
    {
        int r = QMessageBox::warning(this, tr("Spreadsheet"),
                        tr("The document has been modified.\n"
                           "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);
        if (r == QMessageBox::Yes)
        {
            return save();
        }
        else if (r == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToContinue())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::newProject()
{
    if (okToContinue())
    {
        //关闭原工程资源
        delete glWidget;
        glWidget = new GLWidget();
        delete m_modelCfgData;
        m_modelCfgData =  new RobotModelCfg;

        connect(m_modelCfgData, SIGNAL(sigCfgChanged(RobotModelCfg*)),
                glWidget, SLOT(synCfgToModeltree(RobotModelCfg *)) );

        m_glWidgetArea->setWidget(glWidget);
        //通知工程生效
        modelMenu->setEnabled(true);
        controlTool->setHidden(false);
        modelTool->setHidden(false);
        m_saveProjectAct->setEnabled(true);
        setCurrentProjectFile("");
        resize(1024, 680);
    }
}

void MainWindow::openProject()
{
    if (okToContinue())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open Spreadsheet"), ".",
                                   tr("Spreadsheet files (*.prj)"));
        if (!fileName.isEmpty())
        {
            if (!loadPrjFile(fileName))
            {
                return;
            }
        }
    }
}

void MainWindow::openRecentProjectFile()
{
    if (okToContinue())
    {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
        {
            if (!loadPrjFile(action->data().toString()))
            {
                return;
            }

        }
    }
}

bool MainWindow::loadPrjFile(const QString &fileName)
{
    if (!m_prjCfg->readCfg(fileName))
    {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }

    setCurrentProjectFile(fileName);

    //关闭原工程资源
    delete glWidget;
    glWidget = NULL;
    delete m_modelCfgData;
    m_modelCfgData = NULL;

    //从工程配置文件中，获取RobotCfg，模型文件，环境文件等。
    m_modelCfgData = new RobotModelCfg;
    glWidget = new GLWidget();
    connect(m_modelCfgData, SIGNAL(sigCfgChanged(RobotModelCfg*)),
            glWidget, SLOT(synCfgToModeltree(RobotModelCfg *)) );

    m_glWidgetArea->setWidget(glWidget);
    //通知工程生效
    modelMenu->setEnabled(true);
    controlTool->setHidden(false);
    modelTool->setHidden(false);
    m_saveProjectAct->setEnabled(true);

    //更新配置文件,更新时，会自动通知所有订阅cfgchanged信号的对象。
    if ( !m_prjCfg->getRobotfilename().isEmpty()
            && !m_modelCfgData->updateCfgFromXml(qPrintable(m_prjCfg->getRobotfilename())))
    {
        return false;
    }

    m_modelCfgData->debugInfo();
    statusBar()->showMessage(tr("File loaded"), 5000);

    resize(1024, 680);

    return true;
}

void MainWindow::closeProject()
{
    if (okToContinue())
    {
        setWindowTitle(tr("KND Robot Simulator"));//标题
        //通知工程失效。
        modelMenu->setDisabled(true);
        controlTool->setDisabled(true);
        modelTool->setDisabled(true);

        //回收资源
        delete glWidget;
        glWidget = NULL;
        delete m_modelCfgData;
        m_modelCfgData = NULL;

        resize(800, 480);
    }
}

void MainWindow::saveProject()
{
    save();
}

void MainWindow::saveAsProject()
{
    saveAs();
}


bool MainWindow::save()
{
    if (m_curProjectFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveProjectFile(m_curProjectFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                               tr("Save Robot project file"), ".",
                               tr("Spreadsheet files (*.prj)"));
    if (fileName.isEmpty())
    {
        return false;
    }

    return saveProjectFile(fileName);
}

bool MainWindow::saveProjectFile(const QString &fileName)
{
    if (!m_prjCfg->saveCfg(fileName))
    {
        statusBar()->showMessage(tr("Saving canceled"), 2000);
        return false;
    }

    setCurrentProjectFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateRecentProjectFileActions()
{
    QMutableStringListIterator i(m_recentProjectFiles);
    while (i.hasNext())
    {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < m_curRecentProjNum; ++j)
    {
        if (j < m_recentProjectFiles.count())
        {
            QString text = tr("&%1 %2")
                           .arg(j + 1)
                           .arg(m_recentProjectFiles[j]);
            m_recentProjectActions[j]->setText(text);
            m_recentProjectActions[j]->setData(m_recentProjectFiles[j]);
            m_recentProjectActions[j]->setVisible(true);
        }
        else
        {
            m_recentProjectActions[j]->setVisible(false);
        }
    }
    m_separatorAction->setVisible(!m_recentProjectFiles.isEmpty());
}
//将当前文件名保存到全局变量。更新最近打开的工程全局变量。并将这两个值保存到工程文件中。设置窗口标题。
//同时，设置窗口为未修改状态。
void MainWindow::setCurrentProjectFile(const QString &fileName)
{
    m_curProjectFile = fileName;
    setWindowModified(false);

    QString shownName = tr("Untitled");//如果输入filenname是空字符串，则显示
    if (!fileName.isEmpty())
    {
        shownName = strippedName(fileName);
        m_recentProjectFiles.removeAll(fileName);
        m_recentProjectFiles.prepend(fileName);
        updateRecentProjectFileActions();
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("Robot Simulator Project")));
}


void MainWindow::loadRobotModel()
{
    if (NULL == glWidget)
    {
        QMessageBox::warning(0, QObject::tr("Load Robot Model"),
                             QObject::tr("Error: Cannot load model!"));
        return;
    }

    //获取文件模型文件名
    QString filename;
    filename = QFileDialog::getOpenFileName(this,
                               tr("Choose model file"), ".",
                               tr("Model files (*.xml)"));
    if (filename.isEmpty())
    {
        QMessageBox::warning(0, QObject::tr("Load Robot Model"),
                             QObject::tr("Error: Filename is empty!"));
        return;
    }

    //更新配置文件,更新时，会自动通知所有订阅cfgchanged信号的对象。
    if (!m_modelCfgData->updateCfgFromXml(qPrintable(filename)))
    {
        //调用函数中失败提示用户。这里不在重复抛出提示。
        //恢复已经被配置的数据。
        m_modelCfgData->InitCfg();
        return;
    }

    //更新内存中的projfile中保存的机器人模型文件。
    m_prjCfg->setRobotFilename(filename);
    setWindowModified(true);

    //更新opengl模型树
    glWidget->synCfgToModeltree(m_modelCfgData);

    m_modelCfgData->debugInfo();
    QMessageBox::information(0, QObject::tr("Debug"),"Load model success!");

//    glWidget->setAndUpdateRobotModel(qPrintable(filename));

    qDebug("MainWindow load model");
}

void MainWindow::saveRobotModel()
{
    if (NULL == glWidget)
    {
        QMessageBox::warning(0, QObject::tr("DOM Parser"),
                             QObject::tr("Error: Cannot load model!"));
        return;
    }

    //获取文件模型文件名
    QString filename = QFileDialog::getSaveFileName(this,
                               tr("Save Robot Model to file"), ".",
                               tr("Teaching lists files (*.xml)"));
    if (filename.isEmpty())
    {
        qDebug("Filename is null %s",qPrintable(filename));
        return;
    }



    if (!m_modelCfgData->saveCfgtoXml(qPrintable(filename)))
    {
        QMessageBox::warning(this, tr("Robot Model"),
                             tr("Save Robot Model fail!"));
        return;
    }

    //更新到projfile中
    m_prjCfg->setRobotFilename(filename);
    setWindowModified(false);

    QMessageBox::information(this, tr("Robot Model"),
                         tr("Save Robot Model success!"));


}

/*************************************************
Function: // 函数名称
Description: // 注册用户窗口。或者考虑将glWidget定义为虚拟类。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
bool MainWindow::registerCenterWidget(QWidget *centerWidget)
{
    Q_ASSERT(m_usrDefinedWidget == NULL);
    if (m_usrDefinedWidget == NULL)
    {
        return false;
    }

    m_usrDefinedWidget = centerWidget;
    return true;
}

/*************************************************
Function: // 函数名称
Description: // 注册配置文件。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
bool MainWindow::registerCfg(Cfg *cfg)
{
    return true;
}


/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void MainWindow::showPositionPanel()
{
    if (!positionPanelUI)
    {
        positionPanelUI = new position(glWidget->getRobotRootNode(),this);
        connect(positionPanelUI, SIGNAL(xTransitionChanged(double, const char *)),
                glWidget, SLOT(setXTransition(double, const char *)));
        connect(positionPanelUI, SIGNAL(yTransitionChanged(double, const char *)),
                glWidget, SLOT(setYTransition(double, const char *)));
        connect(positionPanelUI, SIGNAL(zTransitionChanged(double, const char *)),
                glWidget, SLOT(setZTransition(double, const char *)));
        connect(glWidget, SIGNAL(xTransitionChanged(double)), positionPanelUI, SLOT(setXTransition(double)));
        connect(glWidget, SIGNAL(yTransitionChanged(double)), positionPanelUI, SLOT(setYTransition(double)));
        connect(glWidget, SIGNAL(zTransitionChanged(double)), positionPanelUI, SLOT(setZTransition(double)));

        //订阅glWidget 机器手臂模型更新
        connect(glWidget, SIGNAL(sigRotbotModelChanged(UsrAiNode *)), positionPanelUI, SLOT(modelUpdate(UsrAiNode *)));
    }
    if (positionPanelUI->isHidden())
    {
        positionPanelUI->show();
    }
    else
    {
        positionPanelUI->activateWindow();
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void MainWindow::showModelPanel()
{
    if (!modelPanelUI)
    {
        modelPanelUI = new modelTree(glWidget->getRobotRootNode(), this);
        connect(modelPanelUI, SIGNAL(modelChanged()), glWidget, SLOT(updateGL()));

        //订阅glWidget机器手臂模型更新
        connect(m_modelCfgData, SIGNAL(sigRotbotModelChanged(UsrAiNode *)), modelPanelUI, SLOT(showModelTree(UsrAiNode *)));
        //connect(modelPanelUI, SIGNAL(sigHiddenModel()), glWidget, SLOT(hiddenModel(const char *)));
    }

    if (modelPanelUI->isHidden())
    {
        modelPanelUI->show();
    }
    else
    {
        modelPanelUI->activateWindow();
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void MainWindow::showControlPanel()
{
    if (!controlPanelUI)
    {
        controlPanelUI = new RobotControlPanel(glWidget->getRobotRootNode(), m_modelCfgData, this);

        //通知opengl窗口更新.
        connect(controlPanelUI, SIGNAL(sigModelChanged()), glWidget, SLOT(updateGL()));

        //订阅glWidget机器手臂模型更新.
        connect(glWidget, SIGNAL(sigRotbotModelChanged(UsrAiNode *)), controlPanelUI, SLOT(updateRobotNode(UsrAiNode *)));

        //订阅glWidget机器手臂模型的配置数据更新.
        connect(m_modelCfgData, SIGNAL(sigCfgChanged(RobotModelCfg *)),
                controlPanelUI, SLOT(updateRobotCfgData(RobotModelCfg *)));
    }

    if (controlPanelUI->isHidden())
    {
        controlPanelUI->show();
    }
    else
    {
        controlPanelUI->activateWindow();
    }

}
QSlider *MainWindow::createSlider(const char *changedSignal,
                                  const char *setterSlot)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    connect(slider, SIGNAL(valueChanged(int)), glWidget, setterSlot);
    connect(glWidget, changedSignal, slider, SLOT(setValue(int)));
    return slider;
}


QSize MainWindow::getSize()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Grabber"),
                                         tr("Enter pixmap size:"),
                                         QLineEdit::Normal,
                                         tr("%1 x %2").arg(glWidget->width())
                                                      .arg(glWidget->height()),
                                         &ok);
    if (!ok)
        return QSize();

    QRegExp regExp(tr("([0-9]+) *x *([0-9]+)"));
    if (regExp.exactMatch(text)) {
        int width = regExp.cap(1).toInt();
        int height = regExp.cap(2).toInt();
        if (width > 0 && width < 2048 && height > 0 && height < 2048)
            return QSize(width, height);
    }

    return glWidget->size();
}
