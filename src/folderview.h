#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include "foldermodel.h"
#include <QTreeWidget>
#include <QObject>
#include <QWidget>

class FolderView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit FolderView(QWidget* parent = Q_NULLPTR);
    ~FolderView();

    QSize treeHeight(QTreeWidget* widget);
    QList<QTreeWidgetItem*> items();
    QModelIndex selectedItemIndex() const;

    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private:
    bool m_dragOver;

    QTreeWidgetItem* createItem(const FolderData* folder);    

signals:
    void removeFolder(const QModelIndex& index);

private slots:
    void init();

public slots:
    void prepareMenu(const QPoint & pos);
    void onItemDblClicked(QTreeWidgetItem* item, int column);
    void onUpdateTreeHeight();
    void onFolderAdded(const FolderData* folder, const QModelIndex& index);
    void onFoldersAddList(const QList<FolderData*> folderList);
    void onFolderRemoved(const QModelIndex& index);
    void onFolderCleared();
    void onFolderDataChanged(const QModelIndex &index, const QVariant &value, int role);
};

#endif // FOLDERVIEW_H
