#include "smallscreenwindow.h"
#include "ui_smallscreenwindow.h"

#include "screenshot.h"

SmallScreenWindow::SmallScreenWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmallScreenWindow)
{
    ui->setupUi(this);

    // Inicialization window form: set style
    InicializationWindow();
    // Inicialization variable
    InicializationVariable();
    // Update screen shot desktop
    UpdateScreenShot();
    // Animation window form
    AnimationShowWindow();
    // Save screen shot
    SaveScreenShot();

    QTimer::singleShot(2000, this, SLOT(AnimationHideWindow()));
}

SmallScreenWindow::SmallScreenWindow(bool isClose) :
    ui(new Ui::SmallScreenWindow)
{
    ui->setupUi(this);

    this->isClose = isClose;

    // Inicialization window form: set style
    InicializationWindow();
    // Inicialization variable
    InicializationVariable();
    // Update screen shot desktop
    UpdateScreenShot();
    // Animation window form
    AnimationShowWindow();
    // Save screen shot
    SaveScreenShot();

    QTimer::singleShot(2000, this, SLOT(AnimationHideWindow()));
}

SmallScreenWindow::~SmallScreenWindow()
{
    delete ui;
}

void SmallScreenWindow::closeEvent(QCloseEvent * /*event*/)
{
    qApp->quit();
}

void SmallScreenWindow::on_ButtonClose_clicked()
{
    close();
}

void SmallScreenWindow::InicializationVariable()
{
    screenShotDispaly = QPixmap();

    ui->lMenu->setText(qApp->applicationName() + ":" + qApp->applicationVersion());
}

void SmallScreenWindow::InicializationWindow()
{
    // Установка поведения форм
    Qt::WindowFlags flag = 0;
    flag |=  Qt::SplashScreen;// Удаление значка с taskbar и кнопок максимум, минимум размера окна
    flag |=  Qt::WindowStaysOnTopHint;// Всегда поверх других
    setWindowFlags(flag);
}

void SmallScreenWindow::AnimationShowWindow()
{
    QRect frect = frameGeometry();
    frect.moveBottomRight(QDesktopWidget().availableGeometry().bottomRight());
    move(frect.left() - 30, frect.top() - 30);

    QPropertyAnimation* animation = new QPropertyAnimation(window(), "geometry");

    animation->setDuration(400);
    animation->setStartValue(QRect(frect.left() - 30, frect.bottom() - 30, 300, 0));
    animation->setEndValue(QRect(frect.right() - 330, frect.bottom() - 280, 300, 250));
    animation->start();
}

void SmallScreenWindow::AnimationHideWindow()
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");

    animation->setDuration(2000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();

    connect(animation, SIGNAL(finished()), this, SLOT(hide()));

    if(this->isClose)
        QTimer::singleShot(2000, this, SLOT(close()));
}

void SmallScreenWindow::UpdateScreenShot()
{
    //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
    QSettings settingsApp("NIIIS", "ScreenPrinting");

    settingsApp.beginGroup("SettingsPrint");
    bool numberMonitor = settingsApp.value("/NumberMonitor", true).toBool();
    int setNumberMonitor = settingsApp.value("/SetNumberMonitor", 0).toInt();
    bool sizeRect = settingsApp.value("/SizeRect", false).toBool();
    int x0Rect = settingsApp.value("/X0Rect", 0).toInt();
    int y0Rect = settingsApp.value("/Y0Rect", 0).toInt();
    int widthRect = settingsApp.value("/WidthRect", 0).toInt();
    int heightRect = settingsApp.value("/HeightRect", 0).toInt();
    settingsApp.endGroup();

    if(sizeRect)
        screenShotDispaly = Screenshot::GetScreenShot(x0Rect,
                                                      y0Rect,
                                                      widthRect,
                                                      heightRect);

    if(numberMonitor)
        screenShotDispaly = Screenshot::GetScreenShot(setNumberMonitor - 1);

    ui->lScreenShot->setPixmap(screenShotDispaly.scaled(ui->lScreenShot->size().width() - 0,
                                                        ui->lScreenShot->size().height() - 0,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation));
}

void SmallScreenWindow::SaveScreenShot()
{
    //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
    QSettings settingsApp("NIIIS", "ScreenPrinting");

    settingsApp.beginGroup("SettingsExport");
    QString directory(settingsApp.value("/Directory", "").toString());
    QString nameFile(settingsApp.value("/NameFile", "").toString());
    int formatFile = settingsApp.value("/FormatFile", 0).toInt();
    int quality = settingsApp.value("/Quality", 85).toInt();
    bool iterationCalculusFileName = settingsApp.value("/IterationCalculusFileName", false).toBool();
    int countIterationCalculusFileName = settingsApp.value("/CountIterationCalculusFileName", 0).toInt();
    bool dataTimeForNameFile = settingsApp.value("/DataTimeForNameFile", false).toBool();
    settingsApp.endGroup();

    settingsApp.beginGroup("SettingsPrint");
    int orientation = settingsApp.value("/Orientation", 0).toInt();
    int colorMode = settingsApp.value("/ColorMode", 0).toInt();
    int pageSize = settingsApp.value("/PageSize", 0).toInt();
    settingsApp.endGroup();

    QString file_name(directory + nameFile);
    QDateTime date_time = QDateTime::currentDateTime();
    // Формат файла
    QString format_file;
    // Полное имя файла без пути к файлу
    QString full_file_name;
    // Ориентация страницы
    QPrinter::Orientation page_orientation;
    // Цветовой режим страницы
    QPrinter::ColorMode page_color;
    // Размер страницы
    QPrinter::PageSize page_size;
    // Результат сохранения
    bool isResult = false;

    switch (formatFile)
    {
        case 0:
            format_file = "pdf";
            break;
        case 1:
            format_file = "png";
            break;
        case 2:
            format_file = "bmp";
            break;
        case 3:
            format_file = "tiff";
            break;
        case 4:
            format_file = "jpeg";
            break;
        case 5:
            format_file = "ico";
            break;
        default:
            format_file = "jpeg";
            break;
    }

    switch (orientation)
    {
        case 0:
            page_orientation = QPrinter::Landscape;
            break;
        case 1:
            page_orientation = QPrinter::Portrait;
            break;
        default:
            page_orientation = QPrinter::Landscape;
            break;
    }

    switch (colorMode)
    {
        case 0:
            page_color = QPrinter::Color;
            break;
        case 1:
            page_color = QPrinter::GrayScale;
            break;
        default:
            page_color = QPrinter::Color;
            break;
    }

    switch (pageSize)
    {
        case 0:
            page_size = QPrinter::A0;
            break;
        case 1:
            page_size = QPrinter::A1;
            break;
        case 2:
            page_size = QPrinter::A2;
            break;
        case 3:
            page_size = QPrinter::A3;
            break;
        case 4:
            page_size = QPrinter::A4;
            break;
        case 5:
            page_size = QPrinter::A5;
            break;
        case 6:
            page_size = QPrinter::A6;
            break;
        case 7:
            page_size = QPrinter::A7;
            break;
        case 8:
            page_size = QPrinter::A8;
            break;
        case 9:
            page_size = QPrinter::A9;
            break;
        default:
            page_size = QPrinter::A4;
            break;
    }

    if(iterationCalculusFileName) {
        if(file_name.isEmpty() || file_name.isNull() || file_name == directory)
            file_name += QString::number(countIterationCalculusFileName);
        else
            file_name += "-" + QString::number(countIterationCalculusFileName);

        countIterationCalculusFileName += 1;
    }

    if(dataTimeForNameFile) {
        if(file_name.isEmpty() || file_name.isNull() || file_name == directory)
            file_name += date_time.toString("yyyymmdd_hhmmsszzz");
        else
            file_name += date_time.toString("__yyyymmdd_hhmmsszzz");
    }

    // В данном случае в имени есть только указание пути
    if(file_name == directory)
        file_name.clear();

    if(format_file == "pdf") {
        if(Screenshot::SaveScreenShotToPdf(screenShotDispaly,
                                           file_name,
                                           page_orientation,
                                           page_size,
                                           page_color)) {
            isResult = true;
        }
    }
    else
    {
        if(Screenshot::SaveScreenShotToImage(screenShotDispaly,
                                            file_name,
                                            format_file,
                                            quality)) {
            isResult = true;
        }
    }

    if(isResult) {
        ui->lFileName->setText(file_name);
        ui->lDateTime->setText(date_time.toString());

        full_file_name.append(file_name.remove(0, directory.length()));

        //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
        QSettings settingsApp("NIIIS", "ScreenPrinting");

        settingsApp.beginGroup("SettingsExport");
        settingsApp.setValue("/CountIterationCalculusFileName", countIterationCalculusFileName);
        settingsApp.endGroup();

        settingsApp.beginGroup("SettingsPrint");
        settingsApp.setValue("/FullFileName", full_file_name);
        settingsApp.endGroup();
    }
}
