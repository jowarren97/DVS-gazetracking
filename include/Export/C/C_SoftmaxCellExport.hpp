/*
    (C) Copyright 2016 CEA LIST. All Rights Reserved.
    Contributor(s): Olivier BICHLER (olivier.bichler@cea.fr)

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

#ifndef N2D2_C_SOFTMAXCELLEXPORT_H
#define N2D2_C_SOFTMAXCELLEXPORT_H

#include "Export/C/C_CellExport.hpp"
#include "Export/CellExport.hpp"
#include "Cell/Cell_Frame_Top.hpp"
#include "Export/DeepNetExport.hpp"
#include "Export/SoftmaxCellExport.hpp"

namespace N2D2 {
/**
 * Class for methods for the SoftmaxCell type for the C export
 * SoftmaxCell, C EXPORT
*/
class C_SoftmaxCellExport : public SoftmaxCellExport, public C_CellExport {
public:
    static void generate(const SoftmaxCell& cell, const std::string& dirName);

    static void generateHeaderConstants(const SoftmaxCell& cell,
                                        std::ofstream& header);

    static std::unique_ptr<C_SoftmaxCellExport> getInstance(Cell& cell);
    void generateCellData(Cell& cell,
                          const std::string& outputName,
                          const std::string& outputSizeName,
                          std::ofstream& prog);

    void generateCellFunction(Cell& cell,
                              const std::vector
                              <std::shared_ptr<Cell> >& parentCells,
                              const std::string& inputName,
                              const std::string& outputName,
                              const std::string& outputSizeName,
                              std::ofstream& prog,
                              bool isUnsigned = false,
                              const std::string& funcProto = "",
                              const std::string& memProto = "",
                              bool memCompact = false);
    void generateOutputFunction(Cell& cell,
                                const std::string& inputName,
                                const std::string& outputName,
                                std::ofstream& prog);

private:
    static Registrar<SoftmaxCellExport> mRegistrar;
    static Registrar<C_CellExport> mRegistrarType;
};
}

#endif // N2D2_C_SOFTMAXCELLEXPORT_H
