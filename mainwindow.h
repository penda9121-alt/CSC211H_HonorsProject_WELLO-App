#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include <QMessageBox>

#include "recipemanager.h"
#include "caloriechecker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer;

    bool firstTimeAtStartup;
    int lastRecipeCategoryPage;

    RecipeManager recipeManager;
    CalorieChecker *calorieChecker;

    // Bio Ingredients / Cart
    double cartTotal = 0.0;
    bool ingredientsLoaded = false;

    void handleStartupFlow();
    bool isFirstTimeUser();
    void saveSetupCompleted();
    void saveUserInfo();
    void loadUserInfo();
    void loadHealthHistory();
    void checkCalories();
    void loadCalorieNeeds();

    void openRecipeFromId(const QString &recipeId, int categoryPageIndex);
    void connectRecipeButton(const QString &buttonName, const QString &recipeId, int categoryPageIndex);

    void loadIngredients();
    void addIngredientCard(QString name, double price, QString imagePath);
};

#endif