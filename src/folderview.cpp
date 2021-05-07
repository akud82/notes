#include "folderview.h"

#include <QTimer>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QList>
#include <QMenu>
#include <QAction>

FolderView::FolderView(QWidget *parent): QTreeWidget(parent)
{
    this->setAttribute(Qt::WA_MacShowFocusRect, 0);
    QTimer::singleShot(0, this, SLOT(init()));    
}

FolderView::~FolderView()
{
}

void FolderView::init()
{
    setSelectionMode( QAbstractItemView::SingleSelection );
    setDropIndicatorShown(true);
    setAcceptDrops(true);
    setDragDropMode(DragDropMode::DropOnly);
    setDragDropOverwriteMode(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setColumnCount(2);
    setMouseTracking(true);
    setUpdatesEnabled(true);


    viewport()->setAttribute(Qt::WA_Hover);
    setRootIsDecorated(false);
    setContextMenuPolicy(Qt::CustomContextMenu);

    // You might also need to use this if you want to limit the size of your last column:
    this->header()->setMinimumSectionSize(5);
    this->header()->resizeSection(0 /*column index*/, 5 /*width*/);
    this->header()->setStretchLastSection(true);

    setColumnWidth(0, 5);

    onFolderCleared();

    connect(this, &QTreeWidget::itemDoubleClicked, this, &FolderView::onItemDblClicked);
    connect(this, &QTreeView::customContextMenuRequested, this, &FolderView::prepareMenu);
}

QModelIndex FolderView::selectedItemIndex() const
{
    QModelIndexList lst = this->selectedIndexes();
    if(lst.length() > 0) return lst.at(0);
    return QModelIndex();
}

void FolderView::prepareMenu(const QPoint & pos)
{
    QTreeWidgetItem *n = itemAt(pos);
    QModelIndex index = indexAt(pos);

    QVariant folderVar = n->data(0, FolderModel::FolderID);

    QAction *newAct = new QAction(QIcon(":/images/trash-16.png"), tr("&Remove"), this);
    newAct->setStatusTip(tr("remove folder"));

    connect(newAct, &QAction::triggered, this, [this, &index]() { emit removeFolder(index); });

    QMenu menu(this);
    menu.addAction(newAct);

    QPoint pt(pos);
    menu.exec(mapToGlobal(pos));
}

QTreeWidgetItem* FolderView::createItem(const FolderData* folder)
{
    QStringList lst = QStringList(QString(""));
    lst.push_back(folder->fullTitle());
    QTreeWidgetItem* item = new QTreeWidgetItem(lst);
    item->setData(0, FolderModel::FolderID, folder->id());
    return item;
}

void FolderView::onFolderAdded(const FolderData* folder, const QModelIndex& index)
{    
    insertTopLevelItem(index.row(), createItem(folder));
    onUpdateTreeHeight();
}

void FolderView::onFoldersAddList(const QList<FolderData*> folderList)
{
    clear();
    for(int i=0; i<folderList.length(); i++) {
        FolderData* folder = folderList.at(i);
        addTopLevelItem(createItem(folder));
    }
    onUpdateTreeHeight();
}

QList<QTreeWidgetItem*> FolderView::items()
{
    QList<QTreeWidgetItem*> items = QList<QTreeWidgetItem*>();

    QTreeWidgetItemIterator fIterator(this, QTreeWidgetItemIterator::All);
    while(*fIterator){
        QTreeWidgetItem* item = *fIterator;
        items.push_back(item);
        ++fIterator;
    }
    return items;
}

void FolderView::onItemDblClicked(QTreeWidgetItem *item, int column)
{    
    if(column == 1) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        this->editItem(item, column);
    }
}

void FolderView::onFolderRemoved(const QModelIndex& index)
{
    takeTopLevelItem(index.row());
    onUpdateTreeHeight();
}

void FolderView::onFolderCleared()
{
    clear();
    onUpdateTreeHeight();
}

void FolderView::onFolderDataChanged(const QModelIndex &index, const QVariant &value, int role)
{
    QTreeWidgetItem* item = itemFromIndex(index);

    if(role == FolderModel::FolderFullTitle)
        item->text(0) = value.toString();

    onUpdateTreeHeight();
}

void FolderView::onUpdateTreeHeight()
{
    QSize size = treeHeight(this);
    setFixedHeight(size.height());
}

QSize FolderView::treeHeight(QTreeWidget* widget) {
    QTreeWidgetItemIterator fIterator(widget, QTreeWidgetItemIterator::All);
    int height = 2;

    while(*fIterator){
        QTreeWidgetItem* item = *fIterator;
        height += widget->visualItemRect(item).height();
        ++fIterator;
    }

    return QSize(widget->sizeHint().width(), height);
}
