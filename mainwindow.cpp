#include "mainwindow.h"
#include "ui_mainwindow.h"

int64_t MainWindow::hComm;
QThread* sub= new QThread;
m_thread* work = new m_thread;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    qRegisterMetaType<TagInfo>("TagInfo");//注册

    ui->setupUi(this);
    this->BaseDir = QDir::homePath();
    this->SysConfigDir = "/home/";
    this->SysDir = "/home";

    qDebug() << this->BaseDir;
//    QString filePath = "/tmp/MyTempo/equip_data.json";
    QString filePath =  this->SysConfigDir + "/MyTempo/Reader_data/equip_data.json";
    // Verificar se o arquivo existe
    QFile file(filePath);
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
//                QString idEquipamento = jsonObject.value("equipamento").toString();
//                QString idCheckpoint = jsonObject.value("idcheck").toString();
//               QString idProva =  jsonObject.value("idprova").toString();
//                QString EquipamentoStatus = jsonObject.value("status").toString();
//                QString local_check = jsonObject.value("identificacao").toString();
                QString prova = jsonObject.value("tituloprova").toString();
                QString hora = jsonObject.value("hora").toString();
//                QString serie = jsonObject.value("serie").toString();
//                QString fabricante = jsonObject.value("fabricante").toString();
                QString descricao_check = jsonObject.value("descricao_check").toString();
//                QString data_prova = jsonObject.value("data_prova").toString();

               if(nomeEquipamentoResposta != "" && descricao_check != "" && prova != "") {
                   this->ui->equipamento_dado1->setText("<b>EQUIPAMENTO</b>: " + nomeEquipamentoResposta);
                   this->ui->checkpoint_dado1->setText("<b>CHECKPOINT</b>: " + descricao_check);
                   this->ui->prova_dado1->setText("<b>PROVA</b>: " + prova);
                   this->ui->hora_prova_dado1->setText("<b>HORA DA PROVA</b>: " + hora);
//                   this->ui->data_prova_dado1->setText("<b>DATA DA PROVA</b>: " + data_prova);
               }
            }
        }
    }
    this->model = new QStandardItemModel;   //创建一个标准的条目模型
    this->ui->tableView->setModel(model);   //将tableview设置成model这个标准条目模型的模板, model设置的内容都将显示在tableview上

    this->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->model->setHorizontalHeaderItem(0, new QStandardItem("Índice") );
    this->model->setHorizontalHeaderItem(1, new QStandardItem("Lido") );
    this->model->setHorizontalHeaderItem(2, new QStandardItem("Len"));
    this->model->setHorizontalHeaderItem(3, new QStandardItem("Cnt(Ant1/2/3/4)"));
    this->model->setHorizontalHeaderItem(4, new QStandardItem("RSSI(dbm)"));
    this->model->setHorizontalHeaderItem(5, new QStandardItem("Channel"));
    this->model->setHorizontalHeaderItem(6, new QStandardItem("Ultima Leitura"));

    // this->ui->tableView->setColumnWidth(0, 50);

    /*setItem设置条目栏中的一个格子的信息*/
    // this->model->setItem(1, 5, new QStandardItem("hello world" ) );

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cmbComPort->addItem(info.portName());
    }

    //QStringList list;
    //QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();//读取所有可用的串口信息
    //int intID = 0;
    //foreach(QSerialPortInfo serialPortInfo,serialPortInfoList)  //打印出端口信息
    //{
    //    QSerialPort serial;
    //    serial.setPort(serialPortInfo);
    //    if(serial.open(QIODevice::ReadWrite))
    //    {
    //        QString strComboShow = (serialPortInfo.portName());
    //        ui->cmbComPort->insertItem(intID,strComboShow,serialPortInfo.portName());
    //        serial.close();
    //    }
        // QString strComboShow = (serialPortInfo.portName());
        // ui-> cmbComPort ->insertItem(intID,strComboShow,serialPortInfo.portName());
    //}
    //ui->cmbComPort->setCurrentIndex(0);
    //ui->cmbComBuad->setCurrentIndex(4);

    work->moveToThread(sub);// 显示数据
    connect(ui->btnStart, &QPushButton::clicked, work, &m_thread::working);
    connect(work, &m_thread::curInfo, this, [=](TagInfo tag)
    {

        QString data;
        for (int i = 0; i < 12; i++) {
            data = data + QString().sprintf("%02X", tag.code[i]) + " ";
        }
        data.replace(" ", "");
        QString tempoAtleta = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

        QString dadosSemEspacos = data.mid(13);

        // Constrói a linha de informação para ser salva no arquivo de texto
        QString txtLine = this->bruteSess + "00000" +dadosSemEspacos + tempoAtleta;

        // Abre o arquivo de texto em modo de escrita (Append)
        QFile txtFile(this->storageBruteFile);  // Substitua pelo caminho correto
        if (txtFile.open(QIODevice::Append | QIODevice::Text)) {
            // Escreve a linha no arquivo de texto
            QTextStream txtStream(&txtFile);
            txtStream << txtLine << "\n";

            // Fecha o arquivo após escrever a linha
            txtFile.close();
        } else {
            qDebug() << "Erro ao abrir o arquivo de texto para escrita.";
        }

        bool flag = false;
        int i32;
        for (int i = 0; i < lstInfo.length(); i++) {
            if (lstInfo[i].Data == data) {
                i32 = i;
                flag = true;
                break;
            }
        }
        if(flag){
            switch (tag.antenna) {
            case 1:
                lstInfo[i32].Ant1 = lstInfo[i32].Ant1+1;
                break;
            case 2:
                lstInfo[i32].Ant2 = lstInfo[i32].Ant2+1;
                break;
            case 3:
                lstInfo[i32].Ant3 = lstInfo[i32].Ant3+1;
                break;
            case 4:
                lstInfo[i32].Ant4 = lstInfo[i32].Ant4+1;
                break;
            default:break;
            }
            lstInfo[i32].RSSI = QString::number((int)tag.rssi/10);
            lstInfo[i32].Channel = QString().sprintf("%d",tag.channel);
            QString ant = QString::number(lstInfo[i32].Ant1)+"/"+QString::number(lstInfo[i32].Ant2)+"/"+QString::number(lstInfo[i32].Ant3)+"/"+QString::number(lstInfo[i32].Ant4);
            this->model->setItem(lstInfo[i32].NO, 3, new QStandardItem(ant));
            this->model->setItem(lstInfo[i32].NO, 4, new QStandardItem(lstInfo[i32].RSSI));
            this->model->setItem(lstInfo[i32].NO, 5, new QStandardItem(lstInfo[i32].Channel));
            this->model->setItem(lstInfo[i32].NO, 6, new QStandardItem(tempoAtleta));

        } else{
            Info _info;
            /*设置行*/
            this->model->setRowCount(lstInfo.length()+1);

            _info.NO = lstInfo.length();
            _info.Data = data;
            _info.Len = QString().sprintf("%d",tag.codeLen);
            _info.Ant1 = 0;
            _info.Ant2 = 0;
            _info.Ant3 = 0;
            _info.Ant4 = 0;
            switch (tag.antenna) {
            case 1:
                _info.Ant1 = _info.Ant1+1;
                break;
            case 2:
                _info.Ant2 = _info.Ant2+1;
                break;
            case 3:
                _info.Ant3 = _info.Ant3+1;
                break;
            case 4:
                _info.Ant4 = _info.Ant4+1;
                break;
            default:break;
            }
            _info.timestamp = QDateTime::currentDateTime();  // Obtenha o timestamp atual
            QString ant = QString::number(_info.Ant1) + "/" + QString::number(_info.Ant2) + "/" + QString::number(_info.Ant3) + "/" + QString::number(_info.Ant4);
            _info.RSSI = QString::number((int)tag.rssi/10);
            _info.Channel = QString().sprintf("%d", tag.channel);
            this->model->setItem(_info.NO, 0, new QStandardItem(QString::number(_info.NO + 1)));
            // this->model->setItem(_info.NO, 1, new QStandardItem(_info.Data));  // Comentado para evitar duplicatas
            this->model->setItem(_info.NO, 2, new QStandardItem(_info.Len));
            this->model->setItem(_info.NO, 3, new QStandardItem(ant));
            this->model->setItem(_info.NO, 4, new QStandardItem(_info.RSSI));
            this->model->setItem(_info.NO, 5, new QStandardItem(_info.Channel));
            lstInfo.append(_info);
            std::vector<size_t> posicoes;

            int posicaoInicialAtleta = 20;
            int posicaoFinalAtleta = 36;
            QString numeroAtleta;
            QString res;
            for (int a = posicaoInicialAtleta; a <= posicaoFinalAtleta && a < _info.Data.length(); ++a) {
                numeroAtleta += _info.Data.at(a);

                int dataNumAtleta = numeroAtleta.remove(' ').toInt();
                QString numCel = QString::number(dataNumAtleta);

                if (numCel.length() > 4) {
                    res = "CHIP " + numCel + " Inválido";
                } else {
                    res = "CHIP " + numCel + " Válido";
                }

                this->model->setItem(_info.NO, 1, new QStandardItem(res));
            }

            // Obtém o timestamp atual formatado como "00:00:00.000"
            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
            QString HoraAtleta = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

//            QString numeroAtletaComTimestamp = numeroAtleta + timestamp;

//            qDebug() << "Número atleta com timestamp:" << numeroAtletaComTimestamp;

            QString data;
            for (int i = 0; i < 12; i++) {
                data = data + QString().sprintf("%02X", tag.code[i]) + " ";
            }

            // Obtém o timestamp atual formatado como "yyyy-MM-dd hh:mm:ss.zzz"
//            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

//            QString txtLine = timestamp + " " + data + " " + QString::number(tag.codeLen) + " " +
//                              QString::number(tag.antenna) + " " +
//                              QString::number((int)tag.rssi / 10) + " " + QString::number(tag.channel);

//            // Abre o arquivo de texto em modo de escrita (Append)
//            QFile txtFile("/tmp/MyTempo/equip_data.txt");  // Substitua pelo caminho correto
//            if (txtFile.open(QIODevice::Append | QIODevice::Text)) {
//                // Escreve a linha no arquivo de texto
//                QTextStream txtStream(&txtFile);
//                txtStream << txtLine << "\n";

//                // Fecha o arquivo após escrever a linha
//                txtFile.close();
//            } else {
//                qDebug() << "Erro ao abrir o arquivo de texto para escrita.";
//            }


            bool flag = false;
            int i32;
            for (int i = 0; i < lstInfo.length(); i++) {
                if (lstInfo[i].Data == data) {
                    i32 = i;
                    flag = true;
                    break;
                }
            }

            QString filePath = this->BaseDir + "/MyTempo/Reader_data/equip_data.json";

            // Verificar se o arquivo existe
            QFile file(filePath);
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
//                        QString nomeEquipamentoResposta = jsonObject.value("modelo").toString();
                        QString idEquipamento = jsonObject.value("equipamento").toString();
                        QString idCheckpoint = jsonObject.value("idcheck").toString();
                        QString idProva =  jsonObject.value("idprova").toString();
//                        QString EquipamentoStatus = jsonObject.value("status").toString();
                        QString local_check = jsonObject.value("identificacao").toString();
//                        QString prova = jsonObject.value("tituloprova").toString();
                        QString hora = jsonObject.value("hora").toString();
//                        QString serie = jsonObject.value("serie").toString();
//                        QString fabricante = jsonObject.value("fabricante").toString();
//                        QString descricao_check = jsonObject.value("descricao_check").toString();
                        QString data_prova = jsonObject.value("data_prova").toString();

                        // CRIA TXT PARA ARMAZENAR OS ATLETAS COM OS DADOS DO EQUIPAMENTO;

                        QFile txtFile(this->storageRefFile);
                        if (txtFile.open(QIODevice::Append | QIODevice::Text)) {
                            QTextStream txtStream(&txtFile);

                            QString formatedNum = numeroAtleta.remove(' ');

                            // Escrever dados no arquivo
                            txtStream << this->equipSession +"000000000000"+ numeroAtleta + HoraAtleta << "\n";

                            // Fechar o arquivo
                            txtFile.close();
                            qDebug() << "Dados adicionados ao arquivo: " << this->storageRefFile;
                        } else {
                            qDebug() << "Erro ao abrir o arquivo para adicionar dados: " << this->storageRefFile;
                        }


                        // Construir o JSON
                        QJsonObject json;
                        json["data_prova"] = data_prova;
                        json["hora_prova"] = hora;
                        json["numero_atleta"] = numeroAtleta;
                        json["tempo_atleta"] = timestamp;
                        json["id_prova"] = idProva;
                        json["id_check"] = idCheckpoint;
                        json["id_equipamento"] = idEquipamento;
                        json["antena"] = 0;
                        json["local"] = local_check;
                        json["entrada"] = 2;
                        json["idstaff"] = 0;

                        // Converter o JSON em QByteArray
                        QJsonDocument jsonDoc(json);
                        QByteArray jsonData = jsonDoc.toJson();

                        // Configurar a solicitação POST
                        QNetworkAccessManager manager;
                        QNetworkRequest request(QUrl("http://api.mytempo.esp.br/api/v1/RecebeTempos.php"));  // Substitua pela URL do seu servidor
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
                            qDebug() << "Resposta do servidor:" << reply->readAll();
                        } else {
                            qDebug() << "Erro ao fazer POST. Código de erro:" << reply->error() << ", Descrição do erro:" << reply->errorString();
                        }
                           // Limpar recursos
                        reply->deleteLater();
                        // Fechar o arquivo após a leitura
                        file.close();

                    } else {
                        qDebug() << "O documento JSON não é um objeto.";
                    }
                } else {
                    qDebug() << "Erro ao abrir o arquivo para leitura.";
                }
            } else {
                qDebug() << "O arquivo não existe.";
            }


        }

    });
}

MainWindow::~MainWindow()
{
    delete ui;
    if(MainWindow::hComm != 0x00){
        CloseDevice(MainWindow::hComm);
    }
}


void MainWindow::on_btnOpen_clicked()
{
    if(MainWindow::hComm != 0x00){
        QMessageBox::information(this,"INFO","There is already an open connection. Please close it before opening it. Thank you for your cooperation!");
        return;
    }
    if(ui->cmbComPort->currentText()==""){
        QMessageBox::information(this,"INFO","Please select Com Port!");
        return;
    }
    if(ui->cmbComBuad->currentText()==""){
        QMessageBox::information(this,"INFO","Please select Com Buad!");
        return;
    }
    QString strPort = "/dev/";
    strPort += ui->cmbComPort->currentText();
    QString BaudRate;
    BaudRate= ui->cmbComBuad->currentText();
    unsigned long result ;
    result = OpenDevice(&MainWindow::hComm ,strPort.toLatin1().data(), BaudRate.toInt());
    if (result == 0x00)
    {
        this->ui->btnClose->setEnabled(true);
        this->ui->btnGetRfPower->setEnabled(true);
        this->ui->btnSetRfPower->setEnabled(true);
        this->ui->btnGetWorkMode->setEnabled(true);
        this->ui->btnSetWorkMode->setEnabled(true);
        this->ui->btnGetFreqBand->setEnabled(true);
        this->ui->btnSetFreqBand->setEnabled(true);
        this->ui->btnReleaseRealy->setEnabled(true);
        this->ui->btnCloseRealy->setEnabled(true);
        this->ui->btnStart->setEnabled(true);
        this->ui->btnStop->setEnabled(true);
        this->ui->btnOpen->setEnabled(false);

        work->hComm = MainWindow::hComm;
        result = GetDevicePara(MainWindow::hComm,&param);
        if (result == 0x00)
        {
            int power = (int)param.RFIDPOWER;
            ui->cmbRfPower->setCurrentIndex(power+1);
            int workmode = (int)param.WORKMODE;
            ui->cmbWorkMode->setCurrentIndex(workmode+1);int freqband = (int)param.REGION;
            FreqInfo freq ;
            freq.region = param.REGION;
            unsigned short inr ;
            unsigned short dec ;
            inr = param.STRATFREI[0]<< 8| param.STRATFREI[1]; // 整数部分
            dec = param.STRATFRED[0]<< 8| param.STRATFRED[1]; // 小数部分
            freq.StartFreq = inr ;
            freq.StopFreq = dec ;
            freq.StepFreq = param.STEPFRE[0]<< 8 | param.STEPFRE[1];
            freq.cnt = param.CN;

            ui->cmbFreqBand->setCurrentIndex(freqband+1);
            float start = (float)inr + (float)dec/1000;
            ui->cmbFreqStart->setCurrentText(QString::number(start,'f',3));
            float end = start + (float)freq.StepFreq/1000 * (float)freq.cnt;
            ui->cmbFreqEnd->setCurrentText(QString::number(end,'f',3));
        }
        QMessageBox::information(this,"INFO","Open sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Open error!");
    }
}

void MainWindow::on_btnClose_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = CloseDevice(MainWindow::hComm);
    if (result == 0x00)
    {
        this->ui->btnClose->setEnabled(false);
        this->ui->btnGetRfPower->setEnabled(false);
        this->ui->btnSetRfPower->setEnabled(false);
        this->ui->btnGetWorkMode->setEnabled(false);
        this->ui->btnSetWorkMode->setEnabled(false);
        this->ui->btnGetFreqBand->setEnabled(false);
        this->ui->btnSetFreqBand->setEnabled(false);
        this->ui->btnReleaseRealy->setEnabled(false);
        this->ui->btnCloseRealy->setEnabled(false);
        this->ui->btnStart->setEnabled(false);
        this->ui->btnStop->setEnabled(false);
        this->ui->btnOpen->setEnabled(true);
        this->ui->cmbRfPower->setCurrentIndex(-1);
        this->ui->cmbWorkMode->setCurrentIndex(-1);
        this->ui->cmbFreqBand->setCurrentIndex(-1);
        this->ui->cmbFreqStart->setCurrentIndex(-1);
        this->ui->cmbFreqEnd->setCurrentIndex(-1);

        MainWindow::hComm = 0x00;
        QFile txtFile(this->storageBruteFile);
        txtFile.close();

        QFile txtFile2(this->storageRefFile);
        txtFile2.close();
        QMessageBox::information(this,"INFO","Close sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Close error!");
    }
}

void MainWindow::on_btnConnect_clicked()
{
    if(MainWindow::hComm != 0x00){
        QMessageBox::information(this,"INFO","There is already an open connection. Please close it before opening it. Thank you for your cooperation!");
        return;
    }
    if(ui->txtIPAddr->toPlainText()==""){
        QMessageBox::information(this,"INFO","Please enter IPAddr.");
        return;
    }
    if(ui->txtPort->toPlainText()==""){
        QMessageBox::information(this,"INFO","Please enter Port.");
        return;
    }
    char strDev[256] = {0};
    QString strIP;
    strIP = ui->txtIPAddr->toPlainText();
    QByteArray c = strIP.toLocal8Bit();
    strcpy(strDev,c.data());
    int iPort = 0;
    QString strPort;
    strPort = ui->txtPort->toPlainText();
    iPort = strPort.toLocal8Bit().toInt();
    unsigned long result;
    result = OpenNetConnection(&MainWindow::hComm , strDev, iPort, 3000);
    if (result == 0x00)
    {
        this->ui->btnDisConnect->setEnabled(true);
        this->ui->btnGetRfPower->setEnabled(true);
        this->ui->btnSetRfPower->setEnabled(true);
        this->ui->btnGetWorkMode->setEnabled(true);
        this->ui->btnSetWorkMode->setEnabled(true);
        this->ui->btnGetFreqBand->setEnabled(true);
        this->ui->btnSetFreqBand->setEnabled(true);
        this->ui->btnReleaseRealy->setEnabled(true);
        this->ui->btnCloseRealy->setEnabled(true);
        this->ui->btnStart->setEnabled(true);
        this->ui->btnStop->setEnabled(true);
        this->ui->btnConnect->setEnabled(false);

        work->hComm = MainWindow::hComm;
        result = GetDevicePara(MainWindow::hComm,&param);
        if (result == 0x00)
        {
            int power = (int)param.RFIDPOWER;
            ui->cmbRfPower->setCurrentIndex(power+1);
            int workmode = (int)param.WORKMODE;
            ui->cmbWorkMode->setCurrentIndex(workmode+1);int freqband = (int)param.REGION;
            FreqInfo freq ;
            freq.region = param.REGION;
            unsigned short inr ;
            unsigned short dec ;
            inr = param.STRATFREI[0]<< 8| param.STRATFREI[1]; // 整数部分
            dec = param.STRATFRED[0]<< 8| param.STRATFRED[1]; // 小数部分
            freq.StartFreq = inr ;
            freq.StopFreq = dec ;
            freq.StepFreq = param.STEPFRE[0]<< 8 | param.STEPFRE[1];
            freq.cnt = param.CN;

            ui->cmbFreqBand->setCurrentIndex(freqband+1);
            float start = (float)inr + (float)dec/1000;
            ui->cmbFreqStart->setCurrentText(QString::number(start,'f',3));
            float end = start + (float)freq.StepFreq/1000 * (float)freq.cnt;
            ui->cmbFreqEnd->setCurrentText(QString::number(end,'f',3));
        }
        QMessageBox::information(this,"INFO","Sucesso ao comunicar!");
    }
    else{
        QMessageBox::critical(this,"WARNING","Error ao comunicar!");
    }
}


void MainWindow::on_btnDisConnect_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = CloseDevice(MainWindow::hComm);
    if (result == 0x00)
    {
        this->ui->btnDisConnect->setEnabled(false);
        this->ui->btnGetRfPower->setEnabled(false);
        this->ui->btnSetRfPower->setEnabled(false);
        this->ui->btnGetWorkMode->setEnabled(false);
        this->ui->btnSetWorkMode->setEnabled(false);
        this->ui->btnGetFreqBand->setEnabled(false);
        this->ui->btnSetFreqBand->setEnabled(false);
        this->ui->btnReleaseRealy->setEnabled(false);
        this->ui->btnCloseRealy->setEnabled(false);
        this->ui->btnStart->setEnabled(false);
        this->ui->btnStop->setEnabled(false);
        this->ui->btnConnect->setEnabled(true);
        this->ui->cmbRfPower->setCurrentIndex(-1);
        this->ui->cmbWorkMode->setCurrentIndex(-1);
        this->ui->cmbFreqBand->setCurrentIndex(-1);
        this->ui->cmbFreqStart->setCurrentIndex(-1);
        this->ui->cmbFreqEnd->setCurrentIndex(-1);

        MainWindow::hComm = 0x00;
        QMessageBox::information(this,"INFO","Desconectado com sucesso!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Erro ao desconectar!");
    }
}


void MainWindow::on_btnGetRfPower_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = GetDevicePara(MainWindow::hComm,&param);
    if (result == 0x00)
    {
        int power = (int)param.RFIDPOWER;
        ui->cmbRfPower->setCurrentIndex(power+1);
        QMessageBox::information(this,"INFO","Get RfPower sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Get RfPower error!");
    }
}


void MainWindow::on_btnSetRfPower_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    QString power = ui->cmbRfPower->currentText();
    if(power==""){
        QMessageBox::information(this,"INFO","Please select RfPower!");
        return;
    }
    unsigned long result = 0x00;
    GetDevicePara(MainWindow::hComm,&param);
    param.RFIDPOWER = power.toInt();
    result = SetDevicePara(MainWindow::hComm,param);
    if (result == 0x00)
    {
        QMessageBox::information(this,"INFO","Set RfPower sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Set RfPower error!");
    }
}


void MainWindow::on_btnGetWorkMode_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = GetDevicePara(MainWindow::hComm,&param);
    if (result == 0x00)
    {
        int workmode = (int)param.WORKMODE;
        ui->cmbWorkMode->setCurrentIndex(workmode+1);
        QMessageBox::information(this,"INFO","Get WorkMode sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Get WorkMode error!");
    }
}


void MainWindow::on_btnSetWorkMode_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    QString workmode = ui->cmbWorkMode->currentText();
    if(workmode==""){
        QMessageBox::information(this,"INFO","Please select WorkMode!");
        return;
    }
    int mode = 0;
    if(workmode == "ActiveMode"){
        mode = 1;
    }
    unsigned long result = 0x00;
    GetDevicePara(MainWindow::hComm,&param);
    param.WORKMODE = mode;
    result = SetDevicePara(MainWindow::hComm,param);
    if (result == 0x00)
    {
        QMessageBox::information(this,"INFO","Set WorkMode sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Set WorkMode error!");
    }
}


void MainWindow::on_cmbFreqBand_currentIndexChanged(int index)
{
    float inr;
    float dec;
    int step;
    int count;
    switch (index) {
    case 2://USA
        inr = 902;
        dec = 750;
        step = 500;
        count = 50;
        break;
    case 3://Korea
        inr = 917;
        dec = 100;
        step = 200;
        count = 15;
        break;
    case 4://Europe
        inr = 865;
        dec = 100;
        step = 200;
        count = 15;
        break;
    case 5://Japan
        inr = 952;
        dec = 200;
        step = 200;
        count = 8;
        break;
    case 6://Malaysia
        inr = 919;
        dec = 500;
        step = 500;
        count = 7;
        break;
    case 7://Europe3
        inr = 865;
        dec = 700;
        step = 600;
        count = 4;
        break;
    case 8://China_1
        inr = 840;
        dec = 125;
        step = 250;
        count = 20;
        break;
    case 9://China_2
        inr = 920;
        dec = 125;
        step = 250;
        count = 20;
        break;
    default:return;
    }

    ui->cmbFreqStart->clear();
    ui->cmbFreqEnd->clear();
    float flo = inr + dec/1000;
    float a = (float)step/1000;
    for(int i =0 ;i<count;i++){
        ui-> cmbFreqStart ->insertItem(i,QString::number(flo,'f',3),QString::number(flo,'f',3));
        ui-> cmbFreqEnd ->insertItem(i,QString::number(flo,'f',3),QString::number(flo,'f',3));
        flo = flo + a;
    }

    ui->cmbFreqEnd->setCurrentIndex(count-1);
}


void MainWindow::on_btnGetFreqBand_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = GetDevicePara(MainWindow::hComm,&param);
    if (result == 0x00)
    {
        int freqband = (int)param.REGION;
        FreqInfo freq ;
        freq.region = param.REGION;
        unsigned short inr ;
        unsigned short dec ;
        inr = param.STRATFREI[0]<< 8| param.STRATFREI[1]; // 整数部分
        dec = param.STRATFRED[0]<< 8| param.STRATFRED[1]; // 小数部分
        freq.StartFreq = inr ;
        freq.StopFreq = dec ;
        freq.StepFreq = param.STEPFRE[0]<< 8 | param.STEPFRE[1];
        freq.cnt = param.CN;

        ui->cmbFreqBand->setCurrentIndex(freqband+1);
        float start = (float)inr + (float)dec/1000;
        ui->cmbFreqStart->setCurrentText(QString::number(start,'f',3));
        float end = start + (float)freq.StepFreq/1000 * (float)freq.cnt;
        ui->cmbFreqEnd->setCurrentText(QString::number(end,'f',3));
        QMessageBox::information(this,"INFO", "Get FreqBand sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Get FreqBand error!");
    }
}


void MainWindow::on_btnSetFreqBand_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    int region = ui->cmbFreqBand->currentIndex();
    if(region==0){
        QMessageBox::information(this,"INFO","Please select FreqBand!");
        return;
    }
    QString start = ui->cmbFreqStart->currentText();
    if(start==""){
        QMessageBox::information(this,"INFO","Please select FreqStart!");
        return;
    }
    QString end = ui->cmbFreqEnd->currentText();
    if(end==""){
        QMessageBox::information(this,"INFO","Please select FreqEnd!");
        return;
    }

    unsigned long result = 0x00;
    GetDevicePara(MainWindow::hComm,&param);
    param.REGION = region-1;
    QStringList list = start.split(QLatin1Char('.'));
    unsigned short inr ;
    unsigned short dec ;
    unsigned short step;
    inr = list[0].toUInt();
    dec = list[1].toUInt();
    param.STRATFREI[0] = *(((char *)&inr)+1);
    param.STRATFREI[1] = *((char *)&inr);
    param.STRATFRED[0] = *(((char *)&dec)+1);
    param.STRATFRED[1] = *((char *)&dec);

    switch (region) {
    case 2://USA
        step = 500;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN = 50;
        break;
    case 3://Korea
        step = 200;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 15;
        break;
    case 4://Europe
        step = 200;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 15;
        break;
    case 5://Japan
        step = 200;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 8;
        break;
    case 6://Malaysia
        step = 500;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 7;
        break;
    case 7://Europe3
        step = 600;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 4;
        break;
    case 8://China_1
        step = 250;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 20;
        break;
    case 9://China_2
        step = 250;
        param.STEPFRE[0] = *(((char *)&step)+1);
        param.STEPFRE[1] = *((char *)&step);
        param.CN  = 20;
        break;
    default:break;
    }
    result = SetDevicePara(MainWindow::hComm,param);
    if (result == 0x00)
    {
        QMessageBox::information(this,"INFO","Set FreqBand sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Set FreqBand error!");
    }
}


void MainWindow::on_btnCloseRealy_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = Close_Relay(MainWindow::hComm,0);
    if (result == 0x00)
    {
        QMessageBox::information(this,"INFO","Close Relay sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Close Relay error!");
    }
}


void MainWindow::on_btnReleaseRealy_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = Release_Relay(MainWindow::hComm,0);
    if (result == 0x00)
    {
        QMessageBox::information(this,"INFO","Release Relay sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Release Relay error!");
    }
}

//Botão iniciar

void MainWindow::on_btnStart_clicked()
{
    QString Status;
    QString msg;
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    this->atualizaViewDados();
    this->equipSession = this->generateNumSeq(3);
    this->bruteSess = this->generateNumSeq(3);
    this->refSess = this->generateNumSeq(3);
    this->ui->sessao_equip->setText("<b>SESSÃO BRUTO: </b>" + this->bruteSess);
    this->ui->label_10->setText("<b>SESSÃO REF.: </b>" + this->equipSession);

    QString directoryPathBrute = this->BaseDir + "/MyTempo/Bruto/";
    QString directoryPathRefined = this->BaseDir + "/MyTempo/Refinados/";

    QDir directoryBrute(directoryPathBrute);
    QDir directoryRefined(directoryPathRefined);

    if (!directoryBrute.exists() && !directoryRefined.exists()) {
        if (!directoryBrute.mkpath(directoryPathBrute)) {
            qDebug() << "Erro ao criar o diretório:" << directoryPathBrute;
            return;
        }
        if (!directoryRefined.mkpath(directoryPathRefined)) {
            qDebug() << "Erro ao criar o diretório:" << directoryPathRefined;
            return;
        }
    }

    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"info","There are currently no open connections!");
        return;
    }
    unsigned long result = 0x00;
    result = InventoryContinue(MainWindow::hComm, 0,0);
//    if (result == 0x00)
//    {
        work->isMonitor = true;
        sub->start();
        QString filePathTxtBrute = this->BaseDir + "/MyTempo/Bruto/MyTempo-Bruto-Sess-" + this->bruteSess + " T-" + timestamp + ".txt";
        QString filePathTxtRefined = this->BaseDir + "/MyTempo/Refinados/MyTempo-Ref-Sess-" + this->equipSession +" T-" + timestamp +  ".txt";
        qDebug() << filePathTxtBrute << " , " << filePathTxtRefined;
        this->storageBruteFile = filePathTxtBrute;
        this->storageRefFile = filePathTxtRefined;
        QFile txtFileBrute(filePathTxtBrute);
        QFile txtFileRef(filePathTxtRefined);

        if (txtFileBrute.open(QIODevice::WriteOnly | QIODevice::Text)) {
            txtFileBrute.close();
        } else {
            qDebug() << "Erro ao criar o arquivo momentaneamente: " << filePathTxtBrute;
        }

        if (txtFileRef.open(QIODevice::WriteOnly | QIODevice::Text)) {
            txtFileRef.close();
        } else {
            qDebug() << "Erro ao criar o arquivo momentaneamente: " << txtFileRef;
        }

        bool conexaoDisponivel = verificarConexaoInternet();
        if (conexaoDisponivel) {
           Status = "Conectado com a internet!";
           msg = "Comunicação iniciada com Sucesso!";
           this->ui->labelStatus->setText("<b>STATUS DE INTERNET:</b> <b style='color:green;'>Online ✔</b>");
        } else {
            Status = "Internet Indisponível!";
            msg = "Comunicação iniciada! <b style='color:red;'>OBS: O equipamento estará operando Offline.<b>";
            this->ui->labelStatus->setText("<b>STATUS DE INTERNET:</b> <b style='color:red;'>Offline 𐄂</b>");

        }
         QMessageBox::information(this, Status, msg);
//    }
//    else
//    {
//        QMessageBox::critical(this,"WARNING","Erro ao tentar-se comunicar!");
//    }
}


void MainWindow::on_btnStop_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"info","There are currently no open connections!");
        return;
    }

    work->isMonitor = false;
    sub->quit();
    sub->wait();

    unsigned long result ;
    result = InventoryStop(MainWindow::hComm,10000);
    if (result == 0x00)
    {
        QMessageBox::information(this,"INFO","Comunicação pausada!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Erro ao parar!");
    }
}

void MainWindow::on_btnScanUSB_clicked()
{
    ui-> cmbUSB->clear();
    int count = CFHid_GetUsbCount();
    if(count>0){
        for(int i=0;i<count;i++){
           char* info;
           CFHid_GetUsbInfo(i,info);
           QString str = QString(QLatin1String(info));
           ui-> cmbUSB ->insertItem(0,str,str);
        }
    }
}

void MainWindow::on_btnUSB_clicked()
{
    unsigned long result ;
    int index = ui->cmbUSB->currentIndex();
    result = OpenHidConnection(&MainWindow::hComm , index);
    if(result == 0x00){
        work->hComm = MainWindow::hComm;
        result = GetDevicePara(MainWindow::hComm,&param);
        if (result == 0x00)
        {
            this->ui->btnUSB_2->setEnabled(true);
            this->ui->btnGetRfPower->setEnabled(true);
            this->ui->btnSetRfPower->setEnabled(true);
            this->ui->btnGetWorkMode->setEnabled(true);
            this->ui->btnSetWorkMode->setEnabled(true);
            this->ui->btnGetFreqBand->setEnabled(true);
            this->ui->btnSetFreqBand->setEnabled(true);
            this->ui->btnReleaseRealy->setEnabled(true);
            this->ui->btnCloseRealy->setEnabled(true);
            this->ui->btnStart->setEnabled(true);
            this->ui->btnStop->setEnabled(true);
            this->ui->btnUSB->setEnabled(false);

            int power = (int)param.RFIDPOWER;
            ui->cmbRfPower->setCurrentIndex(power+1);
            int workmode = (int)param.WORKMODE;
            ui->cmbWorkMode->setCurrentIndex(workmode+1);int freqband = (int)param.REGION;
            FreqInfo freq ;
            freq.region = param.REGION;
            unsigned short inr ;
            unsigned short dec ;
            inr = param.STRATFREI[0]<< 8| param.STRATFREI[1]; // 整数部分
            dec = param.STRATFRED[0]<< 8| param.STRATFRED[1]; // 小数部分
            freq.StartFreq = inr ;
            freq.StopFreq = dec ;
            freq.StepFreq = param.STEPFRE[0]<< 8 | param.STEPFRE[1];
            freq.cnt = param.CN;
            ui->cmbFreqBand->setCurrentIndex(freqband+1);
            float start = (float)inr + (float)dec/1000;
            ui->cmbFreqStart->setCurrentText(QString::number(start,'f',3));
            float end = start + (float)freq.StepFreq/1000 * (float)freq.cnt;
            ui->cmbFreqEnd->setCurrentText(QString::number(end,'f',3));

            QMessageBox::information(this,"INFO","Connect sucess!");
        }
        else{
            QMessageBox::critical(this,"WARNING","Connect error!");
        }
    }else{
        QMessageBox::critical(this,"WARNING","Connect error!");
    }
}


void MainWindow::on_btnUSB_2_clicked()
{
    if(MainWindow::hComm == 0x00){
        QMessageBox::information(this,"INFO","There are currently no open connections, no need to close them!");
        return;
    }
    unsigned long result = 0x00;
    result = CloseDevice(MainWindow::hComm);
    if (result == 0x00)
    {
        this->ui->btnUSB_2->setEnabled(false);
        this->ui->btnGetRfPower->setEnabled(false);
        this->ui->btnSetRfPower->setEnabled(false);
        this->ui->btnGetWorkMode->setEnabled(false);
        this->ui->btnSetWorkMode->setEnabled(false);
        this->ui->btnGetFreqBand->setEnabled(false);
        this->ui->btnSetFreqBand->setEnabled(false);
        this->ui->btnReleaseRealy->setEnabled(false);
        this->ui->btnCloseRealy->setEnabled(false);
        this->ui->btnStart->setEnabled(false);
        this->ui->btnStop->setEnabled(false);
        this->ui->btnUSB->setEnabled(true);
        this->ui->cmbRfPower->setCurrentIndex(-1);
        this->ui->cmbWorkMode->setCurrentIndex(-1);
        this->ui->cmbFreqBand->setCurrentIndex(-1);
        this->ui->cmbFreqStart->setCurrentIndex(-1);
        this->ui->cmbFreqEnd->setCurrentIndex(-1);

        MainWindow::hComm = 0x00;
        QMessageBox::information(this,"INFO","Close sucess!");
    }
    else
    {
        QMessageBox::critical(this,"WARNING","Close error!");
    }
}


void MainWindow::on_pushButton_clicked()
{
    {
        ui_equip = new EquipamentoConfig(this);


        ui_equip->show();
    }
}

void MainWindow::atualizaViewDados() {
    QString filePath = this->SysConfigDir + "MyTempo/equip_data.json";

    // Verificar se o arquivo existe
    QFile file(filePath);
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
//                QString idEquipamento = jsonObject.value("equipamento").toString();
//                QString idCheckpoint = jsonObject.value("idcheck").toString();
//                QString idProva =  jsonObject.value("idprova").toString();
//                QString EquipamentoStatus = jsonObject.value("status").toString();
//                QString local_check = jsonObject.value("identificacao").toString();
                QString prova = jsonObject.value("tituloprova").toString();
                QString hora = jsonObject.value("hora").toString();
//                QString serie = jsonObject.value("serie").toString();
//                QString fabricante = jsonObject.value("fabricante").toString();
                QString descricao_check = jsonObject.value("descricao_check").toString();
//                QString data_prova = jsonObject.value("data_prova").toString();

               if(nomeEquipamentoResposta != "" && descricao_check != "" && prova != "") {
                   this->ui->equipamento_dado1->setText("<b>EQUIPAMENTO</b>: " + nomeEquipamentoResposta);
                   this->ui->checkpoint_dado1->setText("<b>CHECKPOINT</b>: " + descricao_check);
                   this->ui->prova_dado1->setText("<b>PROVA</b>: " + prova);
                   this->ui->hora_prova_dado1->setText("<b>HORA DA PROVA</b>: " + hora);
//                   this->ui->data_prova_dado1->setText("<b>DATA DA PROVA</b>: " + data_prova);
               }
            }
        }
    }
}

QString MainWindow::generateNumSeq(int len) {
    QString RanSeq;
    for (int i = 0; i < len; i++) {
        int numeroAleatorio = QRandomGenerator::global()->bounded(10);
        RanSeq += QString::number(numeroAleatorio);
    }
    return RanSeq;
}


void MainWindow::on_pushButton_2_clicked()
{
    lstInfo.clear();

    // Limpar o modelo da tabela
    model->clear();
    model->setHorizontalHeaderItem(0, new QStandardItem("No"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Data"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Len"));
    model->setHorizontalHeaderItem(3, new QStandardItem("Cnt(Ant1/2/3/4)"));
    model->setHorizontalHeaderItem(4, new QStandardItem("RSSI(dbm)"));
    model->setHorizontalHeaderItem(5, new QStandardItem("Channel"));

    // Atualizar a tabela
    ui->tableView->setModel(model);
}


bool MainWindow::verificarConexaoInternet()
{
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://www.google.com")); // Use um servidor confiável

    QNetworkReply *reply = manager.get(request);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        return true;
    } else {
        return false;
    }
}
//QList<QString> MainWindow::getAllDataWithTimestamps() const
//{
//    QList<QString> dataWithTimestamps;
//    for (const Info& info : lstInfo)
//    {
//        QString record = info.Data + " - " + info.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
//        dataWithTimestamps.append(record);
//    }
//    return dataWithTimestamps;
//}

QList<QJsonObject> MainWindow::getAllDataWithTimestamps() const
{
    QList<QJsonObject> allData;

    for (const Info& info : lstInfo)
    {
        QJsonObject dataObject;
        dataObject["NO"] = info.NO + 1;
        dataObject["Data"] = info.Data;
        dataObject["Len"] = info.Len;
        dataObject["Ant1"] = info.Ant1;
        dataObject["Ant2"] = info.Ant2;
        dataObject["Ant3"] = info.Ant3;
        dataObject["Ant4"] = info.Ant4;
        dataObject["RSSI"] = info.RSSI;
        dataObject["Channel"] = info.Channel;
        dataObject["Timestamp"] = info.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");

        allData.append(dataObject);
    }

    return allData;
}


