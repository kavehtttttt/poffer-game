#include "signup.h"
#include "MainMenu.h"
#include <QFont>
#include <QFrame>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>

signup::signup(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("At the beginning, create an account :)");
    setFixedSize(800, 600);

    QLabel *backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025449.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);
    backgroundLabel->lower();

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addStretch();

    QFrame *formFrame = new QFrame(this);
    formFrame->setFixedSize(400, 480);
    formFrame->setStyleSheet("QFrame { background-color: #F3E4CD; border-radius: 20px; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(12);

    titleLabel = new QLabel("Sign Up", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Segoe UI", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: black;");
    formLayout->addWidget(titleLabel);

    QFont inputFont("Segoe UI", 12);
    QStringList placeholders = {
        "First Name", "Last Name", "Email", "Password", "Phone", "Username"
    };
    QList<QLineEdit*> edits = {
        nameEdit = new QLineEdit(this),
        lastnameEdit = new QLineEdit(this),
        emailEdit = new QLineEdit(this),
        passwordEdit = new QLineEdit(this),
        phoneEdit = new QLineEdit(this),
        usernameEdit = new QLineEdit(this)
    };

    for (int i = 0; i < edits.size(); ++i) {
        edits[i]->setPlaceholderText(placeholders[i]);
        edits[i]->setFont(inputFont);
        edits[i]->setMinimumHeight(38);
        edits[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        edits[i]->setStyleSheet(
            "QLineEdit {"
            " background-color: white;"
            " border: none;"
            " border-radius: 10px;"
            " padding: 8px;"
            " color: black;"
            " font-size: 13px;"
            "}"
            );
        if (placeholders[i] == "Password")
            edits[i]->setEchoMode(QLineEdit::Password);
        formLayout->addWidget(edits[i]);
    }

    submitButton = new QPushButton("Sign Up", this);
    submitButton->setFont(QFont("Segoe UI", 12, QFont::Bold));
    submitButton->setMinimumHeight(42);
    submitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    submitButton->setStyleSheet(
        "QPushButton {"
        " background-color: #AF2C2C;"
        " color: white;"
        " border: none;"
        " border-radius: 10px;"
        " padding: 10px;"
        " font-size: 14px;"
        "}"
        "QPushButton:hover {"
        " background-color: #8B1A1A;"
        "}"
        );
    formLayout->addSpacing(5);
    formLayout->addWidget(submitButton);

    outerLayout->addWidget(formFrame, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Segoe UI", 13, QFont::Bold));
    backButton->setMinimumSize(110, 42);
    backButton->setStyleSheet(
        "QPushButton {"
        " background-color: qradialgradient(cx:0.5, cy:0.5, radius:1.0, fx:0.5, fy:0.5, stop:0 #FF6347, stop:1 #8B0000);"
        " color: white;"
        " border: 3px solid #FFD700;"
        " border-radius: 20px;"
        "}"
        "QPushButton:hover {"
        " background-color: qradialgradient(cx:0.5, cy:0.5, radius:1.0, fx:0.5, fy:0.5, stop:0 #FFA07A, stop:1 #B22222);"
        "}"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    outerLayout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &signup::goBackToMainMenu);
}

void signup::goBackToMainMenu()
{
    this->close();
    MainMenu *mainMenu = new MainMenu();
    mainMenu->setAttribute(Qt::WA_DeleteOnClose);
    mainMenu->show();
}

signup::~signup() {}
