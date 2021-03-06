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

#include "Mesh.h"
#include <QFile>
#include "OpenFoamQTextstream.h"
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>
#include "../math/MathExtension.h"
#include "../../coordinateSystem/CoordSystem.h"

namespace NumToolKit {

namespace Fea {

const QString Mesh::boundaryFileName = "/boundary";
const QString Mesh::faceFileName = "/faces";
const QString Mesh::ownerFileName = "/owner";
const QString Mesh::neighbourFileName = "/neighbour";
const QString Mesh::pointFileName = "/points";

Mesh::Mesh()
{

}

Mesh::Mesh(QString & dir)
{
    nCells_ = 0;
    nFaces_ = 0;
    nNodes_ = 0;

    bool readState = readPoints(dir);
    readState = readState && readFaces(dir);
    readState = readState && readOwner(dir);
    readState = readState && readNeighbour(dir);
    readState = readState && readBoundary(dir);

    if(readState) {
        // find the total number of cell
        nCells_ = *std::max_element(owner().begin(), owner().end()) + 1;
        qDebug() << "========= Total Number of cell: =========" << nCells();
        // find the total number of face
        nFaces_ = faces().size();
        nNodes_ = points().size();

        createCellFaces();
        createCellNodes();
        createFaceNormalAndAreas();

        QList<double> px;
        QList<double> py;
        QList<double> pz;

        QList<QVector3D>::const_iterator it;
        for(it = points().begin(); it != points().end(); ++it) {
            px.push_back(((*it).x()));
            py.push_back(((*it).y()));
            pz.push_back(((*it).z()));
        }
        box_.xlow = *std::min_element(px.begin(),px.end());
        box_.xhig = *std::max_element(px.begin(),px.end());
        box_.ylow = *std::min_element(py.begin(),py.end());
        box_.yhig = *std::max_element(py.begin(),py.end());
        box_.zlow = *std::min_element(pz.begin(),pz.end());
        box_.zhig = *std::max_element(pz.begin(),pz.end());

    }

    meshState_ = readState;
}

Mesh::~Mesh()
{

}

const QList<int> Mesh::faceNodes(int faceId) const
{
    return faces()[faceId];
}

const QList<int> Mesh::cellNodes(int cellId) const
{
    return cellNode_[cellId];
}

const QList<int> Mesh::cellFaces(int cellId) const
{
    return cellFace_[cellId];
}

void Mesh::numberSequence(ElementShape shape, QList<QList<int> >& vertexList) const
{
    int size = nCells();
    vertexList.reserve(size);
//    for (int i = 0; i < size; i++) {
//        vertexList[i] = List<int>(8);
//    }

    switch(shape) {
    case Cubic : {

        for (int i = 0; i < size; i++) {
            QList<int> q;
            vertexList.append(q);
        }

        for (int iCell = 0; iCell < size; iCell++) {

            int num = cellNodes(iCell).size();
            int nodeIds[num];
            for(int i = 0; i < num; i++) {
                nodeIds[i] = cellNodes(iCell)[i];
            }
            std::list<int> uniqueList(nodeIds, nodeIds+num);
            uniqueList.sort();
            uniqueList.unique();

            double xc, yc, zc; // cube center
            xc = yc = zc = 0.0;
            int c = 0;
            std::list<int>::const_iterator it;
            for (it = uniqueList.begin(); it != uniqueList.end(); ++it) {

                xc += points()[(*it)].x();
                yc += points()[(*it)].y();
                zc += points()[(*it)].z();
                c++;
            }

            xc = xc/c;
            yc = yc/c;
            zc = zc/c;

            mathExtension::Point vc(xc,yc,zc);

            c = 0;
            QList<int> vertex;
            vertex.reserve(8);
            for (int i = 0; i < 8; i++) {
                int ii;
                vertex.append(ii);
            }

            for (it = uniqueList.begin(); it != uniqueList.end(); ++it) {
                const int & vertexId = *it;

                double x = points()[vertexId].x();
                double y = points()[vertexId].y();
                double z = points()[vertexId].z();

                mathExtension::Point v(x,y,z);
                mathExtension::Point vd(v-vc);

                switch(coordSystem::whichQuadrant(vd)) {
                case coordSystem::PPP: {vertex[7-1] = vertexId; break;}
                case coordSystem::NPP: {vertex[6-1] = vertexId; break;}
                case coordSystem::NNP: {vertex[2-1] = vertexId; break;}
                case coordSystem::PNP: {vertex[3-1] = vertexId; break;}

                case coordSystem::PPN: {vertex[8-1] = vertexId; break;}
                case coordSystem::NPN: {vertex[5-1] = vertexId; break;}
                case coordSystem::NNN: {vertex[1-1] = vertexId; break;}
                case coordSystem::PNN: {vertex[4-1] = vertexId; break;}
                }
            }

            vertexList[iCell] = vertex;

    }
    case Tetrahedron: {
            break;
    }
    default:{
            break;
    }

    }
    }
}

void Mesh::fetchBCUniqueVertex(const QString &name, QList<int> &vertex) const
{
    QMap<QString, QList<int> >::const_iterator it;
    it = boundaryNameFaces().find(name);

    if(it != boundaryNameFaces().end()) {
        const QList<int> & faceList = boundaryNameFaces()[name];
        QList<int>::const_iterator it2;

        QList<int> allNodes;

        for(it2 = faceList.begin(); it2 != faceList.end(); ++it2) {
            int faceI = *it2;
            const QList<int> & faceNode = faceNodes(faceI);
            int nNode = faceNode.size();
            for(int i = 0; i < nNode; i++) {
                allNodes.push_back(faceNode[i]);
            }
        }
        int num = allNodes.size();
        int nodeIds[num];
        QList<int>::const_iterator itNode;
        int c = 0;
        for (itNode = allNodes.begin(); itNode != allNodes.end(); ++itNode) {
            nodeIds[c++] = *itNode;
        }

        std::list<int> uniqueList(nodeIds, nodeIds+num);
        uniqueList.sort();
        uniqueList.unique();

        std::list<int>::const_iterator it3;
        for (it3 = uniqueList.begin(); it3 != uniqueList.end(); ++it3) {
            vertex.push_back(*it3);
        }

    } else {
        for (it = boundaryNameFaces().begin(); it != boundaryNameFaces().end(); ++it) {
            qDebug() << (*it).first();
        }
    }
}


bool Mesh::readPoints(QString &dir)
{
    QString fileName = dir + pointFileName;
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(NULL,"..", fileName+" fail to open!");
        return false;
    }

    OpenFOAMQTextStream in(&file);
    in.ignore(QString("//"),2);

    // size
    int size;
    in >> size;

    qDebug() << "Node size: " << size;

    in.ignore(QString("("),1);

    for (int i = 0; i < size; i++) {
        QString val;
        double point[3];
        for (int j = 0; j < 3; j++) {
            in >> val;
            val.remove(QRegExp("[()]"));
//            val.remove(")");
            point[j] = val.toDouble();

        }
//        qDebug() << point[0] << "\t" << point[1] << "\t" << point[2];
        points_.push_back(QVector3D(point[0],point[1],point[2]));
    }

    file.close();
    return true;
}

bool Mesh::readFaces(QString &dir)
{
    QString fileName = dir + faceFileName;
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(NULL,"..", fileName+" fail to open!");
        return false;
    }

    OpenFOAMQTextStream in(&file);
    in.ignore(QString("//"),2);

    // size
    int size;
    in >> size;

    qDebug() << "Face size: " << size;

    in.ignore(QString("("),1);

    for (int i = 0; i < size; i++) {
        QString val;
        in >> val;
        QStringList str = val.split("(");
        const int nVal = str[0].toInt();
        int pIds[nVal];
        QList<int> pIdsFace;

        pIds[0] = str[1].toInt();
        pIdsFace.push_back(pIds[0]);
//        qDebug() << "nVal: " << nVal << " " << pIds[0];

        for (int j = 1; j < nVal; j++) {
            in >> val;
            val.remove(QRegExp("[()]"));
//            val.remove(")");
            pIds[j] = val.toInt();
            pIdsFace.push_back(pIds[j]);
//            qDebug() << pIds[j];
        }
//        qDebug() << pIdsFace;
        faces_.push_back(pIdsFace);
    }

    file.close();
    return true;
}

bool Mesh::readBoundary(QString &dir)
{
    QString fileName = dir + boundaryFileName;
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(NULL,"..", fileName+" fail to open!");
        return false;
    }

    OpenFOAMQTextStream in(&file);
    in.ignore(QString("//"),2);

    // size
    int size;
    in >> size;

    qDebug() << "Boundary size: " << size;

    in.ignore(QString("("),1);

    QString boundaryName;
    for (int i = 0; i < size; i++) {
        in >> boundaryName;
        in.ignore(QString("nFaces"),1); // until find the 1st string
        int nFaces;
        in >> nFaces;
        in.ignore(QString("startFace"),1); // until find the 1st string
        int startFace;
        in >> startFace;
        in.ignore(QString("}"),1);

        QList<int> faceList;
        for (int j = 0; j < nFaces; j++) {
            faceList.push_back(startFace + j);
        }
        boundaryNameFaces_[boundaryName] = faceList;
        boundaryName_.push_back(boundaryName);
//        qDebug() << boundaryName << " " << boundaryNameFaces_[boundaryName];
    }

    file.close();
    return true;
}

bool Mesh::readOwner(QString &dir)
{
    QString fileName = dir + ownerFileName;
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(NULL,"..", fileName+" fail to open!");
        return false;
    }

    OpenFOAMQTextStream in(&file);
    in.ignore(QString("//"),2);

    // size
    int size;
    in >> size;

    qDebug() << "Owner size: " << size;

    in.ignore(QString("("),1);

    int cellId;
    for (int i = 0; i < size; i++) {
        in >> cellId;
        owner_.push_back(cellId);
//        qDebug() << QString("%1 : %2").arg(i+1).arg(cellId);
    }

    file.close();
    return true;
}

bool Mesh::readNeighbour(QString &dir)
{
    QString fileName = dir + neighbourFileName;
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(NULL,"..", fileName+" fail to open!");
        return false;
    }

    OpenFOAMQTextStream in(&file);
    in.ignore(QString("//"),2);

    // size
    int size;
    in >> size;

    qDebug() << "Neighbour size: " << size;

    in.ignore(QString("("),1);

    int cellId;
    for (int i = 0; i < size; i++) {
        in >> cellId;
        neighbour_.push_back(cellId);
//        qDebug() << QString("%1 : %2").arg(i+1).arg(cellId);
    }

    file.close();
    return true;
}

void Mesh::createCellFaces()
{
    // create cell to face, using owner and neighbour
    cellFace_.reserve(nCells());
    for(int i = 0; i < nCells(); i++) {
        QList<int> q;
        cellFace_.append(q);
    }

    // get faces from owner
    qDebug() << "Get faces from owner...";
    QList<int>::const_iterator it;
    int faceId = 0;
    for (it = owner().begin(); it != owner().end(); ++it) {
        int cellId = *it;
//        qDebug() << cellId << " " << faceId;
        QList<QList<int> >::iterator it2;
        it2 = cellFace_.begin() + cellId;
        (*it2).push_back(faceId);
        faceId++;
    }

    // get faces from neighbour
    qDebug() << "Get faces from neighbour...";
    faceId = 0;
    for (it = neighbour().begin(); it != neighbour().end(); ++it) {
        int cellId = *it;
//        qDebug() << cellId << " " << faceId;
        QList<QList<int> >::iterator it2;
        it2 = cellFace_.begin() + cellId;
        (*it2).push_back(faceId);
        faceId++;
    }

//    int cellId = 0;
//    QList<QList<int> >::const_iterator it2;
//    for(it2 = cellFace().begin(); it2 != cellFace().end(); ++it2){
//        qDebug() << ++cellId <<" "<< (*it2);
//    }

}


void Mesh::createCellNodes()
{
    // duplicate node
    cellNode_.reserve(nCells());
    for(int i = 0; i < nCells(); i++) {
        QList<int> q;
        cellNode_.append(q);
    }

    // loop over all cells
    int cellId = 0;
    for (cellId = 0; cellId < nCells(); cellId++) {
        // find faceIds of cellId
        const QList<int> & faceIds = cellFaces(cellId);
        QList<int>::const_iterator it;
//        qDebug() << "cellId: " << cellId << " " << faceIds;

        QList<QList<int> >::iterator it2;
        it2 = cellNode_.begin() + cellId;

        for(it = faceIds.begin(); it != faceIds.end(); ++it) {
            int faceId = *it;
            QList<int>::const_iterator it3;
            const QList<int> nodeIds = faceNodes(faceId);

            for(it3 = nodeIds.begin(); it3 != nodeIds.end(); ++it3) {
                (*it2).push_back((*it3));
            }
        }
    }

}

void Mesh::createFaceNormalAndAreas()
{
    // duplicate node
//    faceNormals_.reserve(nFaces());
//    faceAreas_.reserve(nFaces());
//    for(int i = 0; i < nFaces(); i++) {
//        QVector3D v;
//        double a;
//        faceNormals_.append(v);
//        faceAreas_.append(a);
//    }

    // loop over all faces
    int faceId = 0;
    for (faceId = 0; faceId < nFaces(); faceId++) {
        // find nodeIds of faceId
        const QList<int> & nodeIds = faceNodes(faceId);
        QList<int>::const_iterator it;
        it = nodeIds.begin();
        QVector3D p1 = points()[*it];
        QVector3D p2, p3;
        bool normalComputed = false;
        QVector3D normal;
        double area = 0;
        for (; it != nodeIds.end();)
        {
            // first time
            if(!normalComputed) {
                ++it;
                p2 = points()[*it];
                ++it;
                p3 = points()[*it];
                normal = QVector3D::normal(p1, p2, p3);
                faceNormals_.push_back(normal);
//                qDebug() << "normal : " << normal << p1 << p2 << p3;
                normalComputed = true;

                QVector3D p12 = p2 - p1;
                QVector3D p13 = p3 - p1;
                area = QVector3D::crossProduct(p12,p13).length()*0.5;

                ++it;
            }
            QVector3D pNext = points()[*it];
            area += QVector3D::crossProduct(pNext-p1, p3-p1).length()*0.5;
            p3 = pNext;
            ++it;
        }
//        qDebug() << "area : " << area;
        faceAreas_.push_back(area);
    }
}

}

}
