#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void appendNumber(const QString &number);
    void inputOperator(const QString &op);
    void calculateResult();
    double evaluateExpression(const QString &expression);
    void keyPressEvent(QKeyEvent *event);

    Ui::MainWindow *ui;

    double firstNumber = 0.0;
    QString currentOperator;
    QString currentInput;

    bool waitingForSecondOperand = false;
    bool hasSecondOperand = false;
    bool justCalculated = false;
};
#endif // MAINWINDOW_H
