#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>

struct Path
{
    QGraphicsPathItem* path;
    int smooth=0;//0:path,1:smooth,2:circle
};

class Graphic : public QGraphicsView
{
    Q_OBJECT

public:
    Graphic(QWidget *parent = nullptr);

    void updatePen(QColor lineColor, QString lineStyle, QString lineThickness, QColor backgroundColor);
    void setBackground(const QColor &backgroundColor);
    void setAxis(int num);
    void undo(); // 撤销上一次的绘制
    void redo(); // 重做撤销的操作
    void smooth(); //曲线平滑
    QImage toQImage();

    QPen pen;
    bool isPaintCircle=false;
    bool isLineColorRandom=false;
    QColor m_lineColor;
    QColor m_backgroundColor;

public slots:
    void clearAll();

signals:
    void undoAvailableChanged(bool available);
    void redoAvailableChanged(bool available);
    void smoothAvailableChanged(bool available);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPainterPath smoothPathWithLengthPreservation(QPainterPath originalPath);
    QPainterPath smoothPathWithMotionModel(QPainterPath originalPath);

    QGraphicsScene *m_scene;
    QPointF m_startPos;
    QGraphicsPathItem *m_currentPathItem = nullptr;
    QList<Path> m_drawnPaths; // 存储已绘制的路径
    QList<Path> m_undonePaths; // 存储撤销的路径
};

#endif // GRAPHIC_H
