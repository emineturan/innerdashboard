#ifndef IBRESCENEITEMEDIT_H
#define IBRESCENEITEMEDIT_H

#include <QWidget>
#include "ui_IbreSceneItemEdit.h"

class IbreSceneItemEdit: public QWidget
{
    Q_OBJECT

    public:
    IbreSceneItemEdit(QWidget *parent = 0);
    ~IbreSceneItemEdit();
    Ui::IbreSceneItemEdit ui;
    private:

};

#endif // IBRESCENEITEMEDIT_H
