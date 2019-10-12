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

#ifndef N2D2_DVS_DATABASE_H
#define N2D2_DVS_DATABASE_H

#include "Database/AER_Database.hpp"
#include "Database/DIR_Database.hpp"
#include "AerEvent.hpp"

namespace N2D2 {
class DVS_Database : public AER_Database, public DIR_Database {
public:
    DVS_Database(double validation = 0.0, Time_T segmentSize = 1000U, bool loadDataInMemory = true);
    virtual void load(const std::string& dataPath,
                      const std::string& labelPath = "",
                      bool /*extractROIs*/ = false);
    //virtual cv::Mat getStimulusData(StimulusID id);
    virtual void loadAerStimulusData(std::vector<AerReadEvent>& aerData,
                                                StimuliSet set,
                                                StimulusID id);

    virtual void loadAerStimulusData(std::vector<AerReadEvent>& aerData,
                                                    StimuliSet set,
                                                    StimulusID id,
                                                    Time_T start,
                                                    Time_T stop,
                                                    unsigned int repetitions=1,
                                                    unsigned int partialStimulus=0);
    void segmentFile(std::string& fileName);
    std::pair<Time_T, Time_T> getTimes(const std::string& fileName) const;
    double readVersion(std::ifstream& data) const;
    virtual ~DVS_Database() {};

protected:
    double mValidation;
	Time_T mSegmentSize;
    Time_T mSegmentStepSize;
    std::unordered_map<StimulusID, std::pair<Time_T, std::streampos>> mStartPositions; //contains stimulus ID and corresponding timestamp starm & stream position
};
}

#endif // N2D2_DVS_DATABASE_H
