/* -*- c++ -*- ----------------------------------------------------------
    NumToolKit - Numerical Simulation ToolKit
    Yijin Mao, ymao.mu@gmail.com

    This file is part of NumToolKit.

    NumToolKit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NumToolKit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NumToolKit.  If not, see <http://www.gnu.org/licenses/>.

    This software is distributed under
    the GNU General Public License.

    See the README file in the top-level NumToolKit directory.
------------------------------------------------------------------------- */

#ifndef FEAANALYSISPANEL_H
#define FEAANALYSISPANEL_H

#include <QWidget>
#include "../fea/mesh/Mesh.h"
#include "../fea/mesh/FeaOneDMesh.h"
#include "../fea/mesh/FeaTwoDMesh.h"
#include "ViewerWindow.h"
#include "../fea/math/MathExtension.h"

namespace Ui {
class feaAnalysisPanel;
}

class MainWindow;

class feaAnalysisPanel : public QWidget
{
    Q_OBJECT

public:
    enum oneDTest{Bar21, BeamEuler22, String21};
    enum twoDTest{ClassicPlate43, Memberane41, ShearPanel42};
    enum threeDTest {LinearCubical83};

    explicit feaAnalysisPanel(QWidget *parent = 0);
    explicit feaAnalysisPanel(MainWindow *mw, ViewerWindow *vwin, QWidget *parent = 0);
    ~feaAnalysisPanel();
    const QString & getLog() {
        return log_;
    }

signals:
    void meshLoaded();

private slots:

    void on_radio2DElement_toggled(bool checked);

    void on_radio1DElement_toggled(bool checked);

    void on_radio3DElement_toggled(bool checked);

    void on_buttonRun_clicked();

    void on_loadMesh_clicked();

private:
    Ui::feaAnalysisPanel *ui;
    MainWindow *mw_;
    ViewerWindow *vwin_;
    QString log_;
    Mesh *mesh_;

    inline const Mesh* mesh() { return mesh_; }

    void solve3DFEA(const threeDTest & test);
    void solveLinearCubicalElement83();

    void solve2DFEA(const twoDTest & test);
    void solveClassicPlate43();
    void solveMembrane41();
    void solveShearPanel42();

    void solve1DFEA(const oneDTest & test);
    void solveBar21();
    void solveBeamEuler22();

    void setBoundaryConditions(const Mesh& mesh, mathExtension::Matrix &A, mathExtension::Vector &b);
    void set2DBoundaryConditions(const FEATwoDMesh & mesh, mathExtension::Matrix &A, mathExtension::Vector &b);
    void set1DBoundaryConditions(const FEAOneDMesh & mesh, mathExtension::Matrix &A, mathExtension::Vector &b);
    void writeData(const Mesh&, const mathExtension::Vector&x);
};

#endif // FEAANALYSISPANEL_H
