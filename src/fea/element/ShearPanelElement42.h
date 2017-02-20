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

#ifndef SHEARPANELELEMENT42_H
#define SHEARPANELELEMENT42_H

#include "FeaElementTwoD.h"

namespace NumToolKit {

namespace Fea {

class ShearPanelElement42 : public FEAElementTwoD
{
public:
    static const std::string typeName;
    static const int  nNode;
    static const int nDOF;
public:
    ShearPanelElement42();
    ShearPanelElement42
    (
            const std::string & dim,
            const std::string & name,
            const MaterialEle & m,
            const GeometryEle & g
    );

    // access
    const QList<int>& nodeIds() const {return pointIds_;}

protected:
    virtual void infoAboutThisElement();
    virtual void constructGeometry();
    virtual void constructBaseMatrix();
    virtual void transformToGlobal();
    virtual void numberSequence();

private:
    double f1_;
    double f2_;

};

} // end of Fea

} // end of NumToolKit

#endif // SHEARPANELELEMENT42_H
