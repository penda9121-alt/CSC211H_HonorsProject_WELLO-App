#ifndef RECIPEMANAGER_H
#define RECIPEMANAGER_H

#include "recipe.h"
#include <QString>
#include <QStringList>

class RecipeManager
{
public:
    Recipe loadRecipeById(const QString &recipeId);
    QString getImagePath(const QString &recipeId);
};

#endif