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
    ui->comboBox->setCurrentText(tr("渐变"));
    ui->comboBox_2->setCurrentText(tr("实线"));
    ui->comboBox_3->setCurrentText(tr("渐变"));
    ui->comboBox_4->setCurrentText(tr("黑"));
    ui->comboBox_5->setCurrentText("8");

    QGridLayout *layout = new QGridLayout(ui->widget_2);
    int picSize = std::min(ui->widget_2->height(), ui->widget_2->width()) - 14;
    if (picSize > 0) {
        // 设置新的大小和位置
        graphic->setFixedSize(picSize, picSize);
    }
    layout->addWidget(graphic);

    graphic->updatePen(ui->comboBox->currentText(),ui->comboBox_2->currentText(),ui->comboBox_3->currentText());

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
    QString currentOption = ui->comboBox->currentText();
    ui->comboBox->clear();
    ui->comboBox->addItem(tr("渐变"));
    ui->comboBox->addItem(tr("炫光"));
    ui->comboBox->addItem(tr("自定义"));
    ui->comboBox->setCurrentText(currentOption);

    currentOption = ui->comboBox_2->currentText();
    ui->comboBox_2->clear();
    ui->comboBox_2->addItem(tr("实线"));
    ui->comboBox_2->addItem(tr("擦除线"));
    ui->comboBox_2->setCurrentText(currentOption);

    currentOption = ui->comboBox_3->currentText();
    ui->comboBox_3->clear();
    ui->comboBox_3->addItem(tr("渐变"));
    for(int i = 1; i <= 30; ++i){
        ui->comboBox_3->addItem(QString::number(i));
    }
    ui->comboBox_3->setCurrentText(currentOption);

    currentOption = ui->comboBox_4->currentText();
    ui->comboBox_4->clear();
    ui->comboBox_4->addItem(tr("黑"));
    ui->comboBox_4->addItem(tr("白"));
    ui->comboBox_4->addItem(tr("透明"));
    ui->comboBox_4->addItem(tr("自定义"));
    ui->comboBox_4->setCurrentText(currentOption);

    currentOption = ui->comboBox_5->currentText();
    ui->comboBox_5->clear();
    for(int i = 1; i <= 16; ++i){
        ui->comboBox_5->addItem(QString::number(i));
    }
    ui->comboBox_5->setCurrentText(currentOption);
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
            // 获取当前Graphic对象的大小
            QSize graphicSize = graphic->size();

            // 缩放图片以适应Graphic对象的大小
            pixmap = pixmap.scaled(graphicSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            // 清除原有的场景
            graphic->scene()->clear();

            // 在场景中添加新的缩放后的图片
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
            graphic->scene()->addItem(item);

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

