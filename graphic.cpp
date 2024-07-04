#include "graphic.h"

Graphic::Graphic(QWidget *parent) : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setBackgroundBrush(Qt::black);
    m_currentPathItem = nullptr;
}

void Graphic::updatePen(QString lineColor, QString lineStyle, QString lineThickness)
{
    // 更新画笔的颜色
    if (lineColor == "red")
        pen.setColor(Qt::red);
    else if (lineColor == "blue")
        pen.setColor(Qt::blue);
    // 其他颜色...
    pen.setColor("white");

    // 更新画笔的风格
    if (lineStyle == "solid")
        pen.setStyle(Qt::SolidLine);
    else if (lineStyle == "dash")
        pen.setStyle(Qt::DashLine);
    // 其他风格...

    // 更新画笔的宽度
    int thickness = lineThickness.toInt();
    pen.setWidth(thickness);
}

void Graphic::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 创建一个新的路径项
        m_currentPathItem = new QGraphicsPathItem();
        m_currentPathItem->setPen(pen);
        m_scene->addItem(m_currentPathItem);

        // 设置起点
        m_startPos = event->pos();
        m_lastPos = m_startPos;

        // 开始绘制路径
        QPainterPath path;
        path.moveTo(m_lastPos);
        m_currentPathItem->setPath(path);
    }
}

void Graphic::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_currentPathItem) {
        // 更新路径
        QPainterPath path = m_currentPathItem->path();
        path.lineTo(event->pos());
        m_currentPathItem->setPath(path);

        // 更新上一个位置为当前位置
        m_lastPos = event->pos();

        // 更新视图
        update();
    }
}

void Graphic::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_currentPathItem) {
        // 结束路径的绘制
        m_drawnPaths.append(m_currentPathItem);
        m_currentPathItem = nullptr;
    }
}

void Graphic::resizeEvent(QResizeEvent *event)
{
    // 调用基类的resizeEvent()方法
    QGraphicsView::resizeEvent(event);

    // 如果需要调整场景大小或视图显示范围，这里做相应的设置
    m_scene->setSceneRect(0, 0, width(), height());
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void Graphic::undo()
{
    if (!m_drawnPaths.isEmpty()) {
        QGraphicsPathItem *item = m_drawnPaths.takeLast();
        m_undonePaths.append(item);
        m_scene->removeItem(item);
    }
}

void Graphic::redo()
{
    if (!m_undonePaths.isEmpty()) {
        QGraphicsPathItem *item = m_undonePaths.takeLast();
        m_drawnPaths.append(item);
        m_scene->addItem(item);
    }
}

QImage Graphic::toQImage()
{
    // 创建一个与视图大小相同的QPixmap
    QPixmap pixmap(size());

    // 创建一个QPainter实例，以pixmap为目标
    QPainter painter(&pixmap); // 注意这里使用&pixmap

    // 将视图内容渲染到QPixmap上
    render(&painter);

    // 结束绘画
    painter.end();

    // 将QPixmap转换为QImage
    return pixmap.toImage();
}
