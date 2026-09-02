
#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QDateTime>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QLabel>
#include <QCloseEvent>
#include <QMessageBox>
#include <QCoreApplication>



class LogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = nullptr);

public slots:
    void addLog(const QString& level, const QString& message);
    void onSearchClicked();
    void onExportClicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QTableWidget* realTimeTable;
    QTableWidget* searchResultTable;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;
    QLineEdit* searchContentEdit;
    QPushButton* searchButton;
    QPushButton* exportButton;
    const int MAX_ROWS = 100;

    void setupUI();
    void addTableRow(QTableWidget* table, const QString& time, const QString& level, const QString& message);
    void parseLogLine(const QString& line, QTableWidget* table);
    bool validateDate(QDateEdit* edit);
};
