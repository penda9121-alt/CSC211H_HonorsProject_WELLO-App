#ifndef USERPROFILE_H
#define USERPROFILE_H

#include "person.h"

class UserProfile : public Person
{
private:
    double weight;
    double height;
    QString gender;
    QString activityLevel;

public:
    void setWeight(double w) { weight = w; }
    void setHeight(double h) { height = h; }
    void setGender(const QString &g) { gender = g; }
    void setActivity(const QString &a) { activityLevel = a; }

    double getWeight() const { return weight; }
    double getHeight() const { return height; }
    QString getGender() const { return gender; }
    QString getActivity() const { return activityLevel; }
};

#endif // USERPROFILE_H