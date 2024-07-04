#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>

class Graphic : public QGraphicsView
{
    Q_OBJECT

public:
    Graphic(QWidget *parent = nullptr);

    void updatePen(QString lineColor, QString lineStyle, QString lineThickness);
    void undo(); // 撤销上一次的绘制
    void redo(); // 重做撤销的操作
    QImage toQImage();

    QPen pen;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *m_scene;
    QPointF m_startPos, m_lastPos;
    QGraphicsPathItem *m_currentPathItem;
    QList<QGraphicsPathItem*> m_drawnPaths; // 存储已绘制的路径
    QList<QGraphicsPathItem*> m_undonePaths; // 存储撤销的路径
};

#endif // GRAPHIC_H
