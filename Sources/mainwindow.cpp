#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "screenshot.h"
#include "smallscreenwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(windowTitle() + " " + qApp->applicationVersion());

    // Соединение соответствующего слота для контролирования данных, размещенных в буфере
    connect(qApp->clipboard(),
            SIGNAL(dataChanged()),
            this,
            SLOT(slotDataBufferControl()));

    // Настройка области оповещения
    InicializationTrayMenu();

    // Объявление переменных
    InicializationVariable();
    // Получение настроек
    GetSettings();
    // Настройка видимости виджитов, отвечающих за определение размеров снимаемой области
    SetEnabledWidgetSizeScreen();

    // Обновление снимка экрана
    slotUpdateScreenShot();


}

MainWindow::~MainWindow()
{
    encodedString.clear();
    appendText.clear();

    delete ui;
}

// Event

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    setWindowOpacity(0.0);
    QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0.0);
    if(ui->cbWindowOpacity->isChecked())
        animation->setEndValue(static_cast<double>(ui->hsWindowOpacity->value()) / 100.0);
    else
        animation->setEndValue(0.99);
    animation->start();

    // Определение поведения иконки в области оповещения
    ChangeIcon();
}

void MainWindow::resizeEvent(QResizeEvent * /* event */)
{
    ui->lScreenShot->setPixmap(screenShotDispaly.scaled(ui->tbScreenShot->size().width() - 50,
                                                        ui->tbScreenShot->size().height() - 60,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    key = event->key();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Проверка на получение как нажатия, так и отпускания клавиши
    if (event->key() != key)
        return;

    key = 0;

    switch(event->key()) {
        case Qt::Key_Escape:
            // Обработка нажатия Esc
            close();
            break;
        case Qt::Key_SysReq:
            // Обработка нажатия PrintScreen
            SaveScreenShot();
            break;
    // Обработчики других клавиш
    }
    update();
}

bool MainWindow::event(QEvent *event)
{
    QWidget::event(event);

    if(isAnimation)
    {
        double porog_opacity = 0.5;
        if(event->type() == QEvent::WindowActivate) {
            double opacity = static_cast<double>(ui->hsWindowOpacity->value()) / 100.0;
            QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
            animation->setDuration(500);
            animation->setStartValue(porog_opacity);
            if(ui->cbWindowOpacity->isChecked() && opacity > porog_opacity/* && windowOpacity() > 0.1*/) {
                animation->setEndValue(opacity);
                animation->start();
            }
            else if(!ui->cbWindowOpacity->isChecked() && (windowOpacity() - porog_opacity) < 0.01) {
                animation->setEndValue(0.99);
                animation->start();
            }

            return true;
        }

        if(event->type() == QEvent::WindowDeactivate) {
            double opacity = static_cast<double>(ui->hsWindowOpacity->value()) / 100.0;
            QPropertyAnimation* animation1 = new QPropertyAnimation(this, "windowOpacity");
            animation1->setDuration(500);
            animation1->setEndValue(porog_opacity);
            if(ui->cbWindowOpacity->isChecked() && opacity > porog_opacity) {
                animation1->setStartValue(opacity);
                animation1->start();
            }
            else if(!ui->cbWindowOpacity->isChecked()) {
                animation1->setStartValue(0.99);
                animation1->start();
            }

            return true;
        }
    }

    return QWidget::event(event);
}

void MainWindow::closeEvent(QCloseEvent * /* event */)
{
    SetSettings();

    QApplication::quit();
}

// End event

void MainWindow::GetSettings()
{
    //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
    QSettings settingsApp("NIIIS", "ScreenPrinting");

    settingsApp.beginGroup("SettingsExport");
    ui->leDirectory->setText(settingsApp.value("/Directory", "").toString());
    ui->leNameFile->setText(settingsApp.value("/NameFile", "").toString());
    ui->cbFormatFile->setCurrentIndex(settingsApp.value("/FormatFile", 0).toInt());
    ui->sbQuality->setValue(settingsApp.value("/Quality", 85).toInt());
    ui->cbIterationCalculusFileName->setChecked(settingsApp.value("/IterationCalculusFileName", false).toBool());
    ui->sbIterationCalculusFileName->setValue(settingsApp.value("/CountIterationCalculusFileName", 0).toInt());
    ui->cbDataTimeForNameFile->setChecked(settingsApp.value("/DataTimeForNameFile", false).toBool());
    settingsApp.endGroup();

    settingsApp.beginGroup("SettingsPrint");
    ui->rbNumberMonitor->setChecked(settingsApp.value("/NumberMonitor", true).toBool());
    ui->cbNumberMonitor->setCurrentIndex(settingsApp.value("/SetNumberMonitor", 0).toInt());
    ui->rbSizeRect->setChecked(settingsApp.value("/SizeRect", false).toBool());
    ui->sbX0Rect->setValue(settingsApp.value("/X0Rect", 0).toInt());
    ui->sbY0Rect->setValue(settingsApp.value("/Y0Rect", 0).toInt());
    ui->sbWidthRect->setValue(settingsApp.value("/WidthRect", 0).toInt());
    ui->sbHeightRect->setValue(settingsApp.value("/HeightRect", 0).toInt());
    ui->cbOrientation->setCurrentIndex(settingsApp.value("/Orientation", 0).toInt());
    ui->cbColorMode->setCurrentIndex(settingsApp.value("/ColorMode", 0).toInt());
    ui->cbPageSize->setCurrentIndex(settingsApp.value("/PageSize", 0).toInt());
    full_file_name = settingsApp.value("/FullFileName", "").toString();
    settingsApp.endGroup();

    settingsApp.beginGroup("SettingsApplication");
    ui->cbWindowStaysOnTopHint->setChecked(settingsApp.value("/WindowStaysOnTopHint", true).toBool());
    ui->cbWindowCenterDesktop->setChecked(settingsApp.value("/WindowCenterDesktop", true).toBool());
    ui->cbWindowOpacity->setChecked(settingsApp.value("/WindowOpacity", false).toBool());
    ui->hsWindowOpacity->setValue(settingsApp.value("/SetWindowOpacity", 99.0).toDouble());
    ui->cbCloseAppAfterComandNext->setChecked(settingsApp.value("/CloseAppAfterComandNext", true).toBool());
    settingsApp.endGroup();

    WindowStaysOnTop();
    WindowsCenterDesktop();

    ui->tbScreenShot->setItemText(0, encodedString[4] + ": " + full_file_name);
}

void MainWindow::SetSettings()
{
    //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
    QSettings settingsApp("NIIIS", "ScreenPrinting");

    settingsApp.beginGroup("SettingsExport");
    settingsApp.setValue("/Directory", ui->leDirectory->text());
    settingsApp.setValue("/NameFile", ui->leNameFile->text());
    settingsApp.setValue("/FormatFile", ui->cbFormatFile->currentIndex());
    settingsApp.setValue("/Quality", ui->sbQuality->value());
    settingsApp.setValue("/IterationCalculusFileName", ui->cbIterationCalculusFileName->isChecked());
    settingsApp.setValue("/CountIterationCalculusFileName", ui->sbIterationCalculusFileName->value());
    settingsApp.setValue("/DataTimeForNameFile", ui->cbDataTimeForNameFile->isChecked());
    settingsApp.endGroup();

    settingsApp.beginGroup("SettingsPrint");
    settingsApp.setValue("/NumberMonitor", ui->rbNumberMonitor->isChecked());
    settingsApp.setValue("/SetNumberMonitor", ui->cbNumberMonitor->currentIndex());
    settingsApp.setValue("/SizeRect", ui->rbSizeRect->isChecked());
    settingsApp.setValue("/X0Rect", ui->sbX0Rect->value());
    settingsApp.setValue("/Y0Rect", ui->sbY0Rect->value());
    settingsApp.setValue("/WidthRect", ui->sbWidthRect->value());
    settingsApp.setValue("/HeightRect", ui->sbHeightRect->value());
    settingsApp.setValue("/Orientation", ui->cbOrientation->currentIndex());
    settingsApp.setValue("/ColorMode", ui->cbColorMode->currentIndex());
    settingsApp.setValue("/PageSize", ui->cbPageSize->currentIndex());
    settingsApp.setValue("/FullFileName", full_file_name);
    settingsApp.endGroup();

    settingsApp.beginGroup("SettingsApplication");
    settingsApp.setValue("/WindowStaysOnTopHint", ui->cbWindowStaysOnTopHint->isChecked());
    settingsApp.setValue("/WindowCenterDesktop", ui->cbWindowCenterDesktop->isChecked());
    settingsApp.setValue("/WindowOpacity", ui->cbWindowOpacity->isChecked());
    settingsApp.setValue("/SetWindowOpacity", ui->hsWindowOpacity->value());
    settingsApp.setValue("/CloseAppAfterComandNext", ui->cbCloseAppAfterComandNext->isChecked());
    settingsApp.endGroup();
}

void MainWindow::InicializationTrayMenu()
{
    QIcon icon;
    // Объект действия для отображения/скрытия
    icon.addFile(qApp->applicationDirPath() + "/images/camera.png");
    QAction *pactShowHide = new QAction(icon, "&Показать/Скрыть приложение", this);
    connect(pactShowHide,
            SIGNAL(triggered()),
            this,
            SLOT(slotShowHide()));

    // Объект действия для отображения иконки
    icon.addFile(qApp->applicationDirPath() + "/images/screen.png");
    QAction *pactNewScreenShot = new QAction(icon, "&Сделать снимок экрана", this);
    connect(pactNewScreenShot,
            SIGNAL(triggered()),
            this,
            SLOT(slotBlindSaveScreenShot()));

    // Объект действия для выхода
    icon.addFile(qApp->applicationDirPath() + "/images/close.png");
    QAction *pactQuit = new QAction(icon,"&Закрыть", this);
    connect(pactQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    // Создание меню и наполнение объектами действий
    m_ptrayiconMenu = new QMenu(this);
    m_ptrayiconMenu->addAction(pactShowHide);
    m_ptrayiconMenu->addAction(pactNewScreenShot);
    m_ptrayiconMenu->addAction(pactQuit);

    // Объект области оповещения, который в качестве предка получает указатель this
    m_ptrayIcon = new QSystemTrayIcon(this);
    m_ptrayIcon->setContextMenu(m_ptrayiconMenu);
    m_ptrayIcon->setToolTip(windowTitle());

    // Добавление растрового изображения программы к области оповещения
    m_ptrayIcon->show();
}

void MainWindow::InicializationVariable()
{
    screenShotDispaly = QPixmap();

    isAnimation = false;

    ui->tbScreenShot->setCurrentIndex(0);

    codec = QTextCodec::codecForName("Windows-1251");

    // QPrinter::Orientation
    encodedString.append("Горизонтальная");
    encodedString.append("Вертикальная");
    // QPrinter::ColorMode
    encodedString.append("Цветной");
    encodedString.append("Черно-белый");
    // page
    encodedString.append("Снимок экрана");

    for(int i=0; i<encodedString.size(); i++)
    {
        appendText.append(codec->toUnicode(encodedString[i]));
    }
    // QPrinter::PdfFormat
    ui->cbFormatFile->addItems(QStringList() << "pdf" << "png" << "bmp" << "tiff" << "jpeg" << "ico");

    // QApplication::desktop()
    QString numberDesktop;
    for(int i=0; i<Screenshot::GetNumberDesktop(); i++)
    {
        numberDesktop = numberDesktop.setNum(i + 1) + " - " + Screenshot::GetSizeDesktop(i);
        ui->cbNumberMonitor->addItem(numberDesktop);
    }

    // QPrinter::Orientation
    ui->cbOrientation->addItems(QStringList() << appendText[0] << appendText[1]);
    // QPrinter::ColorMode
    ui->cbColorMode->addItems(QStringList() << appendText[2] << appendText[3]);
    // QPrinter::PageSize
    ui->cbPageSize->addItems(QStringList() << "A0" << "A1" << "A2" << "A3" << "A4" << "A5" << "A6" << "A7" << "A8" << "A9");
}

bool MainWindow::SaveScreenShot()
{
    isAnimation = false;

    QString file_name(ui->leDirectory->text() + ui->leNameFile->text());
    QDateTime date_time = QDateTime::currentDateTime();
    QSettings settingsApp("NIIIS", "ScreenPrinting");
    bool isResult = false;

    if(ui->cbIterationCalculusFileName->isChecked()) {
        settingsApp.beginGroup("SettingsExport");
        ui->sbIterationCalculusFileName->setValue(settingsApp.value("/CountIterationCalculusFileName", 0).toInt());
        settingsApp.endGroup();

        if(file_name.isEmpty() || file_name.isNull() || file_name == ui->leDirectory->text())
            file_name += QString::number(ui->sbIterationCalculusFileName->value());
        else
            file_name += "-" + QString::number(ui->sbIterationCalculusFileName->value());

        ui->sbIterationCalculusFileName->setValue(ui->sbIterationCalculusFileName->value() + 1);
    }

    if(ui->cbDataTimeForNameFile->isChecked()) {
        if(file_name.isEmpty() || file_name.isNull() || file_name == ui->leDirectory->text())
            file_name += date_time.toString("yyyymmdd_hhmmsszzz");
        else
            file_name += date_time.toString("__yyyymmdd_hhmmsszzz");
    }

    // В данном случае в имени есть только указание пути
    if(file_name == ui->leDirectory->text())
        file_name.clear();

    if(ui->cbFormatFile->currentText() == "pdf") {
        if(Screenshot::SaveScreenShotToPdf(screenShotDispaly,
                                           file_name,
                                           page_orientation,
                                           page_size,
                                           page_color)) {
            isResult = true;
        }
    }
    else {
        if(Screenshot::SaveScreenShotToImage(screenShotDispaly,
                                            file_name,
                                            ui->cbFormatFile->currentText(),
                                            ui->sbQuality->value())) {
            isResult = true;
        }
    }

    isAnimation = true;

    if(isResult) {
        full_file_name.clear();
        full_file_name.append(file_name.remove(0, ui->leDirectory->text().length()));

        ui->tbScreenShot->setItemText(0, encodedString[4] + ": " + full_file_name);

        //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
        QSettings settingsApp("NIIIS", "ScreenPrinting");

        settingsApp.beginGroup("SettingsExport");
        settingsApp.setValue("/CountIterationCalculusFileName", ui->sbIterationCalculusFileName->value());
        settingsApp.endGroup();

        settingsApp.beginGroup("SettingsPrint");
        settingsApp.setValue("/FullFileName", full_file_name);
        settingsApp.endGroup();

        return true;
    }

    return false;
}

// Slots

void MainWindow::slotActivateWindow()
{
    activateWindow();
}

void MainWindow::slotUpdateScreenShot()
{
    if(ui->rbSizeRect->isChecked())
        screenShotDispaly = Screenshot::GetScreenShot(ui->sbX0Rect->value(),
                                                      ui->sbY0Rect->value(),
                                                      ui->sbWidthRect->value(),
                                                      ui->sbHeightRect->value());

    if(ui->rbNumberMonitor->isChecked())
        screenShotDispaly = Screenshot::GetScreenShot(ui->cbNumberMonitor->currentIndex() - 1);

    ui->lScreenShot->setPixmap(screenShotDispaly.scaled(ui->tbScreenShot->size().width() - 50,
                                                        ui->tbScreenShot->size().height() - 60,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation));

    activateWindow();

    show();

    isAnimation = true;
}

void MainWindow::slotBlindSaveScreenShot()
{
    SetSettings();

    SmallScreenWindow *small_screen_window = new SmallScreenWindow(false);
    small_screen_window->show();

    //HKEY_CURRENT_USER\Software\NIIIS\ScreenPrinting
    QSettings settingsApp("NIIIS", "ScreenPrinting");

    settingsApp.beginGroup("SettingsExport");
    ui->sbIterationCalculusFileName->setValue(settingsApp.value("/CountIterationCalculusFileName", 0).toInt());
    settingsApp.endGroup();

    settingsApp.beginGroup("SettingsPrint");
    full_file_name = settingsApp.value("/FullFileName", "").toString();
    settingsApp.endGroup();

    ui->tbScreenShot->setItemText(0, encodedString[4] + ": " + full_file_name);
}

void MainWindow::slotDataBufferControl()
{
    QClipboard* pcb = QApplication::clipboard();

    QPixmap screen = pcb->pixmap();
    if(!screen.isNull())
        slotBlindSaveScreenShot();
}

void MainWindow::slotShowHide()
{
    setVisible(!isVisible());

    slotShowMessage();

    ChangeIcon();

    activateWindow();
}

void MainWindow::slotShowMessage()
{
    if(!isVisible())
        m_ptrayIcon->showMessage(windowTitle(),
                                 "Приложение скрыто в области оповещения",
                                 QSystemTrayIcon::Information,
                                 3000);
}

void MainWindow::on_ButtonCancel_clicked()
{
    close();
}

void MainWindow::on_ButtonNext_clicked()
{
    if(SaveScreenShot() && ui->cbCloseAppAfterComandNext->isChecked())
        close();
}

void MainWindow::on_ButtonUpdate_clicked()
{
    isAnimation = false;

    hide();

    QTimer::singleShot(500, this, SLOT(slotUpdateScreenShot()));

    ui->tbScreenShot->setCurrentIndex(0);
}

void MainWindow::on_rbNumberMonitor_clicked()
{
    SetEnabledWidgetSizeScreen();
}

void MainWindow::on_rbSizeRect_clicked()
{
    SetEnabledWidgetSizeScreen();
}

void MainWindow::on_ButtonSetDirectory_clicked()
{
    QString path_directory = QFileDialog::getExistingDirectory(0, "Directory", ui->leDirectory->text());
    ui->leDirectory->setText(path_directory.replace("/", "\\") + "\\");
}

void MainWindow::on_cbFormatFile_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "pdf")
    {
        ui->cbOrientation->setEnabled(true);
        ui->cbPageSize->setEnabled(true);
        ui->cbColorMode->setEnabled(true);

        ui->sbQuality->setEnabled(false);
    }
    else
    {
        ui->cbOrientation->setEnabled(false);
        ui->cbPageSize->setEnabled(false);
        ui->cbColorMode->setEnabled(false);

        ui->sbQuality->setEnabled(true);
    }
}

void MainWindow::on_cbOrientation_currentIndexChanged(const QString &arg1)
{
    if(arg1 == appendText[0])
        page_orientation = QPrinter::Landscape;

    if(arg1 == appendText[1])
        page_orientation = QPrinter::Portrait;
}

void MainWindow::on_cbColorMode_currentIndexChanged(const QString &arg1)
{
    if(arg1 == appendText[2])
        page_color = QPrinter::Color;

    if(arg1 == appendText[3])
        page_color = QPrinter::GrayScale;
}

void MainWindow::on_cbPageSize_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "A0")
        page_size = QPrinter::A0;

    if(arg1 == "A1")
        page_size = QPrinter::A1;

    if(arg1 == "A2")
        page_size = QPrinter::A2;

    if(arg1 == "A3")
        page_size = QPrinter::A3;

    if(arg1 == "A4")
        page_size = QPrinter::A4;

    if(arg1 == "A5")
        page_size = QPrinter::A5;

    if(arg1 == "A6")
        page_size = QPrinter::A6;

    if(arg1 == "A7")
        page_size = QPrinter::A7;

    if(arg1 == "A8")
        page_size = QPrinter::A8;

    if(arg1 == "A9")
        page_size = QPrinter::A9;
}

void MainWindow::on_cbWindowStaysOnTopHint_clicked()
{
    WindowStaysOnTop();
}

void MainWindow::on_cbWindowCenterDesktop_clicked()
{
    WindowsCenterDesktop();
}

void MainWindow::on_cbWindowOpacity_clicked()
{
    WindowOpacity();
}

void MainWindow::on_hsWindowOpacity_valueChanged(int /*value*/)
{
    WindowOpacity();
}

// End slots

// Settings function

void MainWindow::SetEnabledWidgetSizeScreen()
{
    if(ui->rbNumberMonitor->isChecked())
    {
        ui->cbNumberMonitor->setEnabled(true);

        ui->sbX0Rect->setEnabled(false);
        ui->sbY0Rect->setEnabled(false);
        ui->sbWidthRect->setEnabled(false);
        ui->sbHeightRect->setEnabled(false);
    }

    if(ui->rbSizeRect->isChecked())
    {
        ui->cbNumberMonitor->setEnabled(false);

        ui->sbX0Rect->setEnabled(true);
        ui->sbY0Rect->setEnabled(true);
        ui->sbWidthRect->setEnabled(true);
        ui->sbHeightRect->setEnabled(true);
    }
}

void MainWindow::ChangeIcon()
{
    QString strPixmapName;

    if(isVisible())
        strPixmapName = qApp->applicationDirPath() + "/images/camera.png";
    else
        strPixmapName = qApp->applicationDirPath() + "/images/camera_leave.png";

    m_ptrayIcon->setIcon(QPixmap(strPixmapName));
}

void MainWindow::WindowStaysOnTop()
{
    // Установка поведения форм
    Qt::WindowFlags flag = 0;

    if(ui->cbWindowStaysOnTopHint->isChecked()) {
        flag |=  Qt::Tool;// Удаление значка с taskbar и кнопок максимум, минимум размера окна
        flag |=  Qt::WindowStaysOnTopHint;// Всегда поверх других      
    }
    else {
        flag |=  Qt::Window;// Удаление значка с taskbar и кнопок максимум, минимум размера окна
    }

    setWindowFlags(flag);
    show();
    activateWindow();
}

void MainWindow::WindowsCenterDesktop()
{
    if(ui->cbWindowCenterDesktop->isChecked()) {
        hide();

        QRect frect = frameGeometry();
        frect.moveCenter(QDesktopWidget().availableGeometry().center());
        move(frect.topLeft());

        show();
        activateWindow();
    }
}

void MainWindow::WindowOpacity()
{
    if(ui->cbWindowOpacity->isChecked()) {
        ui->hsWindowOpacity->setEnabled(true);
        setWindowOpacity(static_cast<double>(ui->hsWindowOpacity->value()) / 100.0);
    }
    else {
        ui->hsWindowOpacity->setEnabled(false);
        setWindowOpacity(0.99);
    }
}

// End settings function
