#include "screenshot.h"

QPixmap Screenshot::GetScreenShot(int numberDesktop)
{
    QPixmap screenShot = QPixmap();

    screenShot = QPixmap::grabWindow(QApplication::desktop()->screen(numberDesktop)->winId(),
                                     QApplication::desktop()->screenGeometry(numberDesktop).left(),
                                     QApplication::desktop()->screenGeometry(numberDesktop).top(),
                                     -1,
                                     -1);

    return screenShot;
}

QPixmap Screenshot::GetScreenShot(int x0Position,
                                  int y0Position,
                                  int width,
                                  int height)
{
    QPixmap screenShot = QPixmap();

    screenShot = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                     x0Position,
                                     y0Position,
                                     width,
                                     height);

    return screenShot;
}

int Screenshot::GetNumberDesktop()
{
    return QApplication::desktop()->numScreens();
}

QString Screenshot::GetSizeDesktop(int numberDesktop)
{
    QString sizeDesktop;
    QString sWidth, sHeight, sLeft, sTop;
    sWidth.setNum(QApplication::desktop()->screenGeometry(numberDesktop).width());
    sHeight.setNum(QApplication::desktop()->screenGeometry(numberDesktop).height());
    sLeft.setNum(QApplication::desktop()->screenGeometry(numberDesktop).left());
    sTop.setNum(QApplication::desktop()->screenGeometry(numberDesktop).top());

    sizeDesktop = sWidth + "x" + sHeight + " (" + sLeft + "," + sTop + ")";

    return sizeDesktop;
}

bool Screenshot::SaveScreenShotToPdf(QPixmap screenShot,
                                     QString fileName,
                                     QPrinter::Orientation orientation,
                                     QPrinter::PageSize pageSize,
                                     QPrinter::ColorMode colorMode)
{
    QByteArray encodedString;
    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");;
    QString appendText, titleMessage;

    Qt::WindowFlags flag = 0;
    flag |=  Qt::Tool;// Удаление значка с taskbar и кнопок максимум, минимум размера окна
    flag |=  Qt::WindowStaysOnTopHint;// Всегда поверх других
    flag |=  Qt::FramelessWindowHint;
    flag |=  Qt::WindowTitleHint;

    QDateTime date_time = QDateTime::currentDateTime();

    QMessageBox msgBox;
    msgBox.setWindowTitle(date_time.toString() + " " + QApplication::applicationName() + ":" + QApplication::applicationVersion());
    msgBox.setWindowFlags(flag);

    // Step 1. Убедится, что screenshot не пустой
    if(screenShot.isNull()){
        encodedString = "Сообщение";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = "В буфере обмена ничего нет";
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Information); // Тип иконки сообщения
        msgBox.setInformativeText(appendText); // Основное сообщение Message Box
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        msgBox.exec();
        return 0;
    }//if

    // Step 2. Убедится, что fileName не пустой
    if (fileName.isEmpty()){
        encodedString = "Сообщение";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = "Имя файла не указано";
        appendText = codec->toUnicode(encodedString);
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Information); // Тип иконки сообщения
        msgBox.setInformativeText(appendText); // Основное сообщение Message Box
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        msgBox.exec();
        return 0;
    }//if

    // Расширение файла
    fileName += ".pdf";

    // Step 3. Проверка существования файла. Если файл существует, то решить, нужно ли его перезаписывать
    if (QFile::exists(fileName)){
        encodedString = "Процедура сохранения";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = " \nуже существует. Заменить?";
        appendText = codec->toUnicode(encodedString);
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Information); // Тип иконки сообщения
        msgBox.setInformativeText(fileName + appendText); // Основное сообщение Message Box
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel); // Добавление реагирования на софт клавиши
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию

        int ret = msgBox.exec(); // Запускаем QMessageBox. После выполнения, в ret будет лежать значение кнопки, на которую нажали - это необходимо для дальнейшей обработки событий

        if(ret == QMessageBox::Cancel)
            return 0;
    }//if

    // Step 4. Объявление экземпляра класса QPrinter и определение его настроек
    QPrinter printer;
    printer.setOutputFileName(fileName);
    printer.setFullPage(true);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(pageSize);
    printer.setOrientation(orientation);
    printer.setColorMode(colorMode);
    printer.newPage();

    // Step 5. Проверка на возможные ошибки
    QPainter painter;
    if (! painter.begin(&printer)){
        encodedString = "Ошибка открытия файла";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = "Убедитесь, что запись в файл разрешена";
        appendText = codec->toUnicode(encodedString);
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Warning); // Тип иконки сообщения
        msgBox.setInformativeText(appendText); // Основное сообщение Message Box
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        msgBox.show();
        return 0;
    }//if

/*
    if (! printer.newPage()){
        encodedString = "Ошибка при добавлении новой страницы";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = "Убедитесь, что на диске есть место";
        appendText = codec->toUnicode(encodedString);
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Warning); // Тип иконки сообщения
        msgBox.setInformativeText(appendText); // Основное сообщение Message Box
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        msgBox.show();
        return 0;
    }//if
*/
    // Step 6. Сохранение
    painter.drawPixmap(0, 0, printer.width(), printer.height(), screenShot);
    painter.end();
/*
    QFile file("c://test.txt");
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&file);
    out << temp;
    out << "\n";
    file.close();
*/
    return 1;
}

bool Screenshot::SaveScreenShotToImage(QPixmap screenShot,
                                       QString fileName,
                                       QString format,
                                       int quality)
{
    QByteArray encodedString;
    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");;
    QString appendText, titleMessage;

    Qt::WindowFlags flag = 0;
    flag |=  Qt::Tool;// Удаление значка с taskbar и кнопок максимум, минимум размера окна
    flag |=  Qt::WindowStaysOnTopHint;// Всегда поверх других
    flag |=  Qt::FramelessWindowHint;
    flag |=  Qt::WindowTitleHint;

    QDateTime date_time = QDateTime::currentDateTime();

    QMessageBox msgBox;
    msgBox.setWindowTitle(date_time.toString() + " " + QApplication::applicationName() + ":" + QApplication::applicationVersion());
    msgBox.setWindowFlags(flag);

    // Step 1. Убедится, что screenshot не пустой
    if(screenShot.isNull()){
        encodedString = "Сообщение";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = "В буфере обмена ничего нет";
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Information); // Тип иконки сообщения
        msgBox.setInformativeText(appendText); // Основное сообщение Message Box
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        msgBox.exec();
        return 0;
    }//if

    // Step 2. Убедится, что fileName не пустой
    if (fileName.isEmpty()){
        encodedString = "Сообщение";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = "Имя файла не указано";
        appendText = codec->toUnicode(encodedString);
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Information); // Тип иконки сообщения
        msgBox.setInformativeText(appendText); // Основное сообщение Message Box
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        msgBox.exec();
        return 0;
    }//if

    // Расширение файла
    fileName += "." + format;

    // Step 3. Проверка существования файла. Если файл существует, то решить, нужно ли его перезаписывать
    if (QFile::exists(fileName)){
        encodedString = "Процедура сохранения";
        titleMessage = codec->toUnicode(encodedString);
        encodedString = " уже существует. Заменить?";
        appendText = codec->toUnicode(encodedString);
        msgBox.setText(titleMessage); // Заголовок сообщения
        msgBox.setIcon(QMessageBox::Information); // Тип иконки сообщения
        msgBox.setInformativeText(fileName + appendText); // Основное сообщение Message Box
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel); // Добавление реагирования на софт клавиши
        msgBox.setDefaultButton(QMessageBox::Save); // На какой кнопке фокусироваться по умолчанию
        int ret = msgBox.exec(); // Запускаем QMessageBox. После выполнения, в ret будет лежать значение кнопки, на которую нажали - это необходимо для дальнейшей обработки событий

        if(ret == QMessageBox::Cancel)
            return 0;
    }//if

    // Step 6. Сохранение
    screenShot.save(fileName, format.toLocal8Bit().data(), quality);


    return 1;
}
