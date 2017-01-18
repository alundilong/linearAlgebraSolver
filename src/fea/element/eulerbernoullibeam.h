#ifndef EULERBERNOULLIBEAM_H
#define EULERBERNOULLIBEAM_H

#include "feaelementoned.h"

class EulerBernoulliBeam : public FEAElementOneD
{
public:
    static const std::string typeName;
    enum location {LEFT, RIGHT};
    static const int  nNode;
    static const int nDOF;
public:
    EulerBernoulliBeam();
    EulerBernoulliBeam
    (
            const std::string & dimension,
            const std::string & name,
            const MaterialEle &m,
            const GeometryEle &g
    );
    ~EulerBernoulliBeam();
};

#endif // EULERBERNOULLIBEAM_H