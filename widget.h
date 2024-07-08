#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QTranslator>
#include <QShortcut>
#include <QFileDialog>
#include <QColorDialog>
#include <QFile>
#include <QImage>
#include <QMessageBox>
#include "graphic.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButton_Windowed_clicked();

    void on_pushButton_Switch_Language_clicked();

    void on_pushButton_Quit_clicked();

    void on_pushButton_Undo_clicked();

    void on_pushButton_Redo_clicked();

    void on_pushButton_Smooth_clicked();

    void on_pushButton_Circle_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Import_clicked();

    void on_pushButton_Export_clicked();

    void on_comboBox_LineColor_currentTextChanged(const QString &arg1);

    void on_comboBox_LineStyle_currentTextChanged(const QString &arg1);

    void on_comboBox_LineThickness_currentTextChanged(const QString &arg1);

    void on_comboBox_BackgroundColor_currentTextChanged(const QString &arg1);

    void on_comboBox_Axis_currentTextChanged(const QString &arg1);

private:
    void setComboBox();

    Ui::Widget *ui;
    bool isEnglish;
    Graphic *graphic;
};
#endif // WIDGET_H
