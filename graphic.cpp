#include "graphic.h"

Graphic::Graphic(QWidget *parent) : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setBackgroundBrush(Qt::black);
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
        m_startPos = event->pos();
        if (!isPaintCircle) {
            // 创建一个新的路径项
            m_currentPathItem = new QGraphicsPathItem();
            m_currentPathItem->setPen(pen);
            m_scene->addItem(m_currentPathItem);
            QPainterPath path;
            path.moveTo(m_startPos);
            m_currentPathItem->setPath(path);
        } else {
            // 使用QPainterPath模拟圆形绘制
            QPainterPath circlePath;
            circlePath.addEllipse(m_startPos, 0, 0); // 从点开始，半径设为0
            m_currentPathItem = new QGraphicsPathItem(circlePath);
            m_currentPathItem->setPen(pen);
            m_scene->addItem(m_currentPathItem);
        }
    }
}

void Graphic::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_currentPathItem) {
        if (!isPaintCircle) {
            QPainterPath path = m_currentPathItem->path();
            path.lineTo(event->pos());
            m_currentPathItem->setPath(path);
        } else {
            QPainterPath newCirclePath; // 创建一个新的空路径
            double radius = qAbs(event->pos().x() - m_startPos.x());
            // 使用新的半径和起始点创建圆形路径
            newCirclePath.addEllipse(m_startPos, radius, radius);
            // 更新 QGraphicsPathItem 的路径
            m_currentPathItem->setPath(newCirclePath);
        }
        // 更新视图
        this->update();
    }
}

void Graphic::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_currentPathItem) {
        if (!isPaintCircle) {
            m_drawnPaths.append({m_currentPathItem,0});
            emit smoothAvailableChanged(true);
        } else {
            m_drawnPaths.append({m_currentPathItem,2});
            emit smoothAvailableChanged(false);
        }
        m_currentPathItem = nullptr;
        // 清空撤销列表
        m_undonePaths.clear();
        emit undoAvailableChanged(true);
        emit redoAvailableChanged(false);
        this->update();
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
        Path item = m_drawnPaths.takeLast();
        m_undonePaths.append(item);
        m_scene->removeItem(item.path);
        if (item.smooth)
            m_scene->addItem(m_drawnPaths.back().path);
    }
    if (m_drawnPaths.isEmpty())
    {
        emit undoAvailableChanged(false);
        emit smoothAvailableChanged(false);
    }
    else if (m_drawnPaths.back().smooth==2)
    {
        emit smoothAvailableChanged(false);
    }
    else
    {
        emit smoothAvailableChanged(true);
    }
    emit redoAvailableChanged(true);
}

void Graphic::redo()
{
    if (!m_undonePaths.isEmpty()) {
        Path item = m_undonePaths.takeLast();
        if (item.smooth)
            m_scene->removeItem(m_drawnPaths.back().path);
        m_drawnPaths.append(item);
        m_scene->addItem(item.path);
    }
    emit undoAvailableChanged(true);
    if (m_drawnPaths.back().smooth==2)
    {
        emit smoothAvailableChanged(false);
    }
    else
    {
        emit smoothAvailableChanged(true);
    }
    if (m_undonePaths.isEmpty())
        emit redoAvailableChanged(false);
}

void Graphic::smooth()
{
    Path item = m_drawnPaths.back();
    m_scene->removeItem(item.path);

    // 创建一个新的 QGraphicsPathItem 并设置画笔
    m_currentPathItem = new QGraphicsPathItem();
    m_currentPathItem->setPen(pen);
    m_scene->addItem(m_currentPathItem);

    QPainterPath smoothedPath=item.path->path();
    //smoothedPath = smoothPathWithLengthPreservation(smoothedPath);
    smoothedPath = smoothPathWithMotionModel(smoothedPath);
    m_currentPathItem->setPath(smoothedPath); // 设置平滑后的路径
    m_drawnPaths.append({m_currentPathItem,1});

    // 清理撤销列表
    if (!m_undonePaths.isEmpty()) {
        m_undonePaths.clear();
    }
    emit redoAvailableChanged(false);
    m_currentPathItem = nullptr;

    this->update();
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

void Graphic::clearAll()
{
    // 清除场景中的所有项目
    while (!m_scene->items().isEmpty()) {
        delete m_scene->items().first(); // 删除场景中的第一个图形项
    }

    // 清空存储已绘制路径和撤销路径的列表
    m_drawnPaths.clear();
    m_undonePaths.clear();

    // 将绘图模式重置为默认（即非圆形）
    isPaintCircle = false;

    // 发送信号，表明没有可以撤销或重做的操作
    emit undoAvailableChanged(false);
    emit redoAvailableChanged(false);
}

QPointF normalize(const QPointF &point)
{
    qreal length = qSqrt(point.x()*point.x() + point.y()*point.y()); // 计算向量的长度
    if (length > 0.0) {
        return QPointF(point.x()/length, point.y()/length); // 归一化向量
    } else {
        return point; // 如果向量长度为0，则返回原向量
    }
}

QPainterPath Graphic::smoothPathWithLengthPreservation(QPainterPath originalPath)
{
    QPainterPath smoothedPath;
    QPolygonF points = originalPath.toSubpathPolygons().at(0);
    if (points.size() <= 5) {
        smoothedPath = originalPath;
    } else {
        smoothedPath.moveTo(points.first());
        int windowSize = 3;
        for (int i = 1; i < points.size() - 1; ++i) {
            QPointF prevPoint = points.at(i - 1);
            QPointF currPoint = points.at(i);
            QPointF nextPoint = points.at(i + 1);

            // 计算局部平均点
            QPointF smoothedPoint = currPoint;
            if (i >= windowSize && i < points.size() - windowSize) {
                QPointF sumPoint(0, 0);
                for (int j = i - windowSize; j <= i + windowSize; ++j) {
                    sumPoint += points.at(j);
                }
                smoothedPoint = sumPoint / (2 * windowSize + 1);
            }

            // 计算切线方向
            QPointF tangent = normalize(nextPoint - prevPoint);

            // 根据切线方向微调平滑点的位置
            qreal adjustmentFactor = 0.1; // 可以根据需要调整
            smoothedPoint += tangent * adjustmentFactor;

            smoothedPath.lineTo(smoothedPoint);
        }
        smoothedPath.lineTo(points.last());
    }
    return smoothedPath;
}

QPainterPath Graphic::smoothPathWithMotionModel(QPainterPath originalPath)
{
    QPainterPath smoothedPath;

    QPolygonF points = originalPath.toSubpathPolygons().at(0);

    if (points.size() <= 5) {
        smoothedPath = originalPath;
    } else {
        // 初始化平滑路径的第一个点
        smoothedPath.moveTo(points.first());

        // 使用窗口大小来控制平滑程度，窗口越大，平滑程度越高
        int windowSize = 3; // 可以根据需要调整

        // 遍历路径上的每个点
        for (int i = 1; i < points.size() - 1; ++i) {
            QPointF smoothedPoint = points.at(i);

            // 计算局部平均点
            if (i >= windowSize && i < points.size() - windowSize) {
                QPointF sumPoint(0, 0);
                for (int j = i - windowSize; j <= i + windowSize; ++j) {
                    sumPoint += points.at(j);
                }
                smoothedPoint = sumPoint / (2 * windowSize + 1);
            }

            // 将平滑后的点添加到路径中
            smoothedPath.lineTo(smoothedPoint);
        }

        // 添加最后一个点
        smoothedPath.lineTo(points.last());
    }

    return smoothedPath;
}

