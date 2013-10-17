#ifndef SMALLSCREENWINDOW_H
#define SMALLSCREENWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
    class SmallScreenWindow;
}

class SmallScreenWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SmallScreenWindow(QWidget *parent = 0);
    SmallScreenWindow(bool isClose);
    ~SmallScreenWindow();

protected:
    // Событие закрытие формы
    void closeEvent(QCloseEvent *event);

signals:

private slots:
    // Анимация скрытия окна
    void AnimationHideWindow();
    // Закрыть окно
    void on_ButtonClose_clicked();

private:
    Ui::SmallScreenWindow *ui;
//FUNCTION
    // Инициализация переменных
    void InicializationVariable();
    // Инициализация формы
    void InicializationWindow();
    // Анимация появления окна
    void AnimationShowWindow();
    // Новый снимок экрана
    void UpdateScreenShot();
    // Сохранить снимок экрана
    void SaveScreenShot();
//SETTINGS FUNCTION

//VARIABLE
    // Снимок экрана
    QPixmap screenShotDispaly;
    //
    bool isClose;
};

#endif // SMALLSCREENWINDOW_H
