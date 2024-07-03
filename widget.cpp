#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    isEnglish = false;

    showFullScreen();

    ui->comboBox->addItem(tr("渐变"));
    ui->comboBox->addItem(tr("炫光"));
    ui->comboBox->addItem(tr("自定义"));
    ui->comboBox->setCurrentText(tr("渐变"));

    ui->comboBox_2->addItem(tr("实线"));
    ui->comboBox_2->addItem(tr("擦除线"));
    ui->comboBox_2->setCurrentText(tr("实线"));

    ui->comboBox_3->addItem(tr("渐变"));
    for(int i = 1; i <= 30; ++i){
        ui->comboBox_3->addItem(QString::number(i));
    }
    ui->comboBox_3->setCurrentText(tr("渐变"));

    ui->comboBox_4->addItem(tr("白"));
    ui->comboBox_4->addItem(tr("黑"));
    ui->comboBox_4->addItem(tr("透明"));
    ui->comboBox_4->addItem(tr("自定义"));
    ui->comboBox_4->setCurrentText(tr("白"));

    for(int i = 1; i <= 16; ++i){
        ui->comboBox_5->addItem(QString::number(i));
    }
    ui->comboBox_5->setCurrentText("8");

    // 创建QShortcut对象，第一个参数是快捷键，第二个参数是父对象，第三个参数是槽函数
    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    connect(shortcut1, &QShortcut::activated, this, &Widget::on_pushButton_2_clicked);

    QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z), this);
    connect(shortcut2, &QShortcut::activated, this, &Widget::on_pushButton_3_clicked);

    QShortcut *shortcut3 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(shortcut3, &QShortcut::activated, this, &Widget::on_pushButton_4_clicked);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
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
    on_pushButton_5_clicked();
}

void Widget::on_pushButton_5_clicked()
{
    if (!isEnglish) {
        // 加载英文翻译文件
        QTranslator translator;
        translator.load(":/i18n/Magical_Circle_en_US");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        if (!isFullScreen()) {
            ui->pushButton->setText(tr("全屏"));
        }
    } else {
        // 卸载英文翻译文件
        QList<QTranslator*> translators = qApp->findChildren<QTranslator*>();
        for (QTranslator *translator : translators) {
            qApp->removeTranslator(translator);
        }
        ui->retranslateUi(this);
        if (!isFullScreen()) {
            ui->pushButton->setText(tr("全屏"));
        }
    }
    isEnglish = !isEnglish;
}



void Widget::on_pushButton_9_clicked()
{
    this->close();
}


void Widget::on_pushButton_2_clicked()
{

}


void Widget::on_pushButton_3_clicked()
{

}


void Widget::on_pushButton_4_clicked()
{

}

