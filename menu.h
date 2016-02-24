#pragma once

#include <QMainWindow>
#include <QWebView>
#include <QWebHistory>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QProgressBar>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QQueue>
#include <QString>
#include <QTimer>
#include <QDesktopServices>
#include <QSettings>
#include <QWidget>
#include <QTranslator>
#include <QLocale>
#include "info.h"



namespace Ui {
class Menu;
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

signals:
    void updateSettings();

private:
    Ui::Settings *ui;
};

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();
    void        parseToTree     (QString, double, double, double, QString);
    void        parseToCart     (                                        );
    void        loadSettings    (                                        );
    void        changeTranslator(QString                                 );

private:
    Ui::Menu *ui;
    QTranslator*        translator;
    Settings*           settings;
    Info*               info;
    QWebView*           browser;
    QProgressBar*       progressBar;
    QQueue<QString>     list;
    QQueue<QString>     cart;
    QString             stringLogin;
    QString             gameId;
    QString             countOfGames;
    QTimer*             timer;
    QSettings*          appSettings;
    int                 checkedGames = 0;
    int                 pageNumber = 0;
    int                 maxPage = 0;
    int                 amountCart = 0;
    int                 countCart = 0;

private slots:
    void on_button_login_clicked();
    void loadFinished(bool);
    void on_button_start_clicked();
    void time_out();
    void on_GamesTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_button_markAll_clicked();
    void on_button_unmark_clicked();
    void on_button_deleteMarkable_clicked();
    void on_button_buyMarkable_clicked();
    void on_button_stop_clicked();
    void on_button_countMarkable_clicked();
    void on_action_settings_triggered();
    void saveSettings();
    void on_actionEnglish_triggered();
    void on_actionRussian_triggered();
    void on_actionInformation_triggered();
};
