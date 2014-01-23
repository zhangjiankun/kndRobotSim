#ifndef MODELTREE_H
#define MODELTREE_H

#include <QDialog>
class QMenu;
class QDialogButtonBox;
class QSettings;
class QTreeWidget;
class QTreeWidgetItem;
class usrAiNode;
namespace Ui {
class modelTree;
}

class modelTree : public QDialog
{
    Q_OBJECT
    
public:
    explicit modelTree(usrAiNode *Node, QWidget *parent) ;
    ~modelTree();
signals:
    void sigAddChildrenModel();
    void sigModelPosition();
    void modelChanged();
private slots:
    void accept();
    void slotHiddenModel();
    void slotShowModel();

private:
    void showModelTree(usrAiNode *Node);
    void addChildModels(usrAiNode *Node, QTreeWidgetItem *parent);
    void contextMenuEvent(QContextMenuEvent * event);
    void creatModelTreeActions();

    QString organization;
    QString application;
    Ui::modelTree *ui;
    QMenu *qMenu;
    QAction* AddChildrenModel;
    QAction* ModelPosition;
    QAction* HiddenModel;
    QAction* ShowModel;
    usrAiNode *NodeFromOut;//结点指针
    QTreeWidgetItem *curItem;//鼠标右键时指定的item
};


#endif // MODELTREE_H
