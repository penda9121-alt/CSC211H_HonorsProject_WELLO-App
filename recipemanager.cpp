#include "recipemanager.h"

#include <QFile>
#include <QTextStream>

Recipe RecipeManager::loadRecipeById(const QString &recipeId)
{
    Recipe recipe;

    // Try the possible resource paths
    QStringList paths;
    paths << ":/data/data/WELLO_RECIPES_FULL.txt"
          << ":/new/dataRecipesFull/data/WELLO_RECIPES_FULL.txt"
          << ":/new/dataRecipesFull/WELLO_RECIPES_FULL.txt"
          << "WELLO_RECIPES_FULL.txt";

    QFile file;
    bool opened = false;

    for (const QString &path : paths) {
        file.setFileName(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            opened = true;
            break;
        }
    }

    if (!opened) {
        return recipe;
    }

    QTextStream in(&file);

    Recipe currentRecipe;
    bool insideRecipe = false;
    bool readingIngredients = false;
    bool readingInstructions = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line == "[RECIPE]") {
            currentRecipe = Recipe();
            insideRecipe = true;
            readingIngredients = false;
            readingInstructions = false;
            continue;
        }

        if (!insideRecipe) {
            continue;
        }

        if (line == "---" || line.startsWith("====")) {
            if (currentRecipe.id == recipeId) {
                recipe = currentRecipe;
                break;
            }

            insideRecipe = false;
            readingIngredients = false;
            readingInstructions = false;
            continue;
        }

        if (line.startsWith("ID:")) {
            currentRecipe.id = line.mid(QString("ID:").length()).trimmed();
        }
        else if (line.startsWith("Category:")) {
            currentRecipe.category = line.mid(QString("Category:").length()).trimmed();
        }
        else if (line.startsWith("Name:")) {
            currentRecipe.name = line.mid(QString("Name:").length()).trimmed();
        }
        else if (line.startsWith("Time:")) {
            currentRecipe.time = line.mid(QString("Time:").length()).trimmed();
        }
        else if (line.startsWith("Calories:")) {
            currentRecipe.calories = line.mid(QString("Calories:").length()).trimmed();
        }
        else if (line == "Ingredients:") {
            readingIngredients = true;
            readingInstructions = false;
        }
        else if (line == "Instructions:") {
            readingIngredients = false;
            readingInstructions = true;
        }
        else if (readingIngredients && !line.isEmpty()) {
            if (line.startsWith("- ")) {
                currentRecipe.ingredients.append(line.mid(2).trimmed());
            } else {
                currentRecipe.ingredients.append(line);
            }
        }
        else if (readingInstructions && !line.isEmpty()) {
            currentRecipe.instructions.append(line);
        }
    }

    // In case the last recipe does not end with ---
    if (recipe.id.isEmpty() && currentRecipe.id == recipeId) {
        recipe = currentRecipe;
    }

    file.close();
    return recipe;
}

QString RecipeManager::getImagePath(const QString &recipeId)
{
    if (recipeId.startsWith("B")) {
        return ":/breakfastImages/images/Breakfast/" + recipeId + ".jpg";
    }

    if (recipeId.startsWith("L")) {
        return ":/lunchImages/images/Lunch/" + recipeId + ".jpg";
    }

    if (recipeId.startsWith("D")) {
        return ":/dinnerImages/images/Dinner/" + recipeId + ".jpg";
    }

    return "";
}