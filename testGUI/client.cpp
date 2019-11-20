#include "client.h"
#include <QtNetwork>
#include <QtWidgets>

QVector<double> passData;

Client::Client(QWidget *parent)
    : QDialog(parent)
    , hostCombo(new QComboBox)
    , portLineEdit(new QLineEdit)
    , getDataButton(new QPushButton(tr("Generate Number")))
    , clientSocket(new QTcpSocket(this))
    , sender(new SendData(this))
    , runner(new Runnable(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    hostCombo->setEditable(true);

    //find out the name of this machine
    QString name = QHostInfo::localHostName();
    if(!name.isEmpty()) {
        hostCombo->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty()) {
            hostCombo->addItem(name + QChar('.') + domain);
        }
    }

    if (name != QLatin1String("localhost")) {
            hostCombo->addItem(QString("localhost"));
        // find out IP addresses of this machine
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        // add non-localhost addresses
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (!ipAddressesList.at(i).isLoopback())
                hostCombo->addItem(ipAddressesList.at(i).toString());
        }
        // add localhost addresses
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i).isLoopback())
                hostCombo->addItem(ipAddressesList.at(i).toString());
        }
    }

    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    //Displaying the serve info with labels
    auto hostLabel = new QLabel(tr("&Server anme: "));
    hostLabel->setBuddy(hostCombo);
    auto portLabel = new QLabel(tr("&Server port: "));
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("This requires you to run the server as well"));

    getDataButton->setDefault(true);
    getDataButton->setEnabled(false);

    //Creating some new test buttons

    //This one will quit the connection
    auto quitButton = new QPushButton(tr("QUIT"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getDataButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    in.setDevice(clientSocket);
    in.setVersion(QDataStream::Qt_5_13);

    //Button setup
    //Enables the get data button on all fronts
    connect(hostCombo, &QComboBox::editTextChanged,
            this, &Client::enableGetDataButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &Client::enableGetDataButton);
    //Links the data button to the correction function
    connect(getDataButton, &QAbstractButton::clicked,
            this, &Client::requestNewData);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    //Connects the functions to the correct knowns
    connect(clientSocket, &QIODevice::readyRead, this, &Client::readData);
    connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);

    //Sets up the little GUI that displays the text
    QGridLayout *mainLayout = nullptr;

    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QGridLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QGridLayout(this);
    }

    //Adds all the widgets to our GUI
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    //Gives the window a title and focus' it
    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        //Get the saved configurations
        QSettings settings(QSettings::UserScope, QLatin1String("Hyperloop"));
        settings.beginGroup(QLatin1String("Hyperloop network"));
        const QString id = settings.value(QLatin1String("DefaultNetwork")).toString();
        settings.endGroup();

        //Use default if there is no config
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        //Creates a new network session and when we try to open a connection it runs sessionOpened()
        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Client::sessionOpened);

        //Sets up the conenction and button
        getDataButton->setEnabled(false);
        statusLabel->setText(tr("Opening the session"));
        networkSession->open();
    }
}

/**
 * @brief Client::requestNewData
 * This requests the new data from the server
 */
void Client::requestNewData()
{
    //Doesn't allow the button to be pressed and then aborts the connection
    getDataButton->setEnabled(false);
    clientSocket->abort();
    //Then reconnects and gets the data
    clientSocket->connectToHost(hostCombo->currentText(),
                                portLineEdit->text().toInt());
    if (!started)
    {
        sender->start();
        started = !started;
    }
}

/**
 * @brief Client::readData
 * This reads the data that is coming in from the server
 */
void Client::readData()
{
    //Starts to read
    in.startTransaction();

    QVector<double> nextData;
    in >> nextData;

    //If it doens't work get out if it does than get the data
    if(!in.commitTransaction())
        return;

    if (nextData == currentData) {
        QTimer::singleShot(0, this, &Client::requestNewData);
        return;
    }

    //Sets the data to be visible in text and then asks for more after a little wait
    currentData = nextData;

    statusLabel->setText(tr("Voltage: %1\nAmps: %2\nSpeed: %3\nRPM: %4")
            .arg(currentData[0]).arg(currentData[1]).arg(currentData[2]).arg(currentData[3]));
    getDataButton->setEnabled(true);
    Sleep(1);
    passData.clear();
    passData = currentData;
    runner->setMData(passData);
}

/**
 * @brief Client::enableGetDataButton
 * Allows you to press the button that starts the connection
 */
void Client::enableGetDataButton()
{
    //enable the button to get data
    getDataButton->setEnabled((!networkSession || networkSession->isOpen()) &&
                              !hostCombo->currentText().isEmpty() &&
                              !portLineEdit->text().isEmpty());
}

/**
 * @brief Client::sessionOpened
 * This establishes the connection to the server
 */
void Client::sessionOpened()
{
    //Get the used config
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice) {
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfig")).toString();
    } else {
        id = config.identifier();
    }

    QSettings settings(QSettings::UserScope, QLatin1String("Hyperloop"));
    settings.beginGroup(QLatin1String("Hyperloop Network"));
    settings.setValue(QLatin1String("DefaultNetworkConfig"), id);
    settings.endGroup();

    statusLabel->setText(tr("Please run the server connection as well"));

    enableGetDataButton();
}

/**
 * @brief Client::displayError
 * This displays a connection error if there is one
 * @param socketError Contains all the info about the errors
 */
void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Hyperloop Client"),
                                 tr("Host not found check settings"));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Hyperloop Client"),
                                 tr("Connection refused make sure it is running"));
        break;
    default:
        QMessageBox::information(this, tr("Hyperloop Client"),
                                 tr("The following error has occured: %1")
                                 .arg(clientSocket->errorString()));
        //
        //
        //
    }
    getDataButton->setEnabled(true);
}

