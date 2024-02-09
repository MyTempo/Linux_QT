#ifndef EQUIPAMENTOCONFIG_H
#define EQUIPAMENTOCONFIG_H

#include <QDialog>
#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QCoreApplication>
#include <QtSql>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QEventLoop>
#include <QDebug>

namespace Ui {
class EquipamentoConfig;
}

class EquipamentoConfig : public QDialog
{
    Q_OBJECT

public:
    explicit EquipamentoConfig(QWidget *parent = nullptr);
    ~EquipamentoConfig();

private slots:
    void on_pushButton_clicked();

private:
    Ui::EquipamentoConfig *ui;
    QString SysDir;
    QString SysConfigDir;
};

#endif // EQUIPAMENTOCONFIG_H
