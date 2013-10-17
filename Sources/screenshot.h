#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QtGui>
#include <QMessageBox>
#include <QPixmap>

class Screenshot
{
public:
    // Получить снимок экрана заданного монитора
    static QPixmap GetScreenShot(int numberDesktop);

    static QPixmap GetScreenShot(int x0Position,
                                 int y0Position,
                                 int width,
                                 int height);

    // Число мониторов
    static int GetNumberDesktop();

    // Размер экрана
    static QString GetSizeDesktop(int numberDesktop);

    // Сохранение снимка экрана в формате *.pdf
    // screenshot - экземпляр класса, хранящий снимок экрана
    // path - путь сохранения
    // orientation - ориентация: вертикальная или горизонтальная
    // pageSize - размер листа: А0..А9
    // colorMode - цветовой режим: цветной или черно-белый
    static bool SaveScreenShotToPdf(QPixmap screenShot,
                                    QString path,
                                    QPrinter::Orientation orientation,
                                    QPrinter::PageSize pageSize,
                                    QPrinter::ColorMode colorMode);

    // Сохранение снимка экрана в формате картинки
    // screenshot - экземпляр класса, хранящий снимок экрана
    // path - путь сохранения
    // format - формат картинки: PNG, JPG, JPEG, BMP
    // quality - качество
    static bool SaveScreenShotToImage(QPixmap screenShot,
                                      QString paths,
                                      QString format,
                                      int quality);
};

#endif // SCREENSHOT_H
