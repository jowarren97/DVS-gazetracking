/*
    (C) Copyright 2016 CEA LIST. All Rights Reserved.
    Contributor(s): Johannes THIELE (johannes.thiele@cea.fr)
                    Olivier BICHLER (olivier.bichler@cea.fr)

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

#include "DataFile/DataFile.hpp"
#include "Database/AsyncLabel_Database.hpp"
#include "utils/Registrar.hpp"
#include <bitset>
#include <stdlib.h>

N2D2::AsyncLabel_Database::AsyncLabel_Database(double validation, bool loadDataInMemory)
    : AER_Database(loadDataInMemory), Database(loadDataInMemory)
{
    std::cout << "AsyncLabel Database Construcor" << std::endl;
    // ctor
}
