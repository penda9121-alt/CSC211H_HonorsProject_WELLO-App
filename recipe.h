#ifndef RECIPE_H
#define RECIPE_H

#include <QString>
#include <QStringList>

struct Recipe
{
    QString id;
    QString category;
    QString name;
    QString time;
    QString calories;
    QStringList ingredients;
    QStringList instructions;
};

#endif