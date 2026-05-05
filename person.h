#ifndef PERSON_H
#define PERSON_H

#include <QString>

class Person
{
protected:
    QString name;
    int age;

public:
    void setName(const QString &n) { name = n; }
    void setAge(int a) { age = a; }

    QString getName() const { return name; }
    int getAge() const { return age; }
};

#endif // PERSON_H