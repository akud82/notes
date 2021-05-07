#include "folderdata.h"
#include <QDataStream>

FolderData::FolderData(QObject *parent) : QObject(parent), m_isModified(false), m_isSelected(false)
{
}

int FolderData::id() const
{
    return m_id;
}

void FolderData::setId(const int &id)
{
    m_id = id;
}

QList<NoteData*> FolderData::notes() const
{
    return m_notes;
}

void FolderData::setNotes(const QList<NoteData*> &notes)
{
    m_notes = notes;
}


QString FolderData::fullTitle() const
{
    return m_fullTitle;
}

void FolderData::setFullTitle(const QString &fullTitle)
{
    m_fullTitle = fullTitle;
}

QDateTime FolderData::lastModificationdateTime() const
{
    return m_lastModificationDateTime;
}

void FolderData::setLastModificationDateTime(const QDateTime &lastModificationdateTime)
{
    m_lastModificationDateTime = lastModificationdateTime;
}

bool FolderData::isModified() const
{
    return m_isModified;
}

void FolderData::setModified(bool isModified)
{
    m_isModified = isModified;
}

bool FolderData::isSelected() const
{
    return m_isSelected;
}

void FolderData::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}

QDateTime FolderData::deletionDateTime() const
{
    return m_deletionDateTime;
}

void FolderData::setDeletionDateTime(const QDateTime& deletionDateTime)
{
    m_deletionDateTime = deletionDateTime;
}

QDateTime FolderData::creationDateTime() const
{
    return m_creationDateTime;
}

void FolderData::setCreationDateTime(const QDateTime& creationDateTime)
{
    m_creationDateTime = creationDateTime;
}

QDataStream &operator<<(QDataStream &stream, const FolderData* folderData) {
    return stream << folderData->id() << folderData->fullTitle() << folderData->creationDateTime() << folderData->lastModificationdateTime() << folderData->deletionDateTime();
}

QDataStream &operator>>(QDataStream &stream, FolderData* &folderData){
    folderData = new FolderData();
    int id;
    int folderId;
    QString fullTitle;
    QDateTime lastModificationDateTime;
    QDateTime deletionDataTime;
    QDateTime creationDateTime;
    stream >> id >> folderId >> fullTitle >> creationDateTime >> lastModificationDateTime >> deletionDataTime;
    folderData->setId(id);
    folderData->setFullTitle(fullTitle);
    folderData->setLastModificationDateTime(lastModificationDateTime);
    folderData->setCreationDateTime(creationDateTime);
    folderData->setDeletionDateTime(deletionDataTime);
    return stream;
}

