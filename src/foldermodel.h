#ifndef FOLDERMODEL_H
#define FOLDERMODEL_H

#include "folderdata.h"
#include <QAbstractItemModel>

class FolderModel : public QAbstractItemModel
{
    Q_OBJECT

public:    
    enum FolderRoles
    {
        FolderID = 0x0200 + 1,
        FolderFullTitle,
        FolderCreationDateTime,
        FolderLastModificationDateTime,
        FolderDeletionDateTime
    };

    enum FolderType
    {
        None = 0x0300 + 1,
        AllNotes,
        Trash,
        Folder
    };

    explicit FolderModel(QObject *parent = nullptr);
    FolderType currentFolderType();
    int currentFolderId();
    FolderData* currentFolder();
    QModelIndex indexById(int id);
    QModelIndex currentIndex();

    void setCurrentFolder(const FolderType type, const QModelIndex& index = QModelIndex());

    // Folder specific functionality:
    QModelIndex addFolder(FolderData* folder);
    void addList(QList<FolderData*> folderList);

    FolderData* getFolder(const QModelIndex& index);
    FolderData* removeFolder(const QModelIndex& index);
    void clearFolders();

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;    
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    FolderData* m_currentFolder;
    QList<FolderData* > m_folderList;
    FolderType m_folderType;

signals:
    void folderChanged(const FolderType type, const QModelIndex& index);
    void folderAdded(const FolderData* folder, const QModelIndex& index);
    void foldersAddList(const QList<FolderData*> folderList, int folderCounter);
    void folderRemoved(const QModelIndex& index);
    void folderCleared();
    void folderDataChanged(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
};

#endif // FOLDERMODEL_H
