#pragma once

#include <QObject>

class VmAdapter : public QObject
{
    Q_OBJECT

public:
    explicit VmAdapter(QObject *parent = nullptr);
};
