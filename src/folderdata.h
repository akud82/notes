#ifndef FOLDERDATA_H
#define FOLDERDATA_H

#include "notedata.h"

#include <QObject>
#include <QDateTime>

class FolderData : public QObject
{
    Q_OBJECT
public:
    explicit FolderData(QObject *parent = nullptr);

    int id() const;
    void setId(const int& id);

    QList<NoteData*> notes() const;
    void setNotes(const QList<NoteData*> &notes);

    QString fullTitle() const;
    void setFullTitle(const QString &fullTitle);

    QDateTime lastModificationdateTime() const;
    void setLastModificationDateTime(const QDateTime &lastModificationdateTime);

    QDateTime creationDateTime() const;
    void setCreationDateTime(const QDateTime& creationDateTime);

    QDateTime deletionDateTime() const;
    void setDeletionDateTime(const QDateTime& deletionDateTime);

    bool isModified() const;
    void setModified(bool isModified);

    bool isSelected() const;
    void setSelected(bool isSelected);

private:
    int m_id;
    QList<NoteData*> m_notes;
    QString m_fullTitle;
    QDateTime m_lastModificationDateTime;
    QDateTime m_creationDateTime;
    QDateTime m_deletionDateTime;
    bool m_isModified;
    bool m_isSelected;
};

QDataStream &operator<<(QDataStream &stream, const FolderData* folderData);
QDataStream &operator>>(QDataStream &stream, FolderData *&folderData);

#endif // FOLDERDATA_H
