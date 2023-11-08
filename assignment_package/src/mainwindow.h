#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scene/Mesh.h"
#include <QTreeWidgetItem>
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void populateLists();
    void populateSkeleton();
    void updateJointListWidget(Joint* joint, QTreeWidgetItem* parentItem);
    void updateVertListWidget();
    void updateFaceListWidget();
    void updateHalfEdgeListWidget();
    void updateInfluencedVertices();

private slots:
    void on_actionQuit_triggered();
    void on_actionCamera_Controls_triggered();
    void on_actionImport_OBJ_triggered();
    void on_actionImport_JSON_triggered();
    void onVertexSelected(QListWidgetItem* item);
    void onHalfEdgeSelected(QListWidgetItem* item);
    void onFaceSelected(QListWidgetItem* item);
    void unselectAll();
    void addVertex();
    void triangulateFace();
    void onFaceGreenSpinBoxValueChanged(double value);
    void onFaceBlueSpinBoxValueChanged(double value);
    void onFaceRedSpinBoxValueChanged(double value);
    void onVertPosXSpinBoxValueChanged(double value);
    void onVertPosYSpinBoxValueChanged(double value);
    void onVertPosZSpinBoxValueChanged(double value);
    void subDivision();
    // skeleton part
    void bindSkin();
    void onJointSelected(QTreeWidgetItem* item);
    void onJointPosXAdd();
    void onJointPosXSub();
    void onJointPosYAdd();
    void onJointPosYSub();
    void onJointPosZAdd();
    void onJointPosZSub();

    void onJointRotXAdd();
    void onJointRotXSub();
    void onJointRotYAdd();
    void onJointRotYSub();
    void onJointRotZAdd();
    void onJointRotZSub();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
