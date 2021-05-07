#include "tagdata.h"

int TagData::id() const
{
    return m_id;
}

void TagData::setId(const int &id)
{
    m_id = id;
}

QString TagData::fullTitle() const
{
    return m_fullTitle;
}

void TagData::setFullTitle(const QString &fullTitle)
{
    m_fullTitle = fullTitle;
}

bool TagData::isModified() const
{
    return m_isModified;
}

void TagData::setModified(bool isModified)
{
    m_isModified = isModified;
}

bool TagData::isSelected() const
{
    return m_isSelected;
}

void TagData::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}

TagData::TagData(QObject *parent) : QObject(parent)
{

}

QDataStream &operator<<(QDataStream &stream, const TagData* tagsData) {
    return stream << tagsData->id() << tagsData->fullTitle();
}

QDataStream &operator>>(QDataStream &stream, TagData* &tagsData){
    tagsData = new TagData();
    int id;
    int folderId;
    QString fullTitle;
    stream >> id >> folderId >> fullTitle;
    tagsData->setId(id);
    tagsData->setFullTitle(fullTitle);
    return stream;
}


