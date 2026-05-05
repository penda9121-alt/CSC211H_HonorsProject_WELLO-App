#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recipemanager.h"
#include "caloriechecker.h"

#include <QSettings>
#include <QTimer>
#include <QPushButton>
#include <QPixmap>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , calorieChecker(new CalorieChecker(this))
    , firstTimeAtStartup(true)
    , lastRecipeCategoryPage(0)
{
    ui->setupUi(this);

    // ================= START ON SPLASH PAGE =================
    ui->stackedWidget->setCurrentWidget(ui->splashPage);

    QTimer::singleShot(3000, this, [=]() {
        handleStartupFlow();
    });

    // ================= CALORIE API =================
    connect(ui->checkCaloriesBtn, &QPushButton::clicked, this, [this]() {
        QString food = ui->foodInput->text().trimmed();

        if (food.isEmpty()) {
            ui->caloriesResultLabel->setText("Please enter a food name.");
            return;
        }

        ui->caloriesResultLabel->setText("Searching... 🔍");
        calorieChecker->checkCalories(food);
    });

    connect(calorieChecker, &CalorieChecker::resultReady,
            this, [this](const QString &result) {
                ui->caloriesResultLabel->setText(result);
            });

    // ================= MAIN NAVIGATION =================
    connect(ui->continueBtn, &QPushButton::clicked, this, [=]() {
        saveUserInfo();
        saveSetupCompleted();
        ui->stackedWidget->setCurrentWidget(ui->menuPage);
    });

    connect(ui->recipesBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->recipesPage);
    });

    connect(ui->healthBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->healthPage);
    });

    connect(ui->ingredientsBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->ingredientsPage);
        loadIngredients();
    });

    connect(ui->ingredientsBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->menuPage);
    });

    connect(ui->profileBtn, &QPushButton::clicked, this, [=]() {
        loadUserInfo();
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    });

    // ================= RECIPES NAVIGATION =================
    connect(ui->breakfastBtn, &QPushButton::clicked, this, [=]() {
        lastRecipeCategoryPage = ui->stackedWidget->indexOf(ui->breakfastPage);
        ui->stackedWidget->setCurrentWidget(ui->breakfastPage);
    });

    connect(ui->lunchBtn, &QPushButton::clicked, this, [=]() {
        lastRecipeCategoryPage = ui->stackedWidget->indexOf(ui->lunchPage);
        ui->stackedWidget->setCurrentWidget(ui->lunchPage);
    });

    connect(ui->dinnerBtn, &QPushButton::clicked, this, [=]() {
        lastRecipeCategoryPage = ui->stackedWidget->indexOf(ui->dinnerPage);
        ui->stackedWidget->setCurrentWidget(ui->dinnerPage);
    });

    connect(ui->recipesBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->menuPage);
    });

    connect(ui->breakfastBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->recipesPage);
    });

    connect(ui->lunchBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->recipesPage);
    });

    connect(ui->dinnerBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->recipesPage);
    });

    connect(ui->recipeDetailBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(lastRecipeCategoryPage);
    });

    // ================= HEALTH PAGE BUTTONS =================
    connect(ui->caloriesBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->caloriesPage);
    });

    connect(ui->calorieNeedsBtn, &QPushButton::clicked, this, [=]() {
        loadCalorieNeeds();
        ui->stackedWidget->setCurrentWidget(ui->calorieNeedsPage);
    });

    connect(ui->healthHistoryBtn, &QPushButton::clicked, this, [=]() {
        loadHealthHistory();
        ui->stackedWidget->setCurrentWidget(ui->healthHistoryPage);
    });

    connect(ui->healthBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->menuPage);
    });

    connect(ui->caloriesBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->healthPage);
    });

    connect(ui->caloriesNeedsBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->healthPage);
    });

    connect(ui->healthHistoryBackBtn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->healthPage);
    });

    //============Order ingredients===================

    connect(ui->CheckoutBtn, &QPushButton::clicked, this, [=]() {
        if (cartTotal <= 0) {
            QMessageBox::warning(
                this,
                "Cart Empty",
                "Please add at least one ingredient before checking out."
                );
            return;
        }

        QMessageBox::information(
            this,
            "Order Placed",
            "Your order has been placed and is on its way! 🛒\n\nThank you for shopping with WELLO."
            );

        cartTotal = 0.0;
        ui->cartTotalLabel->setText("🛒 Total: $0.00");
    });

    // ================= ALL RECIPE BUTTONS =================
    for (int i = 1; i <= 15; i++) {
        QString number = QString("%1").arg(i, 2, 10, QChar('0'));

        connectRecipeButton("b" + number + "ImageBtn",
                            "B" + number,
                            ui->stackedWidget->indexOf(ui->breakfastPage));

        connectRecipeButton("l" + number + "ImageBtn",
                            "L" + number,
                            ui->stackedWidget->indexOf(ui->lunchPage));

        connectRecipeButton("d" + number + "ImageBtn",
                            "D" + number,
                            ui->stackedWidget->indexOf(ui->dinnerPage));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ================= STARTUP =================
void MainWindow::handleStartupFlow()
{
    if (isFirstTimeUser())
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    else
        ui->stackedWidget->setCurrentWidget(ui->menuPage);
}

// ================= USER DATA =================
bool MainWindow::isFirstTimeUser()
{
    QSettings settings("WELLO", "App");
    return !settings.value("setupDone", false).toBool();
}

void MainWindow::saveSetupCompleted()
{
    QSettings settings("WELLO", "App");
    settings.setValue("setupDone", true);
}

void MainWindow::saveUserInfo()
{
    QSettings s("WELLO", "App");

    s.setValue("allergies", ui->allergiesInput->text());
    s.setValue("age", ui->ageInput->text());
    s.setValue("gender", ui->genderComboBox->currentText());
    s.setValue("weight", ui->weightInput->text());
    s.setValue("height", ui->heightInput->text());
    s.setValue("activity", ui->activityComboBox->currentText());
    s.setValue("conditions", ui->conditionsInput->text());
}

void MainWindow::loadUserInfo()
{
    QSettings s("WELLO", "App");

    ui->allergiesInput->setText(s.value("allergies").toString());
    ui->ageInput->setText(s.value("age").toString());
    ui->genderComboBox->setCurrentText(s.value("gender").toString());
    ui->weightInput->setText(s.value("weight").toString());
    ui->heightInput->setText(s.value("height").toString());
    ui->activityComboBox->setCurrentText(s.value("activity").toString());
    ui->conditionsInput->setText(s.value("conditions").toString());
}

// ================= HEALTH HISTORY =================
void MainWindow::loadHealthHistory()
{
    QString history;

    history += "Allergies: " + ui->allergiesInput->text() + "\n\n";
    history += "Age: " + ui->ageInput->text() + "\n\n";
    history += "Gender: " + ui->genderComboBox->currentText() + "\n\n";
    history += "Weight: " + ui->weightInput->text() + " kg\n\n";
    history += "Height: " + ui->heightInput->text() + " cm\n\n";
    history += "Activity Level: " + ui->activityComboBox->currentText() + "\n\n";
    history += "Conditions: " + ui->conditionsInput->text() + "\n";

    ui->healthHistoryText->setPlainText(history);
}

// ================= CALORIE NEEDS =================
void MainWindow::loadCalorieNeeds()
{
    double weight = ui->weightInput->text().toDouble();
    double height = ui->heightInput->text().toDouble();
    int age = ui->ageInput->text().toInt();
    QString gender = ui->genderComboBox->currentText();
    QString activity = ui->activityComboBox->currentText();

    if (weight <= 0 || height <= 0 || age <= 0 || gender == "Select" || activity == "Select") {
        ui->calorieNeedsText->setPlainText("Please complete your profile information first.");
        return;
    }

    double bmr = 0;

    if (gender == "Male") {
        bmr = 10 * weight + 6.25 * height - 5 * age + 5;
    } else {
        bmr = 10 * weight + 6.25 * height - 5 * age - 161;
    }

    double factor = 1.2;

    if (activity == "Lightly active") {
        factor = 1.375;
    } else if (activity == "Moderately active") {
        factor = 1.55;
    } else if (activity == "Very active") {
        factor = 1.725;
    }

    double daily = bmr * factor;
    double weekly = daily * 7;
    double monthly = daily * 30;

    QString result;
    result += "Estimated Calorie Needs\n\n";
    result += "Daily: " + QString::number(daily, 'f', 0) + " calories\n";
    result += "Weekly: " + QString::number(weekly, 'f', 0) + " calories\n";
    result += "Monthly: " + QString::number(monthly, 'f', 0) + " calories\n\n";
    result += "For weight loss: around " + QString::number(daily - 400, 'f', 0) + " calories/day\n";
    result += "For weight gain: around " + QString::number(daily + 300, 'f', 0) + " calories/day\n";

    ui->calorieNeedsText->setPlainText(result);
}

// ================= RECIPE SYSTEM =================
void MainWindow::openRecipeFromId(const QString &recipeId, int categoryPageIndex)
{
    Recipe recipe = recipeManager.loadRecipeById(recipeId);

    if (recipe.id.isEmpty()) {
        ui->recipeTitleLabel->setText("Recipe not found");
        ui->recipeDetailsText->setPlainText("Recipe not found.");
        ui->recipeImageLabel->clear();
        ui->stackedWidget->setCurrentWidget(ui->recipesDetailPage);
        return;
    }

    ui->recipeTitleLabel->setText(recipe.name);

    QString details;
    details += "Time: " + recipe.time + "\n";
    details += "Calories: " + recipe.calories + "\n\n";

    details += "Ingredients:\n";
    for (const QString i : recipe.ingredients)
        details += "- " + i + "\n";

    details += "\nInstructions:\n";
    for (const QString step : recipe.instructions)
        details += step + "\n";

    ui->recipeDetailsText->setPlainText(details);

    QString imagePath = recipeManager.getImagePath(recipeId);
    ui->recipeImageLabel->setPixmap(QPixmap(imagePath));
    ui->recipeImageLabel->setScaledContents(true);

    lastRecipeCategoryPage = categoryPageIndex;
    ui->stackedWidget->setCurrentWidget(ui->recipesDetailPage);
}

void MainWindow::connectRecipeButton(const QString &buttonName, const QString &recipeId, int categoryPageIndex)
{
    QPushButton *btn = findChild<QPushButton*>(buttonName);

    if (btn) {
        connect(btn, &QPushButton::clicked, this, [=]() {
            openRecipeFromId(recipeId, categoryPageIndex);
        });
    }
}

// ================= BIO INGREDIENTS SYSTEM =================
void MainWindow::loadIngredients()
{
    cartTotal = 0.0;
    ui->cartTotalLabel->setText("🛒 Total: $0.00");

    ui->scrollArea->setWidgetResizable(true);

    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);

    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(15);
    layout->setContentsMargins(15, 15, 15, 15);

    container->setLayout(layout);
    ui->scrollArea->setWidget(container);

    addIngredientCard("Tomatoes", 2.50, ":/new/BIM/bioIngredientsImage/Tomato.jpg");
    addIngredientCard("Cucumbers", 1.80, ":/new/BIM/bioIngredientsImage/Cucumber.jpg");
    addIngredientCard("Carrots", 1.50, ":/new/BIM/bioIngredientsImage/Carrots.jpg");
    addIngredientCard("Lettuce", 2.25, ":/new/BIM/bioIngredientsImage/Lettuce.jpg");
    addIngredientCard("Black Pepper", 2.00, ":/new/BIM/bioIngredientsImage/Black Pepper.jpg");
    addIngredientCard("Avocado", 1.99, ":/new/BIM/bioIngredientsImage/Avocado.jpg");
    addIngredientCard("Eggs", 3.50, ":/new/BIM/bioIngredientsImage/Eggs.jpg");
    addIngredientCard("Chicken Breast", 5.99, ":/new/BIM/bioIngredientsImage/Chicke Breast.jpg");
    addIngredientCard("Olive Oil", 7.99, ":/new/BIM/bioIngredientsImage/Olive Oil.jpg");
    addIngredientCard("Brown Rice", 2.99, ":/new/BIM/bioIngredientsImage/Rice.jpg");
}

void MainWindow::addIngredientCard(QString name, double price, QString imagePath)
{
    QWidget *card = new QWidget;
    card->setMinimumHeight(125);
    card->setStyleSheet(
        "QWidget {"
        "background-color: #FFFFFF;"
        "border: 1px solid #D8CDBA;"
        "border-radius: 14px;"
        "padding: 10px;"
        "}"
        );

    QHBoxLayout *cardLayout = new QHBoxLayout(card);


    QLabel *imageLabel = new QLabel;
    QPixmap pix(imagePath);

    if (!pix.isNull()) {
        imageLabel->setPixmap(pix.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        imageLabel->setText("No image");
    }

    imageLabel->setFixedSize(100, 100);
    imageLabel->setAlignment(Qt::AlignCenter);

    QLabel *infoLabel = new QLabel;
    infoLabel->setText(name + "\n$" + QString::number(price, 'f', 2));
    infoLabel->setStyleSheet(
        "QLabel {"
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #2E5E4E;"
        "border: none;"
        "}"
        );

    QSpinBox *qty = new QSpinBox;
    qty->setMinimum(1);
    qty->setMaximum(20);
    qty->setValue(1);
    qty->setFixedWidth(70);

    QPushButton *addBtn = new QPushButton("Add to Cart");
    addBtn->setMinimumHeight(38);
    addBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #8BAE75;"
        "color: white;"
        "border-radius: 10px;"
        "padding: 8px 14px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #789B62;"
        "}"
        );

    connect(addBtn, &QPushButton::clicked, this, [=]() {
        int quantity = qty->value();
        cartTotal += price * quantity;

        ui->cartTotalLabel->setText("🛒 Total: $" + QString::number(cartTotal, 'f', 2));

        QMessageBox::information(
            this,
            "Added to Cart",
            QString::number(quantity) + " " + name + " added to cart."
            );
    });

    cardLayout->setSpacing(18);
    cardLayout->setContentsMargins(12, 10, 12, 10);

    cardLayout->addWidget(imageLabel);
    cardLayout->addWidget(infoLabel);
    cardLayout->addStretch();
    cardLayout->addWidget(qty);
    cardLayout->addWidget(addBtn);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollArea->widget()->layout());

    if (layout) {
        layout->addWidget(card);
    }
}