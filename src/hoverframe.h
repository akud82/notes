#ifndef HOVERFRAME_H
#define HOVERFRAME_H

#include <QFrame>
#include <QObject>
#include <QWidget>

class HoverFrame : public QFrame
{
    Q_OBJECT
public:
    HoverFrame(QWidget *parent = Q_NULLPTR);

protected:
    virtual void enterEvent(QEvent * event) override;
    virtual void leaveEvent(QEvent * event) override;

signals:
    void hoverEnter();
    void hoverLeave();
};

#endif // HOVERFRAME_H
