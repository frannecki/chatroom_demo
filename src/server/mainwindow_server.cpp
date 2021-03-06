#include "server/mainwindow_server.h"

MainWindow_::MainWindow_(int argc, char **argv, QWidget *parent):
    ui(new Ui::MainWindow_), 
    QMainWindow(parent)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/icon.png"));
    QPixmap pixmap = QPixmap(":/images/syoujyo.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    setPalette(palette);
    QObject::connect(&server_, SIGNAL(logUpdated()), this, SLOT(updateLoggingView()));
    ui->logView->setModel(server_.logModel());
    if(ui->checkbox_remember_settings->isChecked()){
        ReadSettings();
    }
}

MainWindow_::~MainWindow_(){
    delete ui;
}

void MainWindow_::updateLoggingView(){
    ui->logView->scrollToBottom();
}

void MainWindow_::on_checkbox_remember_settings_stateChanged(int state){
    if(state != 0)  ReadSettings();
}

void MainWindow_::on_connect_button_clicked(){
    if(!server_.init(ui->line_edit_addr->text().toStdString(), 
                 ui->line_edit_port->text().toStdString()))
    {
        showNoServerMessage();
    }
    else{
        ui->connect_button->setEnabled(false);
        ui->line_edit_addr->setReadOnly(true);
        ui->line_edit_port->setReadOnly(true);
    }
}

void MainWindow_::on_action_About_triggered(){
    QMessageBox::about(this, tr("About"), 
        tr("<h2>Chatroom Demo App v0.0</h2><p>&#169; 2019 frannecki</p>"));
}

void MainWindow_::on_action_Quit_triggered(){
    close();
}

void MainWindow_::on_quit_button_clicked(){
    close();
}

void MainWindow_::showNoServerMessage(){
    QMessageBox msgBox;
    msgBox.setText("[Error] Couldn't establish server!");
    msgBox.exec();
    close();
}

void MainWindow_::ReadSettings(){
    QSettings settings("Qt_socket_server", "chatroom_demo");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QString addr_srv = settings.value("addr_srv",QString("127.0.0.1")).toString();
    QString port_srv = settings.value("port_srv", QString("11311")).toString();
    ui->line_edit_addr->setText(addr_srv);
    ui->line_edit_port->setText(port_srv);
}

void MainWindow_::WriteSettings(){
    QSettings settings("Qt_socket_server", "chatroom_demo");
    settings.setValue("addr_srv",ui->line_edit_addr->text());
    settings.setValue("port_srv",ui->line_edit_port->text());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow_::closeEvent(QCloseEvent *event){
    printf("[INFO] Exiting...\n");
    server_.closeAll();
    WriteSettings();
    QMainWindow::closeEvent(event);
}
