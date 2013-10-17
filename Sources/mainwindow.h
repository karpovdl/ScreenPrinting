#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    //
    void showEvent(QShowEvent *event);
    // Событие изменения размера формы
    void resizeEvent(QResizeEvent *event);
    // Нажатие клавиши
    void keyPressEvent(QKeyEvent *event);
    // Отпускание клавиши
    void keyReleaseEvent(QKeyEvent *event);
    // Перехват любого события
    bool event(QEvent *event);
    // Событие закрытие формы
    void closeEvent(QCloseEvent *event);

public slots:
    void slotActivateWindow();

private slots:
    // Обновление снимка экрана
    void slotUpdateScreenShot();
    // Сохранение снимка экрана в "слепом режиме"
    void slotBlindSaveScreenShot();
    // Контроль буфера
    void slotDataBufferControl();
    //
    void slotShowHide();
    //
    void slotShowMessage();
    // Команда Далее
    void on_ButtonNext_clicked();
    // Команда Отмена
    void on_ButtonCancel_clicked();
    // Команда Обновить
    void on_ButtonUpdate_clicked();
    // Задание размера снимка экрана
    void on_rbNumberMonitor_clicked();
    // Изменение размера формы
    void on_rbSizeRect_clicked();
    // Выбор дирректории
    void on_ButtonSetDirectory_clicked();
    // Ориентация страницы
    void on_cbOrientation_currentIndexChanged(const QString &arg1);
    // Цветовой режим страницы
    void on_cbColorMode_currentIndexChanged(const QString &arg1);
    // Размер страницы
    void on_cbPageSize_currentIndexChanged(const QString &arg1);
    // Реагирование выбор расширения
    void on_cbFormatFile_currentIndexChanged(const QString &arg1);
    //
    void on_cbWindowStaysOnTopHint_clicked();
    //
    void on_cbWindowCenterDesktop_clicked();
    //
    void on_cbWindowOpacity_clicked();
    //
    void on_hsWindowOpacity_valueChanged(int );

private:
    Ui::MainWindow *ui;
//FUNCTION
    // Инициализация переменных
    void InicializationVariable();
    // Настройка меню области оповещения
    void InicializationTrayMenu();
    // Чтение настроек приложения
    void GetSettings();
    // Запись настроек приложения
    void SetSettings();
    // Сохранение снимка экрана
    bool SaveScreenShot();
//SETTINGS FUNCTION
    // Видимость виджитов размера снимка экрана
    void SetEnabledWidgetSizeScreen();
    // Определение поведения иконки в области оповещения
    void ChangeIcon();
    // Перемещение формы в центр окна
    void WindowsCenterDesktop();
    // Прозрачность формы
    void WindowOpacity();
    // Расположение поверх всех окон
    void WindowStaysOnTop();
//VARIABLE
    QVector<QByteArray> encodedString;
    QTextCodec *codec;
    QVector<QString> appendText;
    // Полное имя конечного файла
    QString full_file_name;
    // Снимок экрана
    QPixmap screenShotDispaly;
    // Ориентация страницы
    QPrinter::Orientation page_orientation;
    // Цветовой режим страницы
    QPrinter::ColorMode page_color;
    // Размер страницы
    QPrinter::PageSize page_size;
    // Номер клавиши
    int key;
    // Флаг анимации: плавного затухания/появления формы
    bool isAnimation;
    // Объект, который предоставляет возможность использовать область оповещения
    QSystemTrayIcon *m_ptrayIcon;
    // Виджет для реализации контекстного меню
    QMenu *m_ptrayiconMenu;
    // Булевая переменная для управления сменой растровыхх изображений
    bool m_bIconSwitcher;
};

#endif // MAINWINDOW_H
