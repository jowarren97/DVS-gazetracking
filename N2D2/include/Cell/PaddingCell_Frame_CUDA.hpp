/*
    (C) Copyright 2018 CEA LIST. All Rights Reserved.
    Contributor(s): David BRIAND (david.briand@cea.fr)


    This software is governed by the CeCILL-C license under French law and
    abiding by the rules of distribution of free software.  You can  use,
    modify and/ or redistribute the software under the terms of the CeCILL-C
    license as circulated by CEA, CNRS and INRIA at the following URL
    "http://www.cecill.info".

    As a counterpart to the access to the source code and  rights to copy,
    modify and redistribute granted by the license, users are provided only
    with a limited warranty  and the software's author,  the holder of the
    economic rights,  and the successive licensors  have only  limited
    liability.

    The fact that you are presently reading this means that you have had
    knowledge of the CeCILL-C license and that you accept its terms.
*/

#ifndef N2D2_PADDINGCELL_FRAME_CUDA_H
#define N2D2_PADDINGCELL_FRAME_CUDA_H

#include <tuple>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "Cell_Frame_CUDA.hpp"
#include "DeepNet.hpp"
#include "PaddingCell.hpp"
#include "PaddingCell_Frame_CUDA_Kernels.hpp"

#include "CudaContext.hpp"
#include "CudaUtils.hpp"
#include "containers/CudaTensor.hpp"

namespace N2D2 {
class PaddingCell_Frame_CUDA : public virtual PaddingCell, public Cell_Frame_CUDA<Float_T> {
public:
    PaddingCell_Frame_CUDA(const DeepNet& deepNet, const std::string& name,
                           unsigned int nbOutputs,
                           int topPad,
                           int botPad,
                           int leftPad,
                           int rightPad);
    static std::shared_ptr<PaddingCell> create(const DeepNet& deepNet, const std::string& name,
                                                unsigned int nbOutputs,
                                                int topPad = 0,
                                                int botPad = 0,
                                                int leftPad = 0,
                                                int rightPad = 0)
    {
        return std::make_shared<PaddingCell_Frame_CUDA>(deepNet, name,
                                                        nbOutputs,
                                                        topPad,
                                                        botPad,
                                                        leftPad,
                                                        rightPad);
    }

    virtual void initialize();
    virtual void propagate(bool inference = false);
    virtual void backPropagate();
    virtual void update();
    void checkGradient(double epsilon = 1.0e-4,
                       double maxError = 1.0e-6);
    void discretizeFreeParameters(unsigned int /*nbLevels*/) {}; // no free
    // parameter to
    // discretize
    virtual ~PaddingCell_Frame_CUDA();

protected:
    std::vector<dim3> GPU_BLOCK_GRID;
    std::vector<dim3> GPU_THREAD_GRID;
private:
    static Registrar<PaddingCell> mRegistrar;
};
}

#endif // N2D2_PADDINGCELL_FRAME_CUDA_H
