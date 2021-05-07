#ifndef TAGDATA_H
#define TAGDATA_H

#include <QObject>
#include <QWidget>

class TagData : public QObject
{
    Q_OBJECT
public:
    explicit TagData(QObject *parent = nullptr);

    int id() const;
    void setId(const int& id);

    QString fullTitle() const;
    void setFullTitle(const QString &fullTitle);

    bool isModified() const;
    void setModified(bool isModified);

    bool isSelected() const;
    void setSelected(bool isSelected);

private:
    int m_id;
    QString m_fullTitle;
    bool m_isModified;
    bool m_isSelected;

};

QDataStream &operator<<(QDataStream &stream, const TagData* tagsData);
QDataStream &operator>>(QDataStream &stream, TagData *&tagsData);


#endif // TAGDATA_H
