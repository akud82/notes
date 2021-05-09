#include "foldermodel.h"
#include <QDebug>

FolderModel::FolderModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_currentFolder(Q_NULLPTR),
      m_folderType(None)
{
}

int FolderModel::currentFolderId()
{
    if(m_currentFolder != Q_NULLPTR)
        return m_currentFolder->id();

    return -1;
}

FolderData* FolderModel::currentFolder()
{
    return m_currentFolder;
}

FolderModel::FolderType FolderModel::currentFolderType()
{
    return m_folderType;
}

void FolderModel::setCurrentFolder(const FolderType type, const QModelIndex& index)
{
    FolderData* fdata = getFolder(index);
    if(type != m_folderType || fdata != m_currentFolder) {
        m_folderType = type;
        m_currentFolder = getFolder(index);
        emit folderChanged(type, index);
    }
}

QModelIndex FolderModel::indexById(int id)
{
    auto it = std::find_if(m_folderList.constBegin(), m_folderList.constEnd(), [&id](const FolderData* f) { return f->id() == id; });
    if(it != m_folderList.constEnd()) {
        int row = m_folderList.indexOf(it.i->t());
        return createIndex(row, 0);
    } else {
        return QModelIndex();
    }
}

QModelIndex FolderModel::currentIndex()
{
    if(m_currentFolder != Q_NULLPTR) {
        return indexById(m_currentFolder->id());
    } else {
        return QModelIndex();
    }
}

// Folder specific functionality:
QModelIndex FolderModel::addFolder(FolderData* folder)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_folderList.push_front(folder);
    endInsertRows();

    QModelIndex index = createIndex(0, 0);
    emit folderAdded(folder, index);
    return index;
};

void FolderModel::clearFolders()
{
    beginResetModel();
    m_folderList.clear();
    endResetModel();
    emit folderCleared();
}

FolderData* FolderModel::getFolder(const QModelIndex& index)
{
    if(index.isValid() && index.row() < m_folderList.length()){
        return m_folderList.at(index.row());
    }else{
        return Q_NULLPTR;
    }
};

FolderData* FolderModel::removeFolder(const QModelIndex& index)
{
    int row = index.row();
    beginRemoveRows(QModelIndex(), row, row);    
    FolderData* folder = m_folderList.takeAt(row);
    endRemoveRows();

    emit folderRemoved(index);
    return folder;
};

void FolderModel::addList(QList<FolderData*> folderList)
{
    int start = rowCount();
    int end = start + folderList.count()-1;

    beginInsertRows(QModelIndex(), start, end);
    m_folderList << folderList;
    sort(1, Qt::SortOrder::AscendingOrder);
    endInsertRows();

    emit foldersAddList(m_folderList, m_folderList.length());
}

QModelIndex FolderModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, 0);
};

QModelIndex FolderModel::parent(const QModelIndex &child) const {
    return QModelIndex();
}

void FolderModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::stable_sort(m_folderList.begin(), m_folderList.end(), [](FolderData* lhs, FolderData* rhs){
        return lhs->lastModificationdateTime() > rhs->lastModificationdateTime();
    });

    emit dataChanged(index(0, 0), index(rowCount()-1, 0));
};


int FolderModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return m_folderList.count();
}

int FolderModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return 1;
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    FolderData* folder = m_folderList[index.row()];
    if(role == FolderID) {
        return folder->id();
    } else if (role == FolderFullTitle) {
        return folder->fullTitle();
    } else if (role == FolderCreationDateTime) {
        return folder->creationDateTime();
    } else if(role == FolderLastModificationDateTime) {
        return folder->lastModificationdateTime();
    } else if(role == FolderDeletionDateTime) {
        return folder->deletionDateTime();
    }

    return QVariant();
}

bool FolderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        FolderData* folder = m_folderList[index.row()];
        if(role == FolderID) {
            folder->setId(value.toInt());
        } else if(role == FolderFullTitle) {
            folder->setFullTitle(value.toString());
        } else if (role == FolderCreationDateTime) {
            folder->setCreationDateTime(value.toDateTime());
        } else if(role == FolderLastModificationDateTime) {
            folder->setLastModificationDateTime(value.toDateTime());
        } else if(role == FolderDeletionDateTime) {
            folder->setDeletionDateTime(value.toDateTime());
        } else {
            return false;
        }

        emit folderDataChanged(index, value, role);
        return true;
    }
    return false;
}

Qt::ItemFlags FolderModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEditable ;
}


