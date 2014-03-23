#ifndef POSITION_H
#define POSITION_H

#include <QDialog>

namespace Ui {
class position;
}
class UsrAiNode;
class QStringListModel;

class position : public QDialog
{
    Q_OBJECT
public:
    explicit position(UsrAiNode *Node, QWidget *parent = 0);
    ~position();
signals:
    void xTransitionChanged(double value, const char *RootName);
    void yTransitionChanged(double value, const char *RootName);
    void zTransitionChanged(double value, const char *RootName);
public slots:
    void setXTransition(double value);
    void setYTransition(double value);
    void setZTransition(double value);
    void modelUpdate(UsrAiNode *node);//当结点变化时，需要被通知更新模型
private slots:
    //这里将x位移发给GLWidget,在GLWidget类中更改。既然结点已经指定，也可以在本类中变更
    void on_xPosition_valueChanged(double value){ emit xTransitionChanged(value, qPrintable(globel)); }
    void on_yPosition_valueChanged(double value){ emit yTransitionChanged(value, qPrintable(globel)); }
    void on_zPosition_valueChanged(double value){ emit zTransitionChanged(value, qPrintable(globel)); }
    void on_comboBox_currentIndexChanged(const QString & text);
private:
    UsrAiNode *root;//结点根指针
    QString globel;//on_comboBox_currentIndexChanged 中，局部变量在作用域外释放不用。
    const char * currentRootName;
    QStringListModel *nodeModel;
    Ui::position *ui;
};

#endif // POSITION_H
