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

#include "CoordSystem.h"

namespace NumToolKit {

namespace Fea {

coordSystem::QuadrantCoord coordSystem::whichQuadrant(const mathExtension::Point &p)
{
    // what about on the axis??
    if(p.x() > 0 && p.y() > 0 && p.z() < 0 ) return PPN;
    if(p.x() < 0 && p.y() > 0 && p.z() < 0 ) return NPN;
    if(p.x() < 0 && p.y() < 0 && p.z() < 0 ) return NNN;
    if(p.x() > 0 && p.y() < 0 && p.z() < 0 ) return PNN;

    if(p.x() > 0 && p.y() > 0 && p.z() > 0 ) return PPP;
    if(p.x() < 0 && p.y() > 0 && p.z() > 0 ) return NPP;
    if(p.x() < 0 && p.y() < 0 && p.z() > 0 ) return NNP;
    if(p.x() > 0 && p.y() < 0 && p.z() > 0 ) return PNP;
}

coordSystem::QuadrantCoord2D coordSystem::whichQuadrant2D(const mathExtension::Point &p)
{
    // what about on the axis??
    if(p.x() > 0 && p.y() > 0) return PP;
    if(p.x() < 0 && p.y() > 0) return NP;
    if(p.x() < 0 && p.y() < 0) return NN;
    if(p.x() > 0 && p.y() < 0) return PN;
}

coordSystem::QuadrantCoord1D coordSystem::whichQuadrant1D(const mathExtension::Point &p)
{
    // what about on the axis??
    if(p.x() > 0) return P;
    if(p.x() < 0) return N;
}

coordSystem::coordSystem()
    :
      origin0_(QVector3D(0, 0, 0)),
      e0_(QVector3D(1,0,0)),
      e1_(QVector3D(0,1,0)),
      e2_(QVector3D(0,0,1))
{

}

coordSystem::coordSystem\
(\
        QVector3D origin0, \
        QVector3D axisX, \
        QVector3D axisY, \
        QVector3D axisZ\
)
    :
      origin0_(origin0),
      e0_(axisX),
      e1_(axisY),
      e2_(axisZ)
{

}

void coordSystem::setOXYZ(const QVector3D &o, const QVector3D &e0, const QVector3D &e1, const QVector3D &e2)
{
    origin0_ = o;
    e0_ = e0;
    e1_ = e1;
    e2_ = e2;
}

}

}
