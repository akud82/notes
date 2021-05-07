#include "dbmanager.h"
#include <QtSql/QSqlQuery>
#include <QTimeZone>
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QtConcurrent>

/*!
 * \brief DBManager::DBManager
 * \param parent
 */
DBManager::DBManager(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QList<FolderData*> >("QList<FolderData*>");
    qRegisterMetaType<QList<TagData*> >("QList<TagData*>");
    qRegisterMetaType<QList<NoteData*> >("QList<NoteData*>");
}

/*!
 * \brief DBManager::open
 * \param path
 * \param doCreate
 */
void DBManager::open(const QString &path, bool doCreate)
{
    QSqlDatabase m_db;
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    m_db.setDatabaseName(path);
    if(!m_db.open()){
        qDebug() << "Error: connection with database fail";
    }else{
        qDebug() << "Database: connection ok";
    }

    if(doCreate)
        createTables();


    if(m_db.tables().contains(QString("active_notes"))) {
        QSqlQuery query;
        query.exec("PRAGMA table_info(active_notes)");
        bool folderField = false;
        bool tagsField = false;

        // check table columns
        while (query.next()) {
            if(query.value(1) == "folder_id")
                folderField = true;
            qDebug() << "column name: " << query.value(1);
        }

        if(!folderField) {
            qDebug() << "No folderId field found. DB needs migration";
            query.exec("ALTER TABLE active_notes ADD COLUMN folder_id INTEGER NOT NULL DEFAULT(-1)");
            query.exec("ALTER TABLE deleted_notes ADD COLUMN folder_id INTEGER NOT NULL DEFAULT(-1)");
        }
    }

    if(!m_db.tables().contains("folders")) {
        qDebug() << "No folders table found. DB needs migration";

        QSqlQuery query;
        query.exec("CREATE TABLE folders ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "creation_date INTEGER NOT NULL DEFAULT (0),"
                     "modification_date INTEGER NOT NULL DEFAULT (0),"
                     "deletion_date INTEGER NOT NULL DEFAULT (0),"
                     "full_title TEXT);");

        QString active_index = "CREATE UNIQUE INDEX folders_index on folders (id ASC);";
    }

    if(!m_db.tables().contains("tags")) {
        qDebug() << "No tags table found. DB needs migration";

        QSqlQuery query;
        query.exec("CREATE TABLE tags ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "tag TEXT);");

        query.exec("CREATE TABLE tags_to_notes (tag_id INTEGER NOT NULL, note_id INTEGER NOT NULL)");
        query.exec("CREATE UNIQUE INDEX tags_to_notes_index on tags_to_notes (tag_id ASC, note_id ASC);");
    }
}


/*!
 * \brief DBManager::createTables
 */
void DBManager::createTables()
{    
    QSqlQuery query;
    QString active = "CREATE TABLE active_notes ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "folder_id INTEGER NOT NULL DEFAULT(-1),"
                     "creation_date INTEGER NOT NULL DEFAULT (0),"
                     "modification_date INTEGER NOT NULL DEFAULT (0),"
                     "deletion_date INTEGER NOT NULL DEFAULT (0),"
                     "content TEXT, "
                     "full_title TEXT);";

    query.exec(active);

    QString active_index = "CREATE UNIQUE INDEX active_index on active_notes (id ASC);";
    query.exec(active_index);

    QString deleted = "CREATE TABLE deleted_notes ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "folder_id INTEGER NOT NULL DEFAULT(-1),"
                      "creation_date INTEGER NOT NULL DEFAULT (0),"
                      "modification_date INTEGER NOT NULL DEFAULT (0),"
                      "deletion_date INTEGER NOT NULL DEFAULT (0),"
                      "content TEXT,"
                      "full_title TEXT)";

    query.exec(deleted);
}

/*!
 * \brief DBManager::getLastRowID
 * \return
 */
int DBManager::getLastNoteID(bool isTrash)
{
    QString qry;
    if(isTrash) {
        qry = "SELECT seq from SQLITE_SEQUENCE WHERE name='deleted_notes';";
    } else {
        qry = "SELECT seq from SQLITE_SEQUENCE WHERE name='active_notes';";
    }

    QSqlQuery query;
    query.exec(qry);
    query.next();
    return query.value(0).toInt();
}

int DBManager::getLastFolderID()
{
    QSqlQuery query;
    query.exec("SELECT seq from SQLITE_SEQUENCE WHERE name='folders';");
    query.next();
    return query.value(0).toInt();
}

int DBManager::getLastTagID()
{
    QSqlQuery query;
    query.exec("SELECT seq from SQLITE_SEQUENCE WHERE name='tags';");
    query.next();
    return query.value(0).toInt();
}

bool DBManager::forceLastTagIndexValue(const int indexValue)
{
    QSqlQuery query;
    QString queryStr = QStringLiteral("UPDATE SQLITE_SEQUENCE "
                                      "SET seq=%1 "
                                      "WHERE name='tags';").arg(indexValue);
    query.exec(queryStr);
    return query.numRowsAffected() == 1;
};

bool DBManager::forceLastFolderIndexValue(const int indexValue)
{
    QSqlQuery query;
    QString queryStr = QStringLiteral("UPDATE SQLITE_SEQUENCE "
                                      "SET seq=%1 "
                                      "WHERE name='folders';").arg(indexValue);
    query.exec(queryStr);
    return query.numRowsAffected() == 1;
};

/*!
 * \brief DBManager::forceLastRowIndexValue
 * \param indexValue
 * \return
 */
bool DBManager::forceLastNoteIndexValue(const int indexValue)
{
    QSqlQuery query;
    QString queryStr = QStringLiteral("UPDATE SQLITE_SEQUENCE "
                                      "SET seq=%1 "
                                      "WHERE name='active_notes';").arg(indexValue);
    query.exec(queryStr);
    return query.numRowsAffected() == 1;
}

NoteData* DBManager::mapNote(QSqlQuery& query) {
    NoteData* note = new NoteData(this->parent() == Q_NULLPTR ? Q_NULLPTR : this);

    int id =  query.value(0).toInt();
    int folderId = query.value(1).toInt();

    qint64 epochDateTimeCreation = query.value(2).toLongLong();
    QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation);

    qint64 epochDateTimeModification= query.value(3).toLongLong();
    QDateTime dateTimeModification = QDateTime::fromMSecsSinceEpoch(epochDateTimeModification);

    qint64 epochDateTimeDeletion = query.value(4).toLongLong();
    QDateTime dateTimeDeletion = QDateTime::fromMSecsSinceEpoch(epochDateTimeDeletion);

    QString content = query.value(5).toString();
    QString fullTitle = query.value(6).toString();

    note->setId(id);
    note->setFolderId(folderId);
    note->setCreationDateTime(dateTimeCreation);
    note->setLastModificationDateTime(dateTimeModification);
    note->setDeletionDateTime(dateTimeDeletion);
    note->setContent(content);
    note->setFullTitle(fullTitle);
    return note;
}

TagData* DBManager::mapTag(QSqlQuery& query)
{
    TagData* tag = new TagData(this->parent() == Q_NULLPTR ? Q_NULLPTR : this);

    int id =  query.value(0).toInt();
    QString fullTitle = query.value(1).toString();

    tag->setId(id);
    tag->setFullTitle(fullTitle);
    return tag;
};

FolderData*DBManager::mapFolder(QSqlQuery& query)
{
    FolderData* folder = new FolderData(this->parent() == Q_NULLPTR ? Q_NULLPTR : this);
    int id =  query.value(0).toInt();

    qint64 epochDateTimeCreation = query.value(1).toLongLong();
    QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation);

    qint64 epochDateTimeModification= query.value(2).toLongLong();
    QDateTime dateTimeModification = QDateTime::fromMSecsSinceEpoch(epochDateTimeModification);

    qint64 epochDateTimeDeletion = query.value(3).toLongLong();
    QDateTime dateTimeDeletion = QDateTime::fromMSecsSinceEpoch(epochDateTimeDeletion);

    QString fullTitle = query.value(4).toString();

    folder->setId(id);
    folder->setCreationDateTime(dateTimeCreation);
    folder->setLastModificationDateTime(dateTimeModification);
    folder->setDeletionDateTime(dateTimeDeletion);
    folder->setFullTitle(fullTitle);
    return folder;
};


/*!
 * \brief DBManager::getNote
 * \param id
 * \return
 */
NoteData* DBManager::getNote(QString id)
{
    QSqlQuery query;

    int parsedId = id.split('_')[1].toInt();
    QString queryStr = QStringLiteral("SELECT id, folder_id, creation_date, modification_date, deletion_date, content, full_title FROM active_notes WHERE id = %1 LIMIT 1").arg(parsedId);
    query.exec(queryStr);

    if (query.first())
        return mapNote(query);

    return Q_NULLPTR;
}

TagData* DBManager::getTag(QString id)
{
    QSqlQuery query;

    int parsedId = id.split('_')[1].toInt();
    QString queryStr = QStringLiteral("SELECT id, tag FROM tags WHERE id = %1 LIMIT 1").arg(parsedId);
    query.exec(queryStr);

    if (query.first())
        return mapTag(query);

    return Q_NULLPTR;
};

FolderData* DBManager::getFolder(QString id)
{
    QSqlQuery query;

    int parsedId = id.split('_')[1].toInt();
    QString queryStr = QStringLiteral("SELECT id, creation_date, modification_date, deletion_date, full_title FROM folders WHERE id = %1 LIMIT 1").arg(parsedId);
    query.exec(queryStr);

    if (query.first())
        return mapFolder(query);

    return Q_NULLPTR;
};

/*!
 * \brief DBManager::isNoteExist
 * \param note
 * \return
 */
bool DBManager::isNoteExist(NoteData* note)
{
    QSqlQuery query;

    int id = note->id();
    QString queryStr = QStringLiteral("SELECT EXISTS(SELECT 1 FROM active_notes WHERE id = %1 LIMIT 1 )")
            .arg(id);
    query.exec(queryStr);
    query.next();

    return query.value(0).toInt() == 1;
}

bool DBManager::isTagExist(TagData* tag)
{
    QSqlQuery query;

    int id = tag->id();
    QString queryStr = QStringLiteral("SELECT EXISTS(SELECT 1 FROM tags WHERE id = %1 LIMIT 1 )")
            .arg(id);
    query.exec(queryStr);
    query.next();

    return query.value(0).toInt() == 1;
};


bool DBManager::isFolderExists(FolderData* folder)
{    
    QSqlQuery query;

    int id = folder->id();
    QString queryStr = QStringLiteral("SELECT EXISTS(SELECT 1 FROM folders WHERE id = %1 LIMIT 1 )")
            .arg(id);
    query.exec(queryStr);
    query.next();

    return query.value(0).toInt() == 1;
};

QList<NoteData *> DBManager::getTrashNotes()
{
    QList<NoteData *> noteList;
    QSqlQuery query;
    query.prepare("SELECT id, folder_id, creation_date, modification_date, deletion_date, content, full_title FROM deleted_notes");
    bool status = query.exec();
    if(status){
        while(query.next()){
            noteList.push_back(mapNote(query));
        }
    }

    return noteList;
}

QList<NoteData *> DBManager::getNotes(int folderId)
{
    QList<NoteData *> noteList;

    QSqlQuery query;
    QString qstr = QString("SELECT id, folder_id, creation_date, modification_date, deletion_date, content, full_title FROM active_notes "
                           "WHERE folder_id = %1")
                             .arg(folderId);
    query.prepare(qstr);
    bool status = query.exec();
    if(status){
        while(query.next()){
            noteList.push_back(mapNote(query));
        }
    }

    return noteList;
}

/*!
 * \brief DBManager::getAllNotes
 * \return
 */
QList<NoteData *> DBManager::getAllNotes()
{
    QList<NoteData *> noteList;

    QSqlQuery query;
    query.prepare("SELECT id, folder_id, creation_date, modification_date, deletion_date, content, full_title FROM active_notes");
    bool status = query.exec();
    if(status){
        while(query.next()){
            noteList.push_back(mapNote(query));
        }
    }

    return noteList;
}

QList<TagData *> DBManager::getAllTags()
{
    QList<TagData *> tagList;

    QSqlQuery query;
    query.prepare("SELECT id, tag FROM tags");
    bool status = query.exec();
    if(status){
        while(query.next()){
            tagList.push_back(mapTag(query));
        }
    }

    return tagList;
};

QList<FolderData *> DBManager::getAllFolders()
{
    QList<FolderData *> folderList;

    QSqlQuery query;
    query.prepare("SELECT id, creation_date, modification_date, deletion_date, full_title FROM folders");
    bool status = query.exec();
    if(status){
        while(query.next()){
            folderList.push_back(mapFolder(query));
        }
    }

    return folderList;
};


/*!
 * \brief DBManager::addNote
 * \param note
 * \return
 */
bool DBManager::addNote(NoteData* note)
{
    QSqlQuery query;
    QString emptyStr;

    QString content = note->content()
                            .replace("'","''")
                            .replace(QChar('\x0'), emptyStr);

    QString fullTitle = note->fullTitle()
                              .replace("'","''")
                              .replace(QChar('\x0'), emptyStr);

    int folderId = note->folderId();

    qint64 epochTimeDateCreated = note->creationDateTime()
            .toMSecsSinceEpoch();

    qint64 epochTimeDateLastModified = note->lastModificationdateTime().isNull()
            ? epochTimeDateCreated
            : note->lastModificationdateTime().toMSecsSinceEpoch();

    QString queryStr = QString("INSERT INTO active_notes "
                               "(creation_date, modification_date, deletion_date, content, full_title, folder_id) "
                               "VALUES (%1, %2, -1, '%3', '%4', %5);")
                               .arg(epochTimeDateCreated)
                               .arg(epochTimeDateLastModified)
                               .arg(content)
                               .arg(fullTitle)
                               .arg(folderId);

    qDebug() << "insert" << queryStr;

    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
}

bool DBManager::addTag(TagData* tag)
{
    QSqlQuery query;
    QString emptyStr;

    QString content = tag->fullTitle().replace("'","''").replace(QChar('\x0'), emptyStr);

    QString queryStr = QString("INSERT INTO tags (tag) VALUES (%1);").arg(content);
    query.exec(queryStr);

    return (query.numRowsAffected() == 1);
};


bool DBManager::addFolder(FolderData* folder)
{
    QSqlQuery query;
    QString emptyStr;

    QString fullTitle = folder->fullTitle()
                              .replace("'","''")
                              .replace(QChar('\x0'), emptyStr);

    qint64 epochTimeDateCreated = folder->creationDateTime()
            .toMSecsSinceEpoch();

    qint64 epochTimeDateLastModified = folder->lastModificationdateTime().isNull()
            ? epochTimeDateCreated
            : folder->lastModificationdateTime().toMSecsSinceEpoch();

    QString queryStr = QString("INSERT INTO folders "
                               "(creation_date, modification_date, deletion_date, full_title) "
                               "VALUES (%1, %2, -1, '%3');")
                               .arg(epochTimeDateCreated)
                               .arg(epochTimeDateLastModified)
                               .arg(fullTitle);

    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
};

/*!
 * \brief DBManager::removeNote
 * \param note
 * \return
 */
bool DBManager::removeNote(NoteData* note)
{
    QSqlQuery query;
    QString emptyStr;

    int id = note->id();
    QString queryStr = QStringLiteral("DELETE FROM active_notes "
                                      "WHERE id=%1").arg(id);
    query.exec(queryStr);
    bool removed = (query.numRowsAffected() == 1);

    qint64 epochTimeDateCreated = note->creationDateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateModified = note->lastModificationdateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateDeleted = note->deletionDateTime().toMSecsSinceEpoch();
    QString content = note->content()
                            .replace("'","''")
                            .replace(QChar('\x0'), emptyStr);
    QString fullTitle = note->fullTitle()
                              .replace("'","''")
                              .replace(QChar('\x0'), emptyStr);
    int folderId = note->folderId();

    queryStr = QString("INSERT INTO deleted_notes "
                       "(id, folder_id, creation_date, modification_date, deletion_date, content, full_title) "
                       "VALUES (%1, %2, %3, %4, '%5', '%6', '%7');")
                       .arg(id)
                       .arg(folderId)
                       .arg(epochTimeDateCreated)
                       .arg(epochTimeDateModified)
                       .arg(epochTimeDateDeleted)
                       .arg(content)
                       .arg(fullTitle);

    query.exec(queryStr);
    bool addedToTrashDB = (query.numRowsAffected() == 1);

    return (removed && addedToTrashDB);
}

bool DBManager::removeTag(TagData* tag)
{
    QSqlQuery query;
    QString emptyStr;

    int id = tag->id();
    QString queryStr = QStringLiteral("DELETE FROM tags "
                                      "WHERE id=%1").arg(id);
    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
};

bool DBManager::removeFolder(FolderData* folder)
{
    // TODO: removeAll folder notes to trash, and remove folder after that
    QSqlQuery query;
    QString emptyStr;

    int id = folder->id();
    QString queryStr = QStringLiteral("DELETE FROM folders "
                                      "WHERE id=%1").arg(id);
    query.exec(queryStr);
    bool removed = (query.numRowsAffected() == 1);
    return removed;
};

/*!
 * \brief DBManager::permanantlyRemoveAllNotes
 * \return
 */
bool DBManager::permanantlyRemoveAllNotes()
{
    QSqlQuery query;
    return query.exec(QString("DELETE FROM active_notes"));
}

/*!
 * \brief DBManager::updateNote
 * \param note
 * \return
 */
bool DBManager::updateNote(NoteData* note)
{
    QSqlQuery query;
    QString emptyStr;

    int id = note->id();
    int folderId = note->folderId();
    qint64 epochTimeDateModified = note->lastModificationdateTime().toMSecsSinceEpoch();
    QString content = note->content().replace(QChar('\x0'), emptyStr);
    QString fullTitle = note->fullTitle().replace(QChar('\x0'), emptyStr);

    query.prepare(QStringLiteral("UPDATE active_notes SET modification_date = :date, content = :content, "
                                 "full_title = :title, folderId = :folder WHERE id = :id"));
    query.bindValue(QStringLiteral(":date"), epochTimeDateModified);
    query.bindValue(QStringLiteral(":content"), content);
    query.bindValue(QStringLiteral(":title"), fullTitle);
    query.bindValue(QStringLiteral(":id"), id);
    query.bindValue(QStringLiteral(":folder"), folderId);

    if (!query.exec()) {
        qWarning () << __func__ << ": " << query.lastError();
    }

    return (query.numRowsAffected() == 1);
}

/*!
 * \brief DBManager::migrateNote
 * \param note
 * \return
 */
bool DBManager::migrateNote(NoteData* note)
{
    QSqlQuery query;

    QString emptyStr;

    int id = note->id();
    qint64 epochTimeDateCreated = note->creationDateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateModified = note->lastModificationdateTime().toMSecsSinceEpoch();

    QString content = note->content()
                            .replace("'","''")
                            .replace(QChar('\x0'), emptyStr);

    QString fullTitle = note->fullTitle()
                              .replace("'","''")
                              .replace(QChar('\x0'), emptyStr);

    QString queryStr = QString("INSERT INTO active_notes "
                               "(id, folder_id, creation_date, modification_date, deletion_date, content, full_title) "
                               "VALUES (%1, -1, %2, %3, -1, '%4', '%5');")
                               .arg(id)
                               .arg(epochTimeDateCreated)
                               .arg(epochTimeDateModified)
                               .arg(content)
                               .arg(fullTitle);

    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
}

/*!
 * \brief DBManager::migrateTrash
 * \param note
 * \return
 */
bool DBManager::migrateTrash(NoteData* note)
{
    QSqlQuery query;
    QString emptyStr;

    int id = note->id();
    qint64 epochTimeDateCreated = note->creationDateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateModified = note->lastModificationdateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateDeleted = note->deletionDateTime().toMSecsSinceEpoch();
    QString content = note->content()
                            .replace("'","''")
                            .replace(QChar('\x0'), emptyStr);
    QString fullTitle = note->fullTitle()
                              .replace("'","''")
                              .replace(QChar('\x0'), emptyStr);

    QString queryStr = QString("INSERT INTO deleted_notes "
                               "(id, folder_id, creation_date, modification_date, deletion_date, content, full_title) "
                               "VALUES (%1, -1, %2, %3, %4, '%5', '%6');")
                               .arg(id)
                               .arg(epochTimeDateCreated)
                               .arg(epochTimeDateModified)
                               .arg(epochTimeDateDeleted)
                               .arg(content)
                               .arg(fullTitle);

    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
}

// Tags
bool DBManager::updateTag(TagData* tag)
{};

bool DBManager::assignTag(TagData* tag, NoteData* note)
{};

bool DBManager::removeTag(TagData* tag, NoteData* note)
{
}

bool DBManager::moveToFolder(FolderData* src, FolderData* trg, NoteData* note)
{
}

// Folders
bool DBManager::updateFolder(FolderData* folder)
{};

// SLOTS
// Tags
void DBManager::onTagsListRequested()
{
    qDebug() << "onTagsListRequested()";

    int tagCounter;
    QList<TagData *> tagList;

    tagCounter = getLastTagID();
    tagList = getAllTags();

    emit tagsReceived(tagList, tagCounter);
};

void DBManager::onTagCreateUpdateRequested(TagData* tag)
{
    bool exists = isTagExist(tag);
    if(exists) updateTag(tag);
    else addTag(tag);
};

void DBManager::onTagDeleteRequested(TagData* tag)
{
    removeTag(tag);
};

void DBManager::onTagForceLastRowIndexRequested(int index)
{
    forceLastTagIndexValue(index);
};

// Folders
void DBManager::onFoldersListRequested()
{
    qDebug() << "onFoldersListRequested()";

    int folderCounter;
    QList<FolderData *> folderList;

    folderCounter = getLastFolderID();
    folderList    = getAllFolders();

    emit foldersReceived(folderList, folderCounter);
};

void DBManager::onFolderCreateUpdateRequested(FolderData* folder)
{
    bool exists = isFolderExists(folder);
    if(exists) updateFolder(folder);
    else addFolder(folder);
};

void DBManager::onFolderDeleteRequested(FolderData* folder)
{
    removeFolder(folder);
};

void DBManager::onFolderForceLastRowIndexRequested(int index)
{
    forceLastFolderIndexValue(index);
};


/*!
 * \brief DBManager::onNotesListRequested
 */
void DBManager::onNotesListRequested(FolderModel::FolderType type, int folderId)
{
    qDebug() << "onNotesListRequested()" << folderId;

    int noteCounter;
    QList<NoteData *> noteList;
    noteCounter = getLastNoteID(type == FolderModel::Trash);
    if(type == FolderModel::AllNotes) noteList = getAllNotes();
    else if(type == FolderModel::Trash) noteList = getTrashNotes();
    else if(type == FolderModel::Folder) noteList = getNotes(folderId);

    emit notesReceived(noteList, noteCounter);
}

/*!
 * \brief DBManager::onOpenDBManagerRequested
 * \param path
 * \param doCreate
 */
void DBManager::onOpenDBManagerRequested(QString path, bool doCreate)
{
    open(path, doCreate);
}

/*!
 * \brief DBManager::onCreateUpdateRequested
 * \param note
 */
void DBManager::onNoteCreateUpdateRequested(NoteData* note)
{
    bool exists = isNoteExist(note);
    if(exists) updateNote(note);
    else addNote(note);
}

/*!
 * \brief DBManager::onDeleteNoteRequested
 * \param note
 */
void DBManager::onNoteDeleteRequested(NoteData* note)
{
    removeNote(note);
}

/*!
 * \brief DBManager::onImportNotesRequested
 * \param noteList
 */
void DBManager::onImportNotesRequested(QList<NoteData *> noteList) {
    QSqlDatabase::database().transaction();
    for(NoteData* note : noteList)
        addNote(note);
    QSqlDatabase::database().commit();
}

/*!
 * \brief DBManager::onRestoreNotesRequested
 * \param noteList
 */
void DBManager::onRestoreNotesRequested(QList<NoteData*> noteList) {
    this->permanantlyRemoveAllNotes();
    this->onImportNotesRequested(noteList);
}

/*!
 * \brief DBManager::onExportNotesRequested
 * \param fileName
 */
void DBManager::onExportNotesRequested(QString fileName)
{
    QList<NoteData *> noteList;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    out.setVersion(QDataStream::Qt_5_6);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    out.setVersion(QDataStream::Qt_5_4);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    out.setVersion(QDataStream::Qt_5_2);
#endif
    noteList = getAllNotes();
    out << noteList;
    file.close();

    qDeleteAll(noteList);
    noteList.clear();
}

/*!
 * \brief DBManager::onMigrateNotesRequested
 * \param noteList
 */
void DBManager::onMigrateNotesRequested(QList<NoteData *> noteList)
{
    QSqlDatabase::database().transaction();
    for(NoteData* note : noteList)
        migrateNote(note);
    QSqlDatabase::database().commit();

    qDeleteAll(noteList);
    noteList.clear();
}

/*!
 * \brief DBManager::onMigrateTrashRequested
 * \param noteList
 */
void DBManager::onMigrateTrashRequested(QList<NoteData *> noteList)
{
    QSqlDatabase::database().transaction();
    for(NoteData* note : noteList)
        migrateTrash(note);
    QSqlDatabase::database().commit();

    qDeleteAll(noteList);
    noteList.clear();
}

/*!
 * \brief DBManager::onForceLastRowIndexValueRequested
 * \param index
 */
void DBManager::onNotesForceLastRowIndexRequested(int index)
{
    forceLastNoteIndexValue(index);
}
