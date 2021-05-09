#include "notemodel.h"
#include <QBuffer>
#include <QDebug>
#include <QMimeData>
#include <QDataStream>

NoteModel::NoteModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

NoteModel::~NoteModel()
{
}

QMimeData* NoteModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* data = QAbstractItemModel::mimeData(indexes);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    encodeData(indexes, dataStream);
    data->setData("application/x-note-model", itemData);
    return data;
}

void NoteModel::encodeData(const QModelIndexList &indexes, QDataStream &stream) const
{
    for(int i = 0; i<indexes.length(); i++) {
        QModelIndex idx = indexes[i];
        NoteData* d = m_noteList.at(idx.row());
        stream << d;
    }
}

QModelIndex NoteModel::addNote(NoteData* note)
{
    const int rowCnt = rowCount();
    beginInsertRows(QModelIndex(), rowCnt, rowCnt);
    m_noteList << note;
    endInsertRows();

    return createIndex(rowCnt, 0);
}

QModelIndex NoteModel::insertNote(NoteData *note, int row)
{
    if(row >= rowCount()){
        return addNote(note);
    }else{
        beginInsertRows(QModelIndex(), row, row);
        m_noteList.insert(row, note);
        endInsertRows();
    }

    return createIndex(row,0);
}

NoteData* NoteModel::getNote(const QModelIndex& index)
{
    if(index.isValid()){
        return m_noteList.at(index.row());
    }else{
        return Q_NULLPTR;
    }
}

QModelIndex NoteModel::indexById(int id)
{
    auto it = std::find_if(m_noteList.constBegin(), m_noteList.constEnd(), [&id](const NoteData* note) { return note->id() == id; });
    if(it != m_noteList.constEnd()) {
        int row = m_noteList.indexOf(it.i->t());
        return createIndex(row, 0);
    } else {
        return QModelIndex();
    }
}

void NoteModel::addListNote(QList<NoteData *> noteList)
{
    int start = rowCount();
    int end = start + noteList.count()-1;
    beginInsertRows(QModelIndex(), start, end);
    m_noteList << noteList;
    endInsertRows();
}

NoteData* NoteModel::removeNote(const QModelIndex &noteIndex)
{
    int row = noteIndex.row();
    beginRemoveRows(QModelIndex(), row, row);
    NoteData* note = m_noteList.takeAt(row);
    endRemoveRows();

    return note;
}

bool NoteModel::moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild)
{
    if(sourceRow<0
            || sourceRow >= m_noteList.count()
            || destinationChild <0
            || destinationChild >= m_noteList.count()){

        return false;
    }

    beginMoveRows(sourceParent,sourceRow,sourceRow,destinationParent,destinationChild);
    m_noteList.move(sourceRow,destinationChild);
    endMoveRows();

    return true;
}

void NoteModel::clearNotes()
{
    beginResetModel();
    m_noteList.clear();
    endResetModel();
}

QVariant NoteModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_noteList.count())
        return QVariant();

    NoteData* note = m_noteList[index.row()];
    if(role == NoteID){
        return note->id();
    }else if(role == NoteFullTitle){
        return note->fullTitle();
    }else if(role == NoteCreationDateTime){
        return note->creationDateTime();
    }else if(role == NoteLastModificationDateTime){
        return note->lastModificationdateTime();
    }else if(role == NoteDeletionDateTime){
        return note->deletionDateTime();
    }else if(role == NoteContent){
        return note->content();
    }else if(role == NoteScrollbarPos){
        return note->scrollBarPosition();
    } else if(role == NoteIsTemp){
        return note->isTemp();
    }

    return QVariant();
}

bool NoteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    NoteData* note = m_noteList[index.row()];


    if(role == NoteID){
        note->setId(value.toInt());
    }else if(role == NoteFullTitle){
        note->setFullTitle(value.toString());
    }else if(role == NoteCreationDateTime){
        note->setCreationDateTime(value.toDateTime());
    }else if(role == NoteLastModificationDateTime){
        note->setLastModificationDateTime(value.toDateTime());
    }else if(role == NoteDeletionDateTime){
        note->setDeletionDateTime(value.toDateTime());
    }else if(role == NoteContent){
        note->setContent(value.toString());
    }else if(role == NoteScrollbarPos){
        note->setScrollBarPosition(value.toInt());
    }else if(role == NoteIsTemp) {
        note->setTemp(value.toBool());
    }else{
        return false;
    }

    emit dataChanged(this->index(index.row()),
                     this->index(index.row()),
                     QVector<int>(1,role));

    return true;
}

Qt::ItemFlags NoteModel::flags(const QModelIndex &index) const
{    
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

    return QAbstractListModel::flags(index)  | Qt::ItemIsEnabled |Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

int NoteModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_noteList.count();
}

void NoteModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::stable_sort(m_noteList.begin(), m_noteList.end(), [](NoteData* lhs, NoteData* rhs){
        return lhs->lastModificationdateTime() > rhs->lastModificationdateTime();
    });

    emit dataChanged(index(0), index(rowCount()-1));
}

Qt::DropActions NoteModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions NoteModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

