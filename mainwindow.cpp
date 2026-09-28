#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setStyleSheet(R"(
    QMainWindow {
        background-color: #f2f2f2;
    }

    QLineEdit {
        background-color: white;
        border: 1px solid #cccccc;
        border-radius: 8px;
        padding: 10px;
        font-size: 28px;
        color: black;
    }

    QPushButton {
        background-color: white;
        border: 1px solid #cccccc;
        border-radius: 8px;
        font-size: 20px;
        padding: 10px;
        color: black;
    }

    QPushButton:hover {
        background-color: #eeeeee;
    }

    QPushButton:pressed {
        background-color: #dddddd;
    }
)");

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    ui->displayEdit->setFocusPolicy(Qt::NoFocus);

    connect(ui->btn0, &QPushButton::clicked, this, [this]() {
        appendNumber("0");
    });

    connect(ui->btn1, &QPushButton::clicked, this, [this]() {
        appendNumber("1");
    });

    connect(ui->btn2, &QPushButton::clicked, this, [this]() {
        appendNumber("2");
    });

    connect(ui->btn3, &QPushButton::clicked, this, [this]() {
        appendNumber("3");
    });

    connect(ui->btn4, &QPushButton::clicked, this, [this]() {
        appendNumber("4");
    });

    connect(ui->btn5, &QPushButton::clicked, this, [this]() {
        appendNumber("5");
    });

    connect(ui->btn6, &QPushButton::clicked, this, [this]() {
        appendNumber("6");
    });

    connect(ui->btn7, &QPushButton::clicked, this, [this]() {
        appendNumber("7");
    });

    connect(ui->btn8, &QPushButton::clicked, this, [this]() {
        appendNumber("8");
    });

    connect(ui->btnPlus, &QPushButton::clicked, this, [this]() {
        inputOperator("+");
    });

    connect(ui->btnMinus, &QPushButton::clicked, this, [this]() {
        inputOperator("-");
    });

    connect(ui->btnMultiply, &QPushButton::clicked, this, [this]() {
        inputOperator("*");
    });

    connect(ui->btnDivide, &QPushButton::clicked, this, [this]() {
        inputOperator("/");
    });

    connect(ui->btn9, &QPushButton::clicked, this, [this]() {
        appendNumber("9");
    });

    connect(ui->btnEqual, &QPushButton::clicked, this, [this]() {
        calculateResult();
    });

    connect(ui->btnDot, &QPushButton::clicked, this, [this]() {

        // 上一次已经计算完成，重新开始
        if (justCalculated) {
            ui->displayEdit->clear();

            currentInput.clear();
            currentOperator.clear();
            firstNumber = 0.0;

            justCalculated = false;
            waitingForSecondOperand = false;
            hasSecondOperand = false;
        }

        // 当前正在输入第二个或后面的数字
        // 例如：12.5+3
        if (waitingForSecondOperand && currentInput.isEmpty()) {
            currentInput = "0.";
            ui->displayEdit->setText(
                ui->displayEdit->text() + "0."
                );

            waitingForSecondOperand = false;
            hasSecondOperand = true;
            return;
        }

        // 当前数字已经有小数点
        if (currentInput.contains('.')) {
            return;
        }

        // 当前没有数字
        if (currentInput.isEmpty()) {
            currentInput = "0";
            ui->displayEdit->setText(
                ui->displayEdit->text() + "0"
                );
        }

        currentInput += ".";

        ui->displayEdit->setText(
            ui->displayEdit->text() + "."
            );
    });

    connect(ui->btnClear, &QPushButton::clicked, this, [this]() {
        ui->displayEdit->clear();

        firstNumber = 0.0;
        currentOperator.clear();
        waitingForSecondOperand = false;
        hasSecondOperand = false;
        justCalculated = false;
    });

    connect(ui->btnBackspace, &QPushButton::clicked, this, [this]() {

        QString expression = ui->displayEdit->text();

        if (expression.isEmpty()) {
            return;
        }

        // Error 状态直接清空
        if (expression == "Error") {
            ui->displayEdit->clear();

            currentInput.clear();
            currentOperator.clear();
            firstNumber = 0.0;

            waitingForSecondOperand = false;
            hasSecondOperand = false;
            justCalculated = false;

            return;
        }

        // 删除最后一个字符
        expression.chop(1);
        ui->displayEdit->setText(expression);

        // 重新确定当前正在输入的数字
        int lastOperatorIndex = -1;

        for (int i = expression.length() - 1; i >= 0; --i) {
            QChar ch = expression[i];

            if (ch == '+' ||
                ch == '-' ||
                ch == '*' ||
                ch == '/') {

                lastOperatorIndex = i;
                break;
            }
        }

        if (lastOperatorIndex == -1) {

            currentInput = expression;
            currentOperator.clear();

            waitingForSecondOperand = false;
            hasSecondOperand = false;
        }
        else {

            currentOperator = expression.mid(
                lastOperatorIndex, 1
                );

            currentInput = expression.mid(
                lastOperatorIndex + 1
                );

            if (currentInput.isEmpty()) {
                waitingForSecondOperand = true;
                hasSecondOperand = false;
            }
            else {
                waitingForSecondOperand = true;
                hasSecondOperand = true;
            }
        }

        // 如果刚刚是计算结果，现在继续编辑它
        if (justCalculated) {
            justCalculated = false;
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendNumber(const QString &number)
{
    if (justCalculated) {
        ui->displayEdit->clear();
        currentInput.clear();
        currentOperator.clear();
        firstNumber = 0.0;

        justCalculated = false;
        waitingForSecondOperand = false;
    }

    currentInput += number;

    ui->displayEdit->setText(
        ui->displayEdit->text() + number
        );

    if (!currentOperator.isEmpty()) {
        hasSecondOperand = true;
    }
}

void MainWindow::inputOperator(const QString &op)
{
    QString text = ui->displayEdit->text();

    if (text.isEmpty() || text == "Error") {
        return;
    }

    // 还没有输入第二个数字
    // 例如：5+
    if (waitingForSecondOperand && !hasSecondOperand) {

        currentOperator = op;

        // 替换最后一个操作符
        if (!text.isEmpty()) {
            text.chop(1);
            text += op;
            ui->displayEdit->setText(text);
        }

        return;
    }

    // 已经输入了第二个数字
    // 例如：5+5
    // 此时再按操作符，应该把新的操作符加到后面
    if (hasSecondOperand) {

        firstNumber = currentInput.toDouble();

        currentOperator = op;

        ui->displayEdit->setText(
            ui->displayEdit->text() + op
            );

        currentInput.clear();

        waitingForSecondOperand = true;
        hasSecondOperand = false;

        return;
    }

    // 普通情况下输入第一个操作符
    firstNumber = currentInput.toDouble();
    currentOperator = op;

    ui->displayEdit->setText(
        ui->displayEdit->text() + op
        );

    currentInput.clear();

    waitingForSecondOperand = true;
    hasSecondOperand = false;
    justCalculated = false;
}

void MainWindow::calculateResult()
{
    QString expression = ui->displayEdit->text();

    if (expression.isEmpty() || expression == "Error") {
        return;
    }

    QChar lastChar = expression.at(expression.length() - 1);

    if (lastChar == '+' ||
        lastChar == '-' ||
        lastChar == '*' ||
        lastChar == '/') {
        return;
    }

    double result = 0.0;

    // 计算表达式
    bool success = evaluateExpression(expression, result);

    // 计算失败，例如除以 0
    if (!success) {
        ui->displayEdit->setText("Error");

        firstNumber = 0.0;
        currentInput.clear();
        currentOperator.clear();

        waitingForSecondOperand = false;
        hasSecondOperand = false;
        justCalculated = true;

        return;
    }

    // 正常计算
    ui->displayEdit->setText(QString::number(result));

    firstNumber = result;
    currentInput = QString::number(result);

    currentOperator.clear();

    waitingForSecondOperand = false;
    hasSecondOperand = false;
    justCalculated = true;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_0:
        appendNumber("0");
        break;

    case Qt::Key_1:
        appendNumber("1");
        break;

    case Qt::Key_2:
        appendNumber("2");
        break;

    case Qt::Key_3:
        appendNumber("3");
        break;

    case Qt::Key_4:
        appendNumber("4");
        break;

    case Qt::Key_5:
        appendNumber("5");
        break;

    case Qt::Key_6:
        appendNumber("6");
        break;

    case Qt::Key_7:
        appendNumber("7");
        break;

    case Qt::Key_8:
        appendNumber("8");
        break;

    case Qt::Key_9:
        appendNumber("9");
        break;

    case Qt::Key_Plus:
        inputOperator("+");
        break;

    case Qt::Key_Minus:
        inputOperator("-");
        break;

    case Qt::Key_Asterisk:
        inputOperator("*");
        break;

    case Qt::Key_Slash:
        inputOperator("/");
        break;

    case Qt::Key_Period:
        ui->btnDot->click();
        break;

    case Qt::Key_Backspace:
        ui->btnBackspace->click();
        break;

    case Qt::Key_Delete:
        ui->btnClear->click();
        break;

    case Qt::Key_Enter:
    case Qt::Key_Return:
        calculateResult();
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

bool MainWindow::evaluateExpression(const QString &expression, double &result)
{
    QString exp = expression;

    QStringList numbers;
    QStringList operators;

    QString currentNumber;

    for (int i = 0; i < exp.length(); ++i) {
        QChar ch = exp[i];

        if (ch.isDigit() || ch == '.') {
            currentNumber += ch;
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            numbers.append(currentNumber);
            currentNumber.clear();
            operators.append(QString(ch));
        }
    }

    if (!currentNumber.isEmpty()) {
        numbers.append(currentNumber);
    }

    if (numbers.isEmpty()) {
        return false;
    }

    // 先计算乘法和除法
    for (int i = 0; i < operators.size(); ) {

        if (operators[i] == "*" || operators[i] == "/") {

            double left = numbers[i].toDouble();
            double right = numbers[i + 1].toDouble();

            // 除数为 0
            if (operators[i] == "/" && right == 0) {
                return false;
            }

            double temp;

            if (operators[i] == "*") {
                temp = left * right;
            }
            else {
                temp = left / right;
            }

            numbers[i] = QString::number(temp);
            numbers.removeAt(i + 1);
            operators.removeAt(i);

        }
        else {
            ++i;
        }
    }

    // 再计算加法和减法
    result = numbers[0].toDouble();

    for (int i = 0; i < operators.size(); ++i) {

        double number = numbers[i + 1].toDouble();

        if (operators[i] == "+") {
            result += number;
        }
        else if (operators[i] == "-") {
            result -= number;
        }
    }

    return true;
}