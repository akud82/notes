#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "folderdata.h"
#include "notedata.h"
#include "tagdata.h"
#include <QObject>
#include <QtSql/QSqlDatabase>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = Q_NULLPTR);

private:
    void open(const QString& path, bool doCreate = false);
    void createTables();

    // Tags
    int  getLastTagID();
    bool forceLastTagIndexValue(const int indexValue);
    TagData* mapTag(QSqlQuery& query);

    TagData* getTag(QString id);
    bool isTagExist(TagData* tag);
    QList<TagData *> getAllTags();
    bool addTag(TagData* tag);
    bool removeTag(TagData* tag);
    bool updateTag(TagData* tag);

    bool assignTag(TagData* tag, NoteData* note);
    bool removeTag(TagData* tag, NoteData* note);

    // Folders
    int  getLastFolderID();
    bool forceLastFolderIndexValue(const int indexValue);
    FolderData* mapFolder(QSqlQuery& query);

    FolderData* getFolder(QString id);
    bool isFolderExists(FolderData* folder);
    QList<FolderData *> getAllFolders();
    bool addFolder(FolderData* folder);
    bool removeFolder(FolderData* folder);
    bool updateFolder(FolderData* folder);

    bool moveToFolder(FolderData* src, FolderData* trg, NoteData* note);

    // Notes
    int  getLastNoteID();
    bool forceLastNoteIndexValue(const int indexValue);
    NoteData* mapNote(QSqlQuery& query);

    NoteData* getNote(QString id);
    bool isNoteExist(NoteData* note);

    QList<NoteData *> getAllNotes();
    bool addNote(NoteData* note);
    bool removeNote(NoteData* note);
    bool permanantlyRemoveAllNotes();
    bool updateNote(NoteData* note);

    bool migrateNote(NoteData* note);
    bool migrateTrash(NoteData* note);

signals:
    void notesReceived(QList<NoteData*> noteList, int noteCounter);
    void foldersReceived(QList<FolderData*> folderList, int folderCounter);
    void tagsReceived(QList<TagData*> tagsList, int tagsCounter);

public slots:
    void onOpenDBManagerRequested(QString path, bool doCreate);

    // Tags
    void onTagsListRequested();
    void onTagCreateUpdateRequested(TagData* tag);
    void onTagDeleteRequested(TagData* tag);
    void onTagForceLastRowIndexRequested(int index);

    // Folders
    void onFoldersListRequested();
    void onFolderCreateUpdateRequested(FolderData* folder);
    void onFolderDeleteRequested(FolderData* folder);
    void onFolderForceLastRowIndexRequested(int index);

    // Notes
    void onNotesListRequested();
    void onNoteCreateUpdateRequested(NoteData* note);
    void onNoteDeleteRequested(NoteData* note);
    void onNotesForceLastRowIndexRequested(int index);

    // Import/Export/Migrate Notes
    void onImportNotesRequested(QList<NoteData *> noteList);
    void onRestoreNotesRequested(QList<NoteData *> noteList);
    void onExportNotesRequested(QString fileName);
    void onMigrateNotesRequested(QList<NoteData *> noteList);
    void onMigrateTrashRequested(QList<NoteData *> noteList);
};

#endif // DBMANAGER_H
