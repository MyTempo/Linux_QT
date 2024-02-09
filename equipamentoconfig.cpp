#include "equipamentoconfig.h"
#include "ui_equipamentoconfig.h"
#include <QMessageBox>
EquipamentoConfig::EquipamentoConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EquipamentoConfig)
{
    ui->setupUi(this);
    this->SysConfigDir = QDir::homePath();
    QString filePath = this->SysConfigDir + "/MyTempo/Reader_data/equip_data.json";
    QString serverFile = this->SysConfigDir + "/MyTempo/Reader_data/server.json";
    // Verificar se o arquivo existe
    QFile file(filePath);
    qDebug() << filePath;
    if (file.exists()) {
        // Abrir o arquivo para leitura
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // Ler o conteúdo do arquivo
            QByteArray jsonData = file.readAll();

            // Criar um documento JSON a partir dos dados lidos
            QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

            // Verificar se o documento JSON é um objeto
            if (jsonDocument.isObject()) {
                // Extrair o objeto JSON
                QJsonObject jsonObject = jsonDocument.object();

                // Extrair os valores do objeto JSON
                QString nomeEquipamentoResposta = jsonObject.value("modelo").toString();
                QString idEquipamento = jsonObject.value("equipamento").toString();
                QString idCheckpoint = jsonObject.value("idcheck").toString();
                QString idProva =  jsonObject.value("idprova").toString();
                QString EquipamentoStatus = jsonObject.value("status").toString();
                QString local_check = jsonObject.value("identificacao").toString();
                QString prova = jsonObject.value("tituloprova").toString();
                QString hora = jsonObject.value("hora").toString();
                QString serie = jsonObject.value("serie").toString();
                QString fabricante = jsonObject.value("fabricante").toString();
                QString descricao_check = jsonObject.value("descricao_check").toString();
                QString last_update = jsonObject.value("last_update").toString();

                if(EquipamentoStatus == "1") {
                    EquipamentoStatus = "<b style='color:green;'>ATIVADO</b>";
                }
                else {
                    EquipamentoStatus = "<b style='color:red;'>DESATIVADO</b>";
                }

                if(local_check == "1") {
                    local_check = "CHEGADA";
                }
                else {
                    local_check = "LARGADA";
                }

                QFile file(serverFile);
                if (file.exists()) {
                    // Abrir o arquivo para leitura
                    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        // Ler o conteúdo do arquivo
                        QByteArray jsonData = file.readAll();

                        // Criar um documento JSON a partir dos dados lidos
                        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

                        // Verificar se o documento JSON é um objeto
                        if (jsonDocument.isObject()) {
                            // Extrair o objeto JSON
                            QJsonObject jsonObject = jsonDocument.object();

                            // Extrair os valores do objeto JSON
                            QString serverIp = jsonObject.value("server_ip").toString();
                            int porta = jsonObject.value("port").toInt();
                            // Atualizar o UI com os valores do primeiro elemento
                            ui->label->setText("<b style='text-align:center;'>" + nomeEquipamentoResposta + "</b>");
                            ui->lineEdit->setText(nomeEquipamentoResposta);
                            ui->id_checkpoint->setText("ID CHECKPOINT: " + idCheckpoint);
                            ui->id_equipamento->setText("ID EQUIPAMENTO: " + idEquipamento);
                            ui->id_prova->setText("ID PROVA: " + idProva);
                            ui->local->setText("LOCAL: " + local_check);
                            ui->prova->setText("PROVA: <b>" + prova +"</b>");
                            ui->descricao->setText("DESCRIÇÃO: " + descricao_check);
                            ui->status->setText("STATUS: " + EquipamentoStatus);
                            ui->hora->setText("HORA: " + hora);
                            ui->fabricante->setText("FABRICANTE: " + fabricante);
                            ui->serie->setText("SÉRIE: " + serie);
                            ui->label_2->setText("IP: " + serverIp);
                            // Atualizar a data e hora
                            QDateTime currentDateTime = QDateTime::currentDateTime();
                            QString formattedDateTime = currentDateTime.toString("dd/MM/yyyy - hh:mm:ss");
                            ui->label_5->setText(last_update);
                        } else {
                            qDebug() << "O documento JSON não é um objeto.";
                        }
                        file.close();
                    } else {
                        qDebug() << "Erro ao abrir o arquivo JSON para leitura.";
                    }
                } else {
                    qDebug() << "O arquivo JSON não existe.";
                }




            }
        }
    }

}

EquipamentoConfig::~EquipamentoConfig()
{
    delete ui;
}

void EquipamentoConfig::on_pushButton_clicked() {
    // Obter o nome do equipamento a partir do lineEdit

    QString nomeEquipamento = ui->lineEdit->text();

    if(nomeEquipamento != "") {

    ui->label_9->setText("Buscando Equipamento...");

    qDebug() << nomeEquipamento;
    QJsonObject json;
    json["nome_equipamento"] = nomeEquipamento;

    // Converter o JSON em QByteArray
    QJsonDocument jsonDoc(json);
    QByteArray jsonData = jsonDoc.toJson();

    // Configurar a solicitação POST
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://api.mytempo.esp.br/api/v1/DadosEquipamento.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Realizar a solicitação POST
    QNetworkReply *reply = manager.post(request, jsonData);

    // Aguardar pela resposta (loop de eventos)
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // Verificar se houve erro na resposta
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "POST bem-sucedido";
        ui->label_9->setText("Quase lá...");

        // Ler a resposta do servidor em JSON
        QByteArray responseData = reply->readAll();
        QJsonDocument responseJson = QJsonDocument::fromJson(responseData);

        // Verificar se a resposta é um array JSON e não está vazia
        if (responseJson.isArray() && !responseJson.array().isEmpty()) {
            // Iterar sobre todos os elementos do array
            for (const QJsonValue& value : responseJson.array()) {
                if (value.isObject()) {
                    QJsonObject object = value.toObject();

                    // Extrair os valores do objeto JSON
                    QString nomeEquipamentoResposta = object.value("modelo").toString();
                    QString idEquipamento = object.value("equipamento").toString();
                    QString idCheckpoint = object.value("idcheck").toString();
                    QString idProva = object.value("idprova").toString();
                    QString EquipamentoStatus = object.value("status").toString();
                    QString local_check = object.value("identificacao").toString();
                    QString prova = object.value("tituloprova").toString();
                    QString hora = object.value("hora").toString();
                    QString serie = object.value("serie").toString();
                    QString fabricante = object.value("fabricante").toString();
                    QString descricao_check = object.value("descricao_check").toString();
                    QString data_prova = object.value("dataprova").toString();
                    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

                    QString serverFile = this->SysConfigDir + "/MyTempo/Reader_data/server.json";
                    QFile fileServer(serverFile);
                    if (fileServer.exists()) {
                        // Abrir o arquivo para leitura
                        if (fileServer.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            // Ler o conteúdo do arquivo
                            QByteArray jsonData = fileServer.readAll();

                            // Criar um documento JSON a partir dos dados lidos
                            QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

                            // Verificar se o documento JSON é um objeto
                            if (jsonDocument.isObject()) {
                                // Extrair o objeto JSON
                                QJsonObject jsonObject = jsonDocument.object();

                                // Extrair os valores do objeto JSON
                                QString serverIp = jsonObject.value("server_ip").toString();
                                QJsonObject secondPostData;
                                QString porta = jsonObject.value("port").toString();

                                secondPostData["ip"] = serverIp + ":" + porta;
                                secondPostData["id"] = idEquipamento;

                                // Converter o JSON em QByteArray
                                QJsonDocument secondPostJsonDoc(secondPostData);
                                QByteArray secondPostJsonData = secondPostJsonDoc.toJson();

                                // Configurar a solicitação POST para a segunda API
                                QNetworkRequest secondPostRequest(QUrl("http://api.mytempo.esp.br/api/v1/ConfiguraIp.php"));
                                secondPostRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                                // Realizar a segunda solicitação POST
                                QNetworkReply *secondPostReply = manager.post(secondPostRequest, secondPostJsonData);

                                // Aguardar pela resposta (loop de eventos)
                                QObject::connect(secondPostReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
                                loop.exec();

                                // Verificar se houve erro na segunda resposta
                                if (secondPostReply->error() == QNetworkReply::NoError) {
                                    qDebug() << "Segundo POST bem-sucedido";
                                    // ... Faça algo com a resposta da segunda API ...
                                } else {
                                    qDebug() << "Erro no segundo POST:" << secondPostReply->errorString();
                                }

                                // Limpar a resposta da segunda solicitação
                                secondPostReply->deleteLater();
                            }
                        }
                    }




                    QJsonObject newJsonObject;

                    newJsonObject["modelo"] = nomeEquipamentoResposta;
                    newJsonObject["equipamento"] = idEquipamento;
                    newJsonObject["idcheck"] = idCheckpoint;
                    newJsonObject["idprova"] = idProva;
                    newJsonObject["status"] = EquipamentoStatus;
                    newJsonObject["identificacao"] = local_check;
                    newJsonObject["tituloprova"] = prova;
                    newJsonObject["hora"] = hora;
                    newJsonObject["data_prova"] = data_prova;
                    newJsonObject["serie"] = serie;
                    newJsonObject["fabricante"] = fabricante;
                    newJsonObject["descricao_check"] = descricao_check;
                    newJsonObject["last_update"] = timestamp;

                    // Criar um documento JSON com o novo objeto
                    QJsonDocument newJsonDocument(newJsonObject);
                    QString directoryPath = QDir::homePath() + "/MyTempo/Reader_data/";

                    QDir directory(directoryPath);
                    if (!directory.exists()) {
                        if (!directory.mkpath(directoryPath)) {
                            qDebug() << "Erro ao criar o diretório:" << directoryPath;
                            return;
                        }
                    }
                    // Abrir o arquivo para escrita, sobrescrevendo o conteúdo existente
                    QString filePath = this->SysConfigDir + "/MyTempo/Reader_data/equip_data.json";
                    qDebug() << filePath;
                    QFile file(filePath);
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                        file.write(newJsonDocument.toJson());
                        file.close();
                        qDebug() << "JSON sobrescrito com sucesso em:" << filePath;
                        ui->label_9->setText("<b style='color:green;'>Pronto!</b>");

                    } else {
                        qDebug() << "Erro ao abrir o arquivo para escrita.";
                    }

                    if(EquipamentoStatus == "1") {
                        EquipamentoStatus = "<b style='color:green;'>ATIVADO</b>";
                    }
                    else {
                        EquipamentoStatus = "<b style='color:red;'>DESATIVADO</b>";
                    }

                    if(local_check == "1") {
                        local_check = "CHEGADA";
                    }
                    else {
                        local_check = "LARGADA";
                    }

//                     Atualizar o UI com os valores do primeiro elemento
                    ui->label->setText("<b style='text-align:center;'>" + nomeEquipamentoResposta + "</b>");
                    ui->lineEdit->setText(nomeEquipamentoResposta);
                    ui->id_checkpoint->setText("ID CHECKPOINT: " + idCheckpoint);
                    ui->id_equipamento->setText("ID EQUIPAMENTO: " + idEquipamento);
                    ui->id_prova->setText("ID PROVA: " + idProva);
                    ui->local->setText("LOCAL: " + local_check);
                    ui->prova->setText("PROVA: <b>" + prova +"</b>");
                    ui->descricao->setText("DESCRIÇÃO: " + descricao_check);
                    ui->status->setText("STATUS: " + EquipamentoStatus);
                    ui->hora->setText("HORA: " + hora);
                    ui->fabricante->setText("FABRICANTE: " + fabricante);
                    ui->serie->setText("SÉRIE: " + serie);

                    // Atualizar a data e hora

                    ui->label_5->setText(timestamp);

                    // Exibir mensagem informando que o equipamento foi encontrado
                    QMessageBox::information(this, "Equipamento Encontrado!", "O Equipamento <b>" + nomeEquipamentoResposta + "</b> foi encontrado!");
                    ui->label_9->setText("");

                }
            }
        } else {
            // Exibir mensagem informando que nenhum equipamento foi encontrado
            ui->label_9->setText("<div style='color:red;'>Equipamento não encontrado <b>✕</b></div>");
            QMessageBox::warning(this, "Equipamento Não Encontrado", "O Equipamento <b>" + ui->lineEdit->text() + "</b> Não foi encontrado ou não foi ativado corretamente na prova <b>Tente novamente</b>.");
            ui->lineEdit->setFocus();
            ui->lineEdit->clear();
            ui->label_9->setText("");
        }

        // Exibir a resposta completa para fins de depuração
        qDebug() << "Resposta do servidor:" << responseData;

    } else {
        // Exibir mensagem informando que ocorreu um erro na solicitação POST
        ui->label_9->setText("<div style='color:red;'>Ocorreu um erro na solicitação <b>✕</b></div>");
        qDebug() << "Erro ao fazer POST. Código de erro:" << reply->error() << ", Descrição do erro:" << reply->errorString();
    }

    // Limpar recursos
    reply->deleteLater();
} else {
    // Exibir mensagem informando que é necessário um nome para buscar o equipamento
    QMessageBox::information(this, "Aviso!", "Você precisa de um nome para buscar o equipamento!");
    ui->lineEdit->setFocus();
}
}
