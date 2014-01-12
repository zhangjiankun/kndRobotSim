#ifndef MODELTREE_H
#define MODELTREE_H

#include <QDialog>

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
    void modelChanged();
private slots:
    void accept();
private:
    void showModelTree(usrAiNode *Node);
    void addChildModels(usrAiNode *Node, QTreeWidgetItem *parent);

    QString organization;
    QString application;
    Ui::modelTree *ui;
};


#endif // MODELTREE_H
