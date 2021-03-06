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

#ifndef GEOMETRYELE_H
#define GEOMETRYELE_H

#include "../../coordinateSystem/CoordSystem.h"
#include "../mesh/Mesh.h"
#include "../mesh/FeaOneDMesh.h"
#include "../mesh/FeaTwoDMesh.h"
#include "../../container/List.h"
#include <QList>

namespace NumToolKit {

namespace Fea {

class GeometryEle
{
public:
    enum component {X, Y, Z};
public:
//    GeometryEle(const Mesh & mesh);
    // our mesh is always 3D
    // 2D or 1D problem, variables will be reduced in sepcial way
    GeometryEle(const Mesh & mesh, const QList<int> & vertex);
    GeometryEle(const FEAOneDMesh & mesh, const QList<int> & vertex);
    GeometryEle(const FEATwoDMesh & mesh, const QList<int> & vertex);
//    GeometryEle(const Mesh & mesh, double e[]);
    GeometryEle(const GeometryEle &g);
    ~GeometryEle();

    const double & e(component comp) const { return e_[comp]; }
    void sete(component comp, double val) { e_[comp] = val; }
    const double * e() const { return e_; }
    double *e() { return e_; }
    const double & A() const { return A_; }
    const double & volume() const { return volume_; }

    const QList<int> & vertexIds() const { return vertexIds_; }
    const Mesh & mesh() const { return mesh_; }
    const FEAOneDMesh & oneDMesh() const { return oneDMesh_; }
    const FEATwoDMesh & twoDMesh() const { return twoDMesh_; }
    const QVector3D &center() const { return center_; }

private:
    const Mesh & mesh_;
    const FEAOneDMesh & oneDMesh_;
    const FEATwoDMesh & twoDMesh_;
    double e_[3]; // length in x, y, z direction
    double A_;
    double volume_;

    const QList<int>& vertexIds_;
    QVector3D center_;


};

}

}

#endif // GEOMETRYELE_H
