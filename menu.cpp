#include "menu.h"
#include "ui_menu.h"
#include "ui_settings.h"
#include "ui_info.h"

bool   scanOn          = false;         //scanning mode or not
bool   isLogin         = false;         //authorized or not
bool   ignoreHave      = true;          //if already have this game
int    browser_mode    = 0;             //1=login, 2=store, 3=market, 4=cart,5=after js
bool   ignoreNonProfit = true;          //show games without profit or not
bool   showLogin       = true;          //show login or not
double delay           = 2;             //delay for js loading

Menu::Menu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
    appSettings     = new QSettings("settings.ini", QSettings::IniFormat, this);
    loadSettings();
    settings        = new Settings(0);
    info            = new Info(0);
    browser         = new QWebView;
    progressBar     = new QProgressBar;
    timer           = new QTimer(this);
    timer->setSingleShot(true);
    browser->setGeometry(10,35, 640,480);
    browser->setWindowTitle("Steam Browser");
    ui->GamesTreeWidget->header()->resizeSection(0, 292);
    ui->GamesTreeWidget->setSortingEnabled(true);
    ui->statusBar->addPermanentWidget(progressBar);
    ui->statusBar->showMessage(tr("Ready to work"));
    ui->button_stop->hide();
    ui->button_start->setEnabled(false);
    //ui->menuBar->hide();

    connect(timer, SIGNAL(timeout()), this, SLOT(time_out()));
    connect(browser, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
    connect(browser, SIGNAL(loadFinished(bool)), SLOT(loadFinished(bool)));
    connect(settings, SIGNAL(updateSettings()), this, SLOT(saveSettings()));

    changeTranslator(QLocale::system().name());
}

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->checkBox_library->setChecked(ignoreHave);
    ui->checkBox_login->setChecked(showLogin);
    ui->checkBox_profit->setChecked(ignoreNonProfit);
    ui->doubleSpinBox_hold->setValue(delay);
}

Info::Info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
}
//////////////////////////////////////////////////////////////////////////////////
Menu::~Menu()
{
    delete ui;
}
//------------------------------------------------------------
Settings::~Settings()
{
    delete ui;
}
//------------------------------------------------------------
Info::~Info()
{
    delete ui;
}
//------------------------------------------------------------
void Menu::parseToTree(QString name, double price,            //parsing to Games Tree Widget
                       double gain, double profit,
                       QString id)
{
    QTreeWidgetItem* pgame = new QTreeWidgetItem(ui->GamesTreeWidget);
    pgame->setText(0, name);
    pgame->setData(1, Qt::DisplayRole, QVariant(price));
    pgame->setData(2, Qt::DisplayRole, QVariant(gain));
    pgame->setData(3, Qt::DisplayRole, QVariant(profit));
    pgame->setText(4, id);

    pgame->setFlags(pgame->flags() | Qt::ItemIsUserCheckable);
    pgame->setCheckState(0, Qt::Unchecked);
    ui->statusBar->showMessage(tr("Processed") + QString::number(checkedGames) + tr(" of ") + countOfGames);
    on_button_start_clicked();
}
//------------------------------------------------------------
void Menu::parseToCart()                                      //buying games
{
    if(cart.isEmpty() && countCart == 0)
        return;
    else if(cart.isEmpty() && countCart > 0)
    {
        browser_mode = 0;
        browser->load(QUrl("http://store.steampowered.com/cart/"));
        browser->show();
    }
    else
    {
        QString cartId = cart.dequeue();
        QString cartUrl = "http://store.steampowered.com/app/" + cartId;
        ui->statusBar->showMessage(tr("Adding games into the cart ") + QString::number(++countCart) + tr(" of ") + QString::number(amountCart));
        browser_mode = 4;
        browser->load(QUrl(cartUrl));
    }
}
//-------------------------------------------------------------
void Menu::saveSettings()                                      //save settings
{
    appSettings->setValue("ignoreHave", ignoreHave);
    appSettings->setValue("ignoreNonProfit", ignoreNonProfit);
    appSettings->setValue("showLogin", showLogin);
    appSettings->setValue("delay", delay);

    if(isLogin)
    {
        if(showLogin)
            ui->label_login->setText(tr("Connected as ") + stringLogin);
        else
            ui->label_login->setText(tr("Connected"));
    }
}
//-------------------------------------------------------------
void Menu::loadSettings()                                      //load settings
{
    ignoreHave       = appSettings->value("ignoreHave",      true).toBool();
    ignoreNonProfit  = appSettings->value("ignoreNonProfit", true).toBool();
    showLogin        = appSettings->value("showLogin",       true).toBool();
    delay            = appSettings->value("delay",              2).toDouble();
}
//------------------------------------------------------------
void Menu::changeTranslator(QString postfix)                  //language changes
{
    QApplication::removeTranslator(translator);
    translator = new QTranslator(this);
    translator->load(QApplication::applicationName() + "_" + postfix);
    QApplication::installTranslator(translator);
    ui->retranslateUi(this);
    delete settings;
    settings = new Settings(0);
    delete info;
    info = new Info(0);
}
//------------------------------------------------------------
void Menu::on_button_login_clicked()                          //login button
{
    browser_mode = 1;
    browser->show();
    browser->load(QUrl("https://steamcommunity.com/login/"));
    ui->statusBar->showMessage(tr("Authorization..."));
}
//-------------------------------------------------------------
void Menu::loadFinished(bool bOk)                              //browser loading finished slot
{
    if (!bOk)
    {
        browser->setHtml(tr("<CENTER>There is something wrong"
                               "<br>Try to repeat.</CENTER>"
                               ));
        ui->statusBar->showMessage(tr("Error, browser did not download the page"));
    }
    else if(bOk && browser_mode == 1)                                    //login mode
    {
        stringLogin = browser->page()->mainFrame()->findFirstElement("div[id=header_notification_area]").nextSibling().toPlainText();
        if(stringLogin.isNull())
        {
            isLogin = false;
            ui->statusBar->showMessage(tr("You are not authorized!Please do it!"));
        }
        else
        {
            isLogin = true;
            browser->hide();
            ui->button_start->setEnabled(true);
            if(showLogin)
                ui->label_login->setText(tr("Connected as ") + stringLogin);
            else
                ui->label_login->setText(tr("Connected"));
            ui->label_login->setStyleSheet("QLabel { color : green; }");
            ui->statusBar->showMessage(tr("You are successful authorized!"));
            ui->button_login->hide();
        }
    }
    else if(bOk && browser_mode > 1)

    {
       timer->stop();
       timer->start(delay * 1000);
    }
}
//-------------------------------------------------------------
void Menu::on_button_start_clicked()                          //start scanning
{
    if(!scanOn)
    {
        browser->hide();
        ui->button_start->hide();
        ui->button_stop->show();
        ui->button_buyMarkable->setEnabled(false);
        list.clear();
        cart.clear();
        checkedGames = 0;
        ui->GamesTreeWidget->clear();
        pageNumber = 0;
        maxPage = 0;
        scanOn = true;
    }
    if(pageNumber <= maxPage)
    {
        if(list.isEmpty())
        {
            browser_mode = 2;
            QString page(QString::number(++pageNumber));
            browser->load(QUrl("http://store.steampowered.com/search/?filter=ut2&category1=998&os=#sort_by=Price_ASC&category2=29&page=" + page));
            ui->statusBar->showMessage(tr("Checking games from store page"));
        }
        else
        {
            gameId = list.dequeue();
            QString marketUrl = "http://steamcommunity.com/market/search?q=&category_753_Game%5B%5D=tag_app_" + gameId + "&category_753_cardborder%5B%5D=tag_cardborder_0&category_753_item_class%5B%5D=tag_item_class_2#p1_price_asc";
            browser_mode = 3;
            browser->load(QUrl(marketUrl));
        }
    }
}
//-----------------------------------------------------------------
void Menu::time_out()                                              //processing web page after delay
{
    if(browser_mode == 2)                               //store mode
    {
        countOfGames = browser->page()->mainFrame()->findFirstElement("div[class=search_pagination_left]").toPlainText();
        countOfGames = countOfGames.section(' ', 5, 5); //total number of games

        maxPage = browser->page()->mainFrame()->findFirstElement("div[class=search_pagination_right]").lastChild().previousSibling().toPlainText().toInt();

        QWebElementCollection all_games = browser->page()->mainFrame()->findAllElements("a[class~=search_result_row]");
        foreach(QWebElement singleGame, all_games)
        {
            if((singleGame.lastChild().attribute("class") == "ds_flag ds_owned_flag") && ignoreHave )    //if already have this game
            {
                checkedGames++;
                continue;
            }
            QWebElement name = singleGame.findFirst("div[class=responsive_search_name_combined]").findFirst("div[class~=search_name]").findFirst("span[class=title]");
            singleGame.findFirst("div[class=responsive_search_name_combined]").findFirst("div[class~=search_price_discount_combined]").findFirst("div[class~=search_price]").firstChild().removeFromDocument();
            singleGame.findFirst("div[class=responsive_search_name_combined]").findFirst("div[class~=search_price_discount_combined]").findFirst("div[class~=search_price]").firstChild().removeFromDocument();
            QWebElement price = singleGame.findFirst("div[class=responsive_search_name_combined]").findFirst("div[class~=search_price_discount_combined]").findFirst("div[class~=search_price]");

            list.enqueue(singleGame.attribute("data-ds-appid"));
            list.enqueue(name.toPlainText());
            list.enqueue(price.toPlainText());

        }
        on_button_start_clicked();
    }
    else if(browser_mode == 3)                              //market_mode
    {
        QString popName = list.dequeue();
        QString popPrice = list.dequeue();
        QString priceString, amountString, lotString;
        QWebElement webElement = browser->page()->mainFrame()->findFirstElement("span[class=normal_price]");

        priceString = webElement.toPlainText();
        priceString.remove(QRegExp("[^0-9,.]"));
        priceString.replace(",", ".");
        if(priceString.endsWith("."))
            priceString.chop(1);
        double cardPrice = priceString.toDouble();

        webElement = browser->page()->mainFrame()->findFirstElement("span[id=searchResults_total]");
        amountString = webElement.toPlainText();
        qreal double_amount = amountString.toDouble();
        double amount = qRound(double_amount / 2);
        double gain = cardPrice * amount;
        gain = gain - (gain * 0.05);
        popPrice.remove(QRegExp("[^0-9,.]"));
        popPrice.replace(",", ".");
        if(popPrice.endsWith("."))
            popPrice.chop(1);
        double gamePrice = popPrice.toDouble();

        lotString = browser->page()->mainFrame()->findFirstElement("span[class=market_listing_num_listings_qty]").toPlainText();
        lotString.remove(QRegExp("[^0-9]"));
        double lots = lotString.toDouble();

        double profit = gain - gamePrice;
        checkedGames++;
        if(ignoreNonProfit && profit < 0)
            on_button_start_clicked();
        else
        parseToTree(popName, gamePrice, lots, profit, gameId);
    }
    else if(browser_mode == 4)
    {
       QString js = browser->page()->mainFrame()->findFirstElement("div[class=btn_addtocart]").firstChild().attribute("href");
       if(js.isEmpty())             //if cannot buy a game
           parseToCart();           //go next
       js.remove(0, 11);
       browser_mode = 5;
       browser->page()->mainFrame()->evaluateJavaScript(js);
    }
    else if(browser_mode == 5)
    {
        parseToCart();
    }
}
//-----------------------------------------------------------------------------------
void Menu::on_GamesTreeWidget_itemClicked(QTreeWidgetItem *item, int column)    //game click info
{
    if(column == 1) //game price
    {
        QString clickGameId = item->text(4);
        QString clickStoreUrl = "http://store.steampowered.com/app/" + clickGameId;
        QDesktopServices::openUrl(QUrl(clickStoreUrl));
    }
    else if(column == 2)    //card price
    {
        QString clickCardId = item->text(4);
        QString clickMarketUrl = "http://steamcommunity.com/market/search?q=&category_753_Game%5B%5D=tag_app_" + clickCardId + "&category_753_cardborder%5B%5D=tag_cardborder_0&category_753_item_class%5B%5D=tag_item_class_2#p1_price_asc";
        QDesktopServices::openUrl(QUrl(clickMarketUrl));
    }
}
//----------------------------------------------------------------------------------------
void Menu::on_button_markAll_clicked()                                                  //set all checked
{
    QTreeWidgetItem* itm;
    for(int i = 0; i < ui->GamesTreeWidget->topLevelItemCount(); i++)
    {
        itm = ui->GamesTreeWidget->topLevelItem(i);
        itm->setCheckState(0, Qt::Checked);
    }
}
//-----------------------------------------------------------------------------------------
void Menu::on_button_unmark_clicked()                                                   //set all unchecked
{
    QTreeWidgetItem* itm;
    for(int i = 0; i < ui->GamesTreeWidget->topLevelItemCount(); i++)
    {
        itm = ui->GamesTreeWidget->topLevelItem(i);
        itm->setCheckState(0, Qt::Unchecked);
    }
}
//-----------------------------------------------------------------------------------------
void Menu::on_button_deleteMarkable_clicked()                                           //delete markable games
{
    QTreeWidgetItem* itm;
    for(int i = 0; i < ui->GamesTreeWidget->topLevelItemCount(); i++)
    {
        itm = ui->GamesTreeWidget->topLevelItem(i);
        if(itm->checkState(0) == Qt::Checked)
        {
            itm->~QTreeWidgetItem();
            i--;
        }
    }
}
//------------------------------------------------------------------------------------------
void Menu::on_button_buyMarkable_clicked()                                                //add to cart markable games
{
    QTreeWidgetItem* itm;
    for(int i = 0; i < ui->GamesTreeWidget->topLevelItemCount(); i++)
    {
        itm = ui->GamesTreeWidget->topLevelItem(i);
        if(itm->checkState(0) == Qt::Checked)
        {
            cart.enqueue(itm->text(4));
        }
    }
    countCart = 0;
    amountCart = cart.count();
    parseToCart();
}
//---------------------------------------------------------
void Menu::on_button_stop_clicked()                         //stop scan
{
    if(scanOn)
    {
        timer->stop();
        browser->stop();
        ui->button_stop->hide();
        ui->button_start->show();
        ui->button_buyMarkable->setEnabled(true);
        scanOn = false;
    }
}
//-----------------------------------------------------------
void Menu::on_button_countMarkable_clicked()                //count all markable
{
    ui->label_spend->clear();
    ui->label_profit->clear();
    ui->label_gain->clear();
    QTreeWidgetItem* itm;
    for(int i = 0; i < ui->GamesTreeWidget->topLevelItemCount(); i++)
    {
        itm = ui->GamesTreeWidget->topLevelItem(i);
        if(itm->checkState(0) == Qt::Checked)
        {
            double totalSpend = ui->label_spend->text().toDouble();
            double currentSpend = itm->data(1, Qt::DisplayRole).toDouble();
            totalSpend += currentSpend;
            ui->label_spend->setText(QString::number(totalSpend, 'f', 2));

            double totalProfit = ui->label_profit->text().toDouble();
            double currentProfit = itm->data(3, Qt::DisplayRole).toDouble();
            totalProfit += currentProfit;
            ui->label_profit->setText(QString::number(totalProfit, 'f', 2));

            double gain = totalSpend + totalProfit;
            ui->label_gain->setText(QString::number(gain, 'f', 2));
        }
    }
}
//----------------------------------------------------------------
void Menu::on_action_settings_triggered()                       //open settings
{
    settings->show();
}
//----------------------------------------------------------------
void Settings::on_pushButton_saveSettings_clicked()             //save settings button
{
    if(ui->checkBox_library->isChecked())
        ignoreHave = true;
    else
        ignoreHave = false;

    if(ui->checkBox_profit->isChecked())
        ignoreNonProfit = true;
    else
        ignoreNonProfit = false;

    if(ui->checkBox_login->isChecked())
        showLogin = true;
    else
        showLogin = false;

    delay = ui->doubleSpinBox_hold->value();

    emit updateSettings();
    close();
}
//-------------------------------------------------------------------
void Menu::on_actionEnglish_triggered()                            //english selected
{
    changeTranslator("en");
}
//-------------------------------------------------------------------
void Menu::on_actionRussian_triggered()                            //russian selected
{
    changeTranslator("ru");
}
//------------------------------------------------------------------
void Menu::on_actionInformation_triggered()                       //information about app
{
    info->show();
}
