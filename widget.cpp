#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , graphic(new Graphic)
{
    ui->setupUi(this);
    isEnglish = false;

    showFullScreen();

    ui->pushButton_Undo->setEnabled(false);
    ui->pushButton_Redo->setEnabled(false);

    setComboBox();
    ui->comboBox_LineColor->setCurrentText(tr("渐变"));
    ui->comboBox_LineStyle->setCurrentText(tr("炫光"));
    ui->comboBox_LineThickness->setCurrentText("8");
    ui->comboBox_BackgroundColor->setCurrentText(tr("黑"));
    ui->comboBox_Axis->setCurrentText("8");

    QGridLayout *layout = new QGridLayout(ui->widget_2);
    int picSize = std::min(ui->widget_2->height(), ui->widget_2->width()) - 14;
    if (picSize > 0) {
        // 设置新的大小和位置
        graphic->setFixedSize(picSize, picSize);
    }
    layout->addWidget(graphic);

    graphic->updatePen(ui->comboBox_LineColor->currentText(),ui->comboBox_LineStyle->currentText(),ui->comboBox_LineThickness->currentText(),ui->comboBox_BackgroundColor->currentText());

    // 创建QShortcut对象，第一个参数是快捷键，第二个参数是父对象，第三个参数是槽函数
    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    connect(shortcut1, &QShortcut::activated, this, &Widget::on_pushButton_Undo_clicked);

    QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z), this);
    connect(shortcut2, &QShortcut::activated, this, &Widget::on_pushButton_Redo_clicked);

    QShortcut *shortcut3 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(shortcut3, &QShortcut::activated, this, &Widget::on_pushButton_Export_clicked);

    connect(graphic, &Graphic::undoAvailableChanged, ui->pushButton_Undo, &QPushButton::setEnabled);
    connect(graphic, &Graphic::redoAvailableChanged, ui->pushButton_Redo, &QPushButton::setEnabled);
    connect(graphic, &Graphic::smoothAvailableChanged, ui->pushButton_Smooth, &QPushButton::setEnabled);


}

Widget::~Widget()
{
    delete graphic;
    delete ui;
}

void Widget::setComboBox()
{
    QString currentOption = ui->comboBox_LineColor->currentText();
    ui->comboBox_LineColor->clear();
    ui->comboBox_LineColor->addItem(tr("渐变"));
    ui->comboBox_LineColor->addItem(tr("白"));
    ui->comboBox_LineColor->addItem(tr("黑"));
    ui->comboBox_LineColor->addItem(tr("自定义"));
    if (currentOption.startsWith("rgba")) {
        ui->comboBox_LineColor->addItem(currentOption);
    }
    ui->comboBox_LineColor->setCurrentText(currentOption);

    currentOption = ui->comboBox_LineStyle->currentText();
    ui->comboBox_LineStyle->clear();
    ui->comboBox_LineStyle->addItem(tr("炫光"));
    ui->comboBox_LineStyle->addItem(tr("实线"));
    ui->comboBox_LineStyle->addItem(tr("橡皮擦"));
    ui->comboBox_LineStyle->setCurrentText(currentOption);

    currentOption = ui->comboBox_LineThickness->currentText();
    ui->comboBox_LineThickness->clear();
    ui->comboBox_LineThickness->addItem(tr("自定义"));
    for(int i = 1; i <= 30; ++i){
        ui->comboBox_LineThickness->addItem(QString::number(i));
    }
    if (currentOption.startsWith("rgba")) {
        ui->comboBox_LineThickness->addItem(currentOption);
    }
    ui->comboBox_LineThickness->setCurrentText(currentOption);

    currentOption = ui->comboBox_BackgroundColor->currentText();
    ui->comboBox_BackgroundColor->clear();
    ui->comboBox_BackgroundColor->addItem(tr("黑"));
    ui->comboBox_BackgroundColor->addItem(tr("白"));
    ui->comboBox_BackgroundColor->addItem(tr("透明"));
    ui->comboBox_BackgroundColor->addItem(tr("自定义"));
    if (currentOption.startsWith("rgba")) {
        ui->comboBox_BackgroundColor->addItem(currentOption);
    }
    ui->comboBox_BackgroundColor->setCurrentText(currentOption);

    currentOption = ui->comboBox_Axis->currentText();
    ui->comboBox_Axis->clear();
    for(int i = 1; i <= 16; ++i){
        ui->comboBox_Axis->addItem(QString::number(i));
    }
    ui->comboBox_Axis->setCurrentText(currentOption);
}

void Widget::on_pushButton_Windowed_clicked()
{
    if (isFullScreen()) {
        // 如果当前是全屏状态，切换到窗口化
        showNormal();
    } else {
        // 如果当前不是全屏状态，切换到全屏
        showFullScreen();
    }
    // 改变按钮文本
    isEnglish = !isEnglish;
    on_pushButton_Switch_Language_clicked();
}

void Widget::on_pushButton_Switch_Language_clicked()
{
    if (!isEnglish) {
        // 加载英文翻译文件
        QTranslator translator;
        translator.load(":/i18n/Magical_Circle_en_US");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        if (!isFullScreen()) {
            ui->pushButton_Windowed->setText(tr("全屏"));
        }
        setComboBox();
    }
    else {
        // 卸载英文翻译文件
        QList<QTranslator*> translators = qApp->findChildren<QTranslator*>();
        for (QTranslator *translator : translators) {
            qApp->removeTranslator(translator);
        }
        ui->retranslateUi(this);
        if (!isFullScreen()) {
            ui->pushButton_Windowed->setText(tr("全屏"));
        }
        setComboBox();
    }
    isEnglish = !isEnglish;
}



void Widget::on_pushButton_Quit_clicked()
{
    this->close();
}


void Widget::on_pushButton_Undo_clicked()
{
    graphic->undo();
}


void Widget::on_pushButton_Redo_clicked()
{
    graphic->redo();
}

void Widget::on_pushButton_Smooth_clicked()
{
    graphic->smooth();
}

void Widget::on_pushButton_Circle_clicked()
{
    // 切换 graphic->isPaintCircle 的值与按钮的选中状态一致
    graphic->isPaintCircle = ui->pushButton_Circle->isChecked();
}

void Widget::on_pushButton_Clear_clicked()
{
    graphic->clearAll();
}

void Widget::on_pushButton_Import_clicked()
{
    // 打开文件对话框，允许用户选择图片文件
    QString fileName = QFileDialog::getOpenFileName(this, tr("导入图像"), "",
                                                    "Images (*.png *.jpg *.jpeg *.JPG *.JPEG *.ico)");
    if (!fileName.isEmpty()) {
        // 读取图片
        QPixmap pixmap(fileName);

        if (!pixmap.isNull()) {
            // 修改背景色为透明，这样不会在想改变背景色的时候还要选中别的项再选回来
            ui->comboBox_BackgroundColor->setCurrentIndex(2);

            // 获取当前Graphic对象的大小
            QSize graphicSize = graphic->size();

            // 缩放图片以适应Graphic对象的大小
            pixmap = pixmap.scaled(graphicSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            // 在场景中添加新的缩放后的图片
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
            graphic->scene()->addItem(item);
            item->setZValue(-1000);

            // 确保图形视图适配新图片的大小
            graphic->fitInView(item, Qt::KeepAspectRatio);
        } else {
            QMessageBox::critical(this, tr("错误"), tr("文件未能读取！"));
        }
    }
}

void Widget::on_pushButton_Export_clicked()
{
    // 弹出一个文件保存对话框，允许用户保存为多种格式
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存图像"), "",
                                                    "PNG files (*.png);;JPEG files (*.jpg *.jpeg);;ICO files (*.ico);;All files (*)");

    if (!fileName.isEmpty())
    {
        // 根据文件扩展名确定保存格式
        QFileInfo fileInfo(fileName);
        QString format = fileInfo.suffix().toLower();

        // 如果用户选择了文件名，我们将其保存为相应格式的文件
        // 假设 graphic 是一个可以转换为 QImage 的对象
        QImage image = graphic->toQImage(); // 这里需要实现 graphic 对象到 QImage 的转换

        // 确保文件名有扩展名
        if (format.isEmpty())
        {
            // 如果没有扩展名，默认保存为PNG
            format = "png";
            fileName += ".png";
        }

        // 尝试保存图像
        bool saved = image.save(fileName, format.toLatin1().constData());

        if (saved)
        {
            QMessageBox::information(this, tr("成功"), tr("文件已成功保存！"));
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr("文件未能保存！"));
        }
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    // 调用基类的resizeEvent()方法
    QWidget::resizeEvent(event);

    // 获取新的大小
    int picSize = std::min(ui->widget_2->height(), ui->widget_2->width()) - 14;
    if (picSize > 0) {
        // 设置新的大小和位置
        graphic->setFixedSize(picSize, picSize);
    }
}

void Widget::on_comboBox_LineColor_currentTextChanged(const QString &arg1)
{
    graphic->isLineColorRandom=false;
    if (arg1 == "渐变" || arg1 == "Gradient")
        graphic->isLineColorRandom=true;
    else if (arg1 == "白" || arg1 == "White")
        graphic->updatePen(Qt::white, ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), QColor());
    else if (arg1 == "黑" || arg1 == "Black")
        graphic->updatePen(Qt::black, ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), QColor());
    else if (arg1 == "自定义" || arg1 == "Customize")
    {
        // 获取当前项数
        int itemCount = ui->comboBox_LineColor->count();
        QString lastItemText = ui->comboBox_LineColor->itemText(itemCount - 1);

        // 显示颜色对话框
        QColor selectedColor = QColorDialog::getColor(Qt::white, this, tr("选择线条颜色"));
        if (selectedColor.isValid()) {
            // 检查最后一项是否为"自定义"
            if (lastItemText == "自定义" || lastItemText == "Customize") {
                // 不需要做任何操作
            } else {
                // 删除最后一项
                ui->comboBox_LineColor->removeItem(itemCount - 1);
            }
            // 创建表示颜色的字符串
            QString colorString = "rgba(" + QString::number(selectedColor.red()) + ", "
                                  + QString::number(selectedColor.green()) + ", "
                                  + QString::number(selectedColor.blue()) + ", "
                                  + QString::number(selectedColor.alpha()) + ")";

            // 添加新项
            ui->comboBox_LineColor->addItem(colorString);

            // 设置新添加的项为当前项
            ui->comboBox_LineColor->setCurrentIndex(ui->comboBox_LineColor->count() - 1);
        }
        else
            ui->comboBox_LineColor->setCurrentIndex(0);
    }
    else
    {
        QRegularExpression re("rgba\\((\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
        QRegularExpressionMatch match = re.match(arg1);

        // 提取红绿蓝alpha值
        int red = match.captured(1).toInt();
        int green = match.captured(2).toInt();
        int blue = match.captured(3).toInt();
        int alpha = match.captured(4).toInt();

        graphic->updatePen(QColor(red, green, blue, alpha), ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), QColor());
    }
}


void Widget::on_comboBox_LineStyle_currentTextChanged(const QString &arg1)
{
    if (arg1 == "自定义" || arg1 == "Customize")
    {
        // 如果arg1是“自定义”，弹出弹窗包含两个lineedit，用户输入数字后，检查ui->comboBox_LineStyle最后一项是否是自定义，
        // 如果不是删除最后一项。再在ui->comboBox_LineStyle新建形如 数字1->数字2 的选项，并选中这个选项

    }
    else
        graphic->updatePen(graphic->m_lineColor, arg1, ui->comboBox_LineThickness->currentText(), QColor());

}


void Widget::on_comboBox_LineThickness_currentTextChanged(const QString &arg1)
{
    graphic->updatePen(QColor(), ui->comboBox_LineStyle->currentText(), arg1, QColor());
}


void Widget::on_comboBox_BackgroundColor_currentTextChanged(const QString &arg1)
{
    if (arg1 == "黑" || arg1 == "Black")
        graphic->updatePen(QColor(), ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), Qt::black);
    else if (arg1 == "白" || arg1 == "White")
        graphic->updatePen(QColor(), ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), Qt::white);
    else if (arg1 == "透明" || arg1 == "Transparent")
        graphic->updatePen(QColor(), ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), Qt::transparent);
    else if (arg1 == "自定义" || arg1 == "Customize")
    {
        // 获取当前项数
        int itemCount = ui->comboBox_BackgroundColor->count();
        QString lastItemText = ui->comboBox_BackgroundColor->itemText(itemCount - 1);

        // 显示颜色对话框
        QColor selectedColor = QColorDialog::getColor(Qt::white, this, tr("选择背景颜色"));
        if (selectedColor.isValid()) {
            // 检查最后一项是否为"自定义"
            if (lastItemText == "自定义" || lastItemText == "Customize") {
                // 不需要做任何操作
            } else {
                // 删除最后一项
                ui->comboBox_BackgroundColor->removeItem(itemCount - 1);
            }
            // 创建表示颜色的字符串
            QString colorString = "rgba(" + QString::number(selectedColor.red()) + ", "
                                  + QString::number(selectedColor.green()) + ", "
                                  + QString::number(selectedColor.blue()) + ", "
                                  + QString::number(selectedColor.alpha()) + ")";

            // 添加新项
            ui->comboBox_BackgroundColor->addItem(colorString);

            // 设置新添加的项为当前项
            ui->comboBox_BackgroundColor->setCurrentIndex(ui->comboBox_BackgroundColor->count() - 1);
        }
        else
            ui->comboBox_BackgroundColor->setCurrentIndex(0);
    }
    else
    {
        QRegularExpression re("rgba\\((\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
        QRegularExpressionMatch match = re.match(arg1);

        // 提取红绿蓝alpha值
        int red = match.captured(1).toInt();
        int green = match.captured(2).toInt();
        int blue = match.captured(3).toInt();
        int alpha = match.captured(4).toInt();

        graphic->updatePen(QColor(), ui->comboBox_LineStyle->currentText(), ui->comboBox_LineThickness->currentText(), QColor(red, green, blue, alpha));
    }
}


void Widget::on_comboBox_Axis_currentTextChanged(const QString &arg1)
{
    graphic->setAxis(arg1.toInt());
}

