#pragma once

#include <QObject>

class CompilerAdapter : public QObject
{
    Q_OBJECT

public:
    explicit CompilerAdapter(QObject *parent = nullptr);
};
