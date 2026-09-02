
#include "logwindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextStream>

LogWindow::LogWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("日志系统");
    setMinimumSize(1600, 1200);
    setupUI();
    setStyleSheet(R"(
        /* 原有样式保持不变 */
        LogWindow { background-color: #f5f6fa; }
        QTableWidget { background: white; border: 2px solid #dcdde1; gridline-color: #ecf0f1; }
        QHeaderView::section { background-color: #487eb0; color: white; }
        QTableWidget::item { border-bottom: 1px solid #ecf0f1; }
        QTableWidget::item:selected { background-color: #487eb0; color: white; }
        QScrollBar:vertical { background: #ecf0f1; }
        QScrollBar::handle:vertical { background: #bdc3c7; border-radius: 6px; }
        QTableWidget::item[logLevel="Info"] { color: #3498db; }
        QTableWidget::item[logLevel="Warning"] { color: #e74c3c; }
        QPushButton { background-color: #487eb0; color: white; border: none; border-radius: 8px; }
        QPushButton:hover { background-color: #40739e; }
        QPushButton:pressed { background-color: #2c3e50; }
        QLabel { color: #2c3e50; }
        QDateEdit, QLineEdit { border: 1px solid #dcdde1; border-radius: 4px; padding: 4px; }
    )");
}

void LogWindow::setupUI()
{
    // 实时日志表格
    realTimeTable = new QTableWidget(this);
    realTimeTable->setColumnCount(3);
    realTimeTable->setHorizontalHeaderLabels({"时间", "等级", "内容"});
    realTimeTable->horizontalHeader()->setStretchLastSection(true);
    realTimeTable->verticalHeader()->setVisible(false);
    realTimeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 搜索控件
    startDateEdit = new QDateEdit(QDate::currentDate(), this);
    endDateEdit = new QDateEdit(QDate::currentDate(), this);
    searchContentEdit = new QLineEdit(this);
    searchContentEdit->setPlaceholderText("输入搜索内容");
    searchButton = new QPushButton("搜索", this);
    exportButton = new QPushButton("导出", this);
    // 设置按钮大小
        searchButton->setFixedSize(120, 40);  // 宽度120像素，高度40像素
        exportButton->setFixedSize(120, 40);

        // 设置按钮字体
        QFont buttonFont = searchButton->font();
        buttonFont.setPointSize(12);  // 字体大小12pt
        searchButton->setFont(buttonFont);
        exportButton->setFont(buttonFont);

    // 搜索结果表格
    searchResultTable = new QTableWidget(this);
    searchResultTable->setColumnCount(3);
    searchResultTable->setHorizontalHeaderLabels({"时间", "等级", "内容"});
    searchResultTable->horizontalHeader()->setStretchLastSection(true);
    searchResultTable->verticalHeader()->setVisible(false);
    searchResultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 布局设置
    QWidget* searchWidget = new QWidget(this);
    QGridLayout* searchLayout = new QGridLayout(searchWidget);
    searchLayout->addWidget(new QLabel("起始时间:"), 0, 0);
    searchLayout->addWidget(startDateEdit, 0, 1);
    searchLayout->addWidget(new QLabel("结束时间:"), 1, 0);
    searchLayout->addWidget(endDateEdit, 1, 1);
    searchLayout->addWidget(new QLabel("搜索内容:"), 2, 0);
    searchLayout->addWidget(searchContentEdit, 2, 1);
    searchLayout->addWidget(searchButton, 0, 2, 2, 1);
    searchLayout->addWidget(exportButton, 2, 2);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(realTimeTable);
    mainLayout->addWidget(searchWidget);
    mainLayout->addWidget(searchResultTable);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    setLayout(mainLayout);

    // 信号连接
    connect(searchButton, &QPushButton::clicked, this, &LogWindow::onSearchClicked);
    connect(exportButton, &QPushButton::clicked, this, &LogWindow::onExportClicked);
}

void LogWindow::addLog(const QString& level, const QString& message)
{

    const QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    bool atBottom = realTimeTable->verticalScrollBar()->value() == realTimeTable->verticalScrollBar()->maximum();

    if(realTimeTable->rowCount() >= MAX_ROWS) {
        realTimeTable->removeRow(0);
    }

    const int row = realTimeTable->rowCount();
    realTimeTable->insertRow(row);

    QTableWidgetItem* timeItem = new QTableWidgetItem(time);
    QTableWidgetItem* levelItem = new QTableWidgetItem(level);
    levelItem->setForeground(level == "Warning" ? QColor(255, 165, 0) : QColor(0, 0, 255));

    QTableWidgetItem* msgItem = new QTableWidgetItem(message);

    realTimeTable->setItem(row, 0, timeItem);
    realTimeTable->setItem(row, 1, levelItem);
    realTimeTable->setItem(row, 2, msgItem);

    if(atBottom) {
        realTimeTable->scrollToBottom();
    }
}

void LogWindow::onSearchClicked()
{
    if (searchContentEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入搜索内容");
        return;
    }
    if (!validateDate(startDateEdit) || !validateDate(endDateEdit)) {
        QMessageBox::warning(this, "错误", "日期格式不正确！");
        return;
    }


    searchResultTable->setRowCount(0);

    QString currentDir2 = QCoreApplication::applicationDirPath();
    QString filePath = currentDir2 + "/savepath.txt";
    QFile file(filePath);

    QString dataPath, imagePath;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line;
            } else if (line.startsWith("imagepath=")) {
                imagePath = line;
            }
        }
        file.close();
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy.MM.dd.HH.mm.ss");

    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);
    QDir dataDir(datapath2);
    QString dataFolderPath = dataDir.absoluteFilePath("log");
    QDir logDir(dataFolderPath);

    if (!logDir.exists()) {
        QMessageBox::warning(this, "错误", "日志目录不存在！");
        return;
    }

    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    QString searchContent = searchContentEdit->text().trimmed();

    QFileInfoList fileList = logDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    foreach (QFileInfo fileInfo, fileList) {
        QDateTime creationTime = fileInfo.created();
        QDate creationDate = creationTime.date();

        bool timeMatch = true;
        if (startDate.isValid() && creationDate < startDate) timeMatch = false;
        if (endDate.isValid() && creationDate > endDate) timeMatch = false;
        if (!timeMatch) continue;

        // 读取文件内容
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();

            // 解析日志行，提取各个部分
            QRegularExpression re("^(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}) \\[(Info|Warning)\\] - (.*)$");
            QRegularExpressionMatch match = re.match(line);

            if (match.hasMatch()) {
                QString timePart = match.captured(1);      // 时间部分
                QString levelPart = match.captured(2);     // 等级部分
                QString messagePart = match.captured(3);   // 内容部分

                bool levelMatch = levelPart.contains(searchContent, Qt::CaseInsensitive);
                bool messageMatch = messagePart.contains(searchContent, Qt::CaseInsensitive);

                if (searchContent.isEmpty() || levelMatch || messageMatch) {
                    parseLogLine(line, searchResultTable);
                }
            }
        }
        file.close();
    }

    // 设置搜索结果样式
    for (int i = 0; i < searchResultTable->rowCount(); ++i) {
        QTableWidgetItem* levelItem = searchResultTable->item(i, 1);
        if (levelItem) {
            levelItem->setForeground(levelItem->text() == "Warning" ? QColor(255, 165, 0) : QColor(0, 0, 255));
        }
    }
}
void LogWindow::parseLogLine(const QString& line, QTableWidget* table)
{
    QRegularExpression re("^(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}) \\[(Info|Warning)\] - (.*)$");
    QRegularExpressionMatch match = re.match(line);

    if (match.hasMatch()) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(match.captured(1)));
        table->setItem(row, 1, new QTableWidgetItem(match.captured(2)));
        table->setItem(row, 2, new QTableWidgetItem(match.captured(3)));
    }
}

void LogWindow::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出日志文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件！");
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < searchResultTable->rowCount(); ++i) {
        QString time = searchResultTable->item(i, 0)->text();
        QString level = searchResultTable->item(i, 1)->text();
        QString message = searchResultTable->item(i, 2)->text();
        out << QString("%1 [%2] - %3\n").arg(time).arg(level).arg(message);
    }
    file.close();
    QMessageBox::information(this, "成功", "日志文件已导出！");
}

bool LogWindow::validateDate(QDateEdit* edit)
{
    return edit->date().isValid();
}

void LogWindow::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}
