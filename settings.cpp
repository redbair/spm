#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->checkBox_library->setChecked(Menu::ignoreHave);
    ui->checkBox_login->setChecked(Menu::showLogin);
    ui->checkBox_profit->setChecked(Menu::ignoreNonProfit);
    ui->doubleSpinBox_hold->setValue(Menu::delay);
}

Settings::~Settings()
{
    delete ui;
}
//----------------------------------------------------------------
void Settings::on_pushButton_saveSettings_clicked()
{

}
//----------------------------------------------------------------
void Settings::on_pushButton_resetSettings_clicked()
{

}
