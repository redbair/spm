#pragma once
#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private slots:
    void on_pushButton_saveSettings_clicked();

    void on_pushButton_resetSettings_clicked();

private:
    Ui::Settings *ui;
};

