#ifndef MODELTREE_H
#define MODELTREE_H

#include <QDialog>

namespace Ui {
class modelTree;
}

class modelTree : public QDialog
{
    Q_OBJECT
    
public:
    explicit modelTree(QWidget *parent = 0);
    ~modelTree();
    
private:
    Ui::modelTree *ui;
};

#endif // MODELTREE_H
