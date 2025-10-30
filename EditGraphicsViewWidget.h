#pragma once

#include <QWidget>
#include "ui_EditGraphicsViewWidget.h"
#include "qgraphicsitem.h"
#include "CustomSceneItem.h"
#include "SceneItemEdit.h"


class EditGraphicsViewWidget : public QWidget
{
	Q_OBJECT

public:
	EditGraphicsViewWidget(QWidget *parent = Q_NULLPTR);
	~EditGraphicsViewWidget();
	void addToGraphicsScene(QGraphicsItem* item);
	QGraphicsView* getGraphicsView();

private:
	Ui::EditGraphicsViewWidget ui;
	QGraphicsScene* mScene;
protected:
	void resizeEvent(QResizeEvent* event) override;

	

};
