#pragma once

#include <QGraphicsScene>

class EditorGraphicsScene : public QGraphicsScene
{
public:
    EditorGraphicsScene();

private:
    void CreateBackgroundBrush();
};
