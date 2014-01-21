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
    qMenu = NULL;

    ui->setupUi(this);
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderLabels( QStringList() << tr("Model Name") );
    ui->treeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
    //其中参数设置为Qt::CustomContextMenu，则点击鼠标右键会发射信号customContextMenuRequested(const QPoint),事实上设置之后反而不能工作。
    //ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    //ui->treeWidget->header()->setResizeMode(1, QHeaderView::Stretch);

    setWindowTitle(tr("Model Tree Viewer"));
    showModelTree(Node);
    creatModelTreeActions();
    //QWidget及其子类都可有右键菜单，customContextMenuRequested属于类QWidget，不用关联，因为已经自动关联了contextMenuEvent
    //connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(contextMenuEvent(const QPoint)));
}

modelTree::~modelTree()
{
    delete ui;
    delete AddChildrenModel;
    delete ModelPosition;
    delete HiddenModel;
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

void modelTree::creatModelTreeActions()
{
    AddChildrenModel = new QAction(tr("&AddChildrenModel"),this);
    connect(AddChildrenModel, SIGNAL(triggered()), this, SIGNAL(sigAddChildrenModel()));

    ModelPosition = new QAction(tr("&ModelPosition"), this);
    connect(ModelPosition, SIGNAL(triggered()), this, SLOT(sigModelPosition()));

    HiddenModel = new QAction(tr("&Hidden"), this);
    connect(HiddenModel, SIGNAL(triggered()), this, SLOT(sigHiddenModel()));
}

void modelTree::contextMenuEvent(QContextMenuEvent * event)
{
    if (NULL == qMenu)
    {
        qMenu = new QMenu(ui->treeWidget);
    }

    qMenu->clear();//清除原有菜单
    QTreeWidgetItem *curItem = NULL;
    QPoint point = event->pos();//得到窗口坐标
    curItem = ui->treeWidget->itemAt(point);//这里不管是不是点击item上都为默认选中item
    if(curItem==NULL)
    {
       qDebug("Item is NULL");
       return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    }
    qDebug("currentItem is %f ",curItem);

    qMenu->addAction(AddChildrenModel);
    qMenu->addAction(ModelPosition);
    qMenu->addAction(HiddenModel);
    qMenu->exec(QCursor::pos()); //在鼠标点击的位置显示鼠标右键菜单
}

