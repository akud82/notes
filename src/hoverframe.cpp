#include "hoverframe.h"
#include <QDebug>

HoverFrame::HoverFrame(QWidget *parent): QFrame(parent)
{

}

void HoverFrame::enterEvent(QEvent* event)
{
    emit hoverEnter();
}

void HoverFrame::leaveEvent(QEvent *event)
{
    emit hoverLeave();
}
