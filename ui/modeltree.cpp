#include<QtGui>

#include "modeltree.h"
#include "ui_modeltree.h"

#include "../usrainode.h"

modelTree::modelTree(usrAiNode *Node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelTree)
{
    organization = "Trolltech";
    application = "Designer";

    ui->setupUi(this);
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderLabels(
            QStringList() << tr("Model Name"));
    ui->treeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
    //ui->treeWidget->header()->setResizeMode(1, QHeaderView::Stretch);

    setWindowTitle(tr("Model Tree Viewer"));
    showModelTree(Node);

}

modelTree::~modelTree()
{
    delete ui;
}


void modelTree::accept()
{
    QDialog dialog(this);

    QLabel *orgLabel = new QLabel(tr("&Organization:"));
    QLineEdit *orgLineEdit = new QLineEdit(organization);
    orgLabel->setBuddy(orgLineEdit);

    QLabel *appLabel = new QLabel(tr("&Application:"));
    QLineEdit *appLineEdit = new QLineEdit(application);
    appLabel->setBuddy(appLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(orgLabel, 0, 0);
    gridLayout->addWidget(orgLineEdit, 0, 1);
    gridLayout->addWidget(appLabel, 1, 0);
    gridLayout->addWidget(appLineEdit, 1, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    dialog.setWindowTitle(tr("Choose Settings"));

    if (dialog.exec()) {
        organization = orgLineEdit->text();
        application = appLineEdit->text();
        //showModelTree();
    }
}

void modelTree::showModelTree(usrAiNode *Node)
{
    //QSettings settings(organization, application);
    ui->treeWidget->clear();
    addChildModels(Node, 0);

    ui->treeWidget->sortByColumn(0);
    ui->treeWidget->setFocus();
    setWindowTitle(tr("Model Tree Viewer - %1 by %2")
                   .arg(application).arg(organization));
}

void modelTree::addChildModels(usrAiNode *Node, QTreeWidgetItem *parent)
{
    std::list<usrAiNode *>::iterator childrenItem;
    std::list<usrAiNode *> tmpChildList = Node->getChildrenList();
    QTreeWidgetItem *item;

    if (!parent)
        parent = ui->treeWidget->invisibleRootItem();

    for(childrenItem = tmpChildList.begin(); childrenItem != tmpChildList.end(); ++childrenItem)
    {
        #if 1
        item = new QTreeWidgetItem(parent);
        item->setText(0,(*childrenItem)->getmName());
        addChildModels(*childrenItem, item);
        #endif
    }

}
