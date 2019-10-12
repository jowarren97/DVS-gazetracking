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

#include "Database/DVS_Database.hpp"
#include "DataFile/DataFile.hpp"
#include "Database/DIR_Database.hpp"
#include "utils/Registrar.hpp"
#include <bitset>

N2D2::DVS_Database::DVS_Database(double validation, Time_T segmentSize, bool loadDataInMemory, AerFormat version)
    : AER_Database(loadDataInMemory)
    , DIR_Database(loadDataInMemory)
    , Database(loadDataInMemory)
    , mValidation(validation)
    , mSegmentSize(segmentSize)
    , mVersion(version)
{
    // ctor
    mValidExtensions.push_back("aedat");
}

/// This loads the database and partitions it into learning and testing samples
void N2D2::DVS_Database::load(const std::string& dataPath,
    const std::string& /*labelPath*/,
    bool /*extractROIs*/)
{
    loadDir(dataPath, 0, "", -1); // loads all files into mStimuli, with name as
                                  // filename as label -1 (unlabelled)

    // Need code for leave one out

    partitionStimuli(1.0 - mValidation, mValidation, 0.0);
    /*
    unsigned int nbClasses = 10;
    unsigned int nbTrainImages = 60000;

    for (unsigned int cls = 0; cls < nbClasses; ++cls) {
        for (unsigned int i = 1; i <= nbTrainImages; ++i) {

            std::ostringstream nameStr;
            nameStr << dataPath << "/Train/" << cls << "/";
            nameStr << std::setfill('0') << std::setw(5) << i << ".bin";

            std::ifstream fileExists(nameStr.str());
            if (fileExists) {

                mStimuli.push_back(Stimulus(nameStr.str(), cls)); //add stimuli
    & class mStimuliSets(Unpartitioned).push_back(mStimuli.size() - 1); //push
    back indices of stimuli
            }
        }
    }
    
    // Assign loaded stimuli to learning and validation set
    partitionStimuli(1.0 - mValidation, mValidation, 0.0);

    unsigned int nbTestImages = 10000;

    for (unsigned int cls = 0; cls < nbClasses; ++cls) {
        std::stringstream clsStr;
        clsStr << cls;
        mLabelsName.push_back(clsStr.str());

        for (unsigned int i = 1; i <= nbTestImages; ++i) {

            std::ostringstream nameStr;
            nameStr << dataPath << "/Test/" << cls << "/";
            nameStr << std::setfill('0') << std::setw(5) << i << ".bin";
    //setfill sets default char of str to '0', while width makes length 5
    (00000)


            std::ifstream fileExists(nameStr.str());
            if (fileExists) {

                mStimuli.push_back(Stimulus(nameStr.str(), cls));
                mStimuliSets(Unpartitioned).push_back(mStimuli.size() - 1);
            }
        }
    }

    // Assign loaded stimuli to test set
    partitionStimuli(0.0, 0.0, 1.0);
        */
}

void N2D2::DVS_Database::loadDir(const std::string& dirPath,
    int depth,
    const std::string& labelName,
    int labelDepth)
{

    {
        if (!((std::string)mDefaultLabel).empty())
            labelID(mDefaultLabel);

        DIR* pDir = opendir(dirPath.c_str());

        if (pDir == NULL)
            throw std::runtime_error(
                "Couldn't open database directory: " + dirPath);

        struct dirent* pFile;
        struct stat fileStat;
        std::vector<std::string> subDirs;
        std::vector<std::string> files;

        std::cout << "Loading directory database \"" << dirPath << "\""
                  << std::endl;

        while ((pFile = readdir(pDir))) {
            const std::string fileName(pFile->d_name);
            const std::string filePath(dirPath + "/" + fileName);

            // Ignore file in case of stat failure
            if (stat(filePath.c_str(), &fileStat) < 0)
                continue;
            // Exclude current and parent directories
            if (!strcmp(pFile->d_name, ".") || !strcmp(pFile->d_name, ".."))
                continue;

            bool masked = false;

            for (std::vector<std::string>::const_iterator it
                 = mIgnoreMasks.begin(),
                 itEnd = mIgnoreMasks.end();
                 it != itEnd; ++it) {
                if (Utils::match((*it), filePath)) {
                    std::cout << Utils::cnotice << "Notice: path \"" << filePath
                              << "\" ignored (matching mask: " << (*it) << ")."
                              << Utils::cdef << std::endl;
                    masked = true;
                }
            }

            if (masked)
                continue;

            if (S_ISDIR(fileStat.st_mode))
                subDirs.push_back(filePath);
            else {
                // Exclude files with wrong extension
                std::string fileExtension = Utils::fileExtension(fileName);
                std::transform(fileExtension.begin(), fileExtension.end(),
                    fileExtension.begin(), ::tolower);

                if (mValidExtensions.empty()
                    || std::find(mValidExtensions.begin(),
                           mValidExtensions.end(), fileExtension)
                        != mValidExtensions.end()) {
                    if (!Registrar<DataFile>::exists(fileExtension)) {
                        std::cout << Utils::cnotice << "Notice: file "
                                  << fileName
                                  << " does not appear to be a valid stimulus,"
                                     " ignoring."
                                  << Utils::cdef << std::endl;
                        continue;
                    }

                    files.push_back(filePath);
                }
            }
        }

        closedir(pDir);

        if (!files.empty()) {
            // Load stimuli contained in this directory
            std::sort(files.begin(), files.end());

            const int dirLabelID = (labelDepth >= 0) ? labelID(labelName) : -1;

            for (std::vector<std::string>::const_iterator it = files.begin(),
                                                          itEnd = files.end();
                 it != itEnd; ++it) {
                // mStimuli.push_back(Stimulus(*it, dirLabelID));
                // mStimuliSets(Unpartitioned).push_back(mStimuli.size() - 1);

                segmentFile(*it);
            }
        }

        if (depth != 0) {
            // Recursively load stimuli contained in the subdirectories
            std::sort(subDirs.begin(), subDirs.end());

            for (std::vector<std::string>::const_iterator it = subDirs.begin(),
                                                          itEnd = subDirs.end();
                 it != itEnd; ++it) {
                if (labelDepth > 0)
                    loadDir(*it, depth - 1,
                        labelName + "/" + Utils::baseName(*it), labelDepth - 1);
                else
                    loadDir(*it, depth - 1, labelName, labelDepth);
            }
        }

        std::cout << "Found " << mStimuli.size() << " stimuli" << std::endl;
    }
}

void N2D2::DVS_Database::loadAerStimulusData(
    std::vector<AerReadEvent>& aerData, StimuliSet set, StimulusID id)
{

    std::string filename = mStimuli[mStimuliSets(set)[id]].name;

    std::vector<AerReadEvent> stimu;

    std::ifstream data(
        filename, std::ios::in | std::ios::binary | std::ios::ate);

    if (data.good()) {

        std::streampos size;
        char* memblock;
        size = data.tellg(); // Get pointer to end of file
        memblock = new char[size]; // Allocate memory for data
        data.seekg(0, std::ios::beg); // Set pointer to beginning
        data.read(memblock, size);
        data.close();

        unsigned int nbEvents = (unsigned int)((int)size / 5);

        for (unsigned int ev = 0; ev < nbEvents; ++ev) {

            unsigned int offset = 5 * ev;

            unsigned int xCoor
                = (unsigned int)((unsigned char)memblock[offset]);
            unsigned int yCoor
                = (unsigned int)((unsigned char)memblock[offset + 1]);

            // Use unsigned int because only 24 bit
            unsigned int bitstring = ((unsigned char)memblock[offset + 2]);
            bitstring = (bitstring << 8) | (unsigned char)memblock[offset + 3];
            bitstring = (bitstring << 8) | (unsigned char)memblock[offset + 4];
            bitstring = bitstring << 8;

            // Bit 24 represents sign
            unsigned int sign = static_cast<unsigned int>(bitstring >> 31);
            // Bit 23-0 represent time
            unsigned int time
                = static_cast<unsigned int>((bitstring << 1) >> (32 - 23));

            aerData.push_back(AerReadEvent(xCoor, yCoor, sign, time));
        }

        delete[] memblock;
    }
    else {
        throw std::runtime_error("DVS_Database::loadAerStimulusData: "
                                 "Could not open AER file: "
            + filename);
    }
}

/// This is only used in clock-simulation
void N2D2::DVS_Database::loadAerStimulusData(std::vector<AerReadEvent>& aerData,
    StimuliSet set,
    StimulusID id,
    Time_T start,
    Time_T stop,
    unsigned int repetitions,
    unsigned int partialStimulus)
{ /*
    std::string filename = mStimuli[mStimuliSets(set)[id]].name;
    // std::cout << filename << std::endl;
    std::ifstream data(filename, std::fstream::binary);
    if (!data.good())
        throw std::runtime_error("Could not open AER file: " + filename);

    if (mStartPositions.find(id)
        != mStartPositions.end()) //&& start == history[fileName].first)
        // Start from last position
        data.seekg(mStartPositions[id]
                       .second); // find starting position to read file from

    std::vector<AerReadEvent> stimu;

    std::ifstream data(filename,
        std::ios::in | std::ios::binary
            | std::ios::ate); // define input file stream

    if (data.good()) { // checks no errors

        std::streampos size;
        char* memblock;
        size = data.tellg(); // Get pointer to end of file
        memblock = new char[size]; // Allocate memory for data
        data.seekg(0, std::ios::beg); // Set pointer to beginning
        data.read(memblock, size); // read data into memblock
        data.close();

        unsigned int nbEvents
            = (unsigned int)((int)size / 5); // not sure why divide by 5?

        for (unsigned int ev = 0; ev < nbEvents; ++ev) {

            unsigned int offset = 5 * ev;

            unsigned int xCoor
                = (unsigned int)((unsigned char)memblock[offset]);
            unsigned int yCoor
                = (unsigned int)((unsigned char)memblock[offset + 1]);

            // Use unsigned int because only 24 bit
            unsigned int bitstring = ((unsigned char)memblock[offset + 2]);
            bitstring = (bitstring << 8) | (unsigned char)memblock[offset + 3];
            bitstring = (bitstring << 8) | (unsigned char)memblock[offset + 4];
            bitstring = bitstring << 8;

            // Bit 24 represents sign
            unsigned int sign = static_cast<unsigned int>(bitstring >> 31);
            // Bit 23-0 represent time
            unsigned int time
                = static_cast<unsigned int>((bitstring << 1) >> (32 - 23));

            stimu.push_back(AerReadEvent(xCoor, yCoor, sign, time));
        }

        delete[] memblock;
    }
    else {
        throw std::runtime_error("DVS_Database::loadAerStimulusData: "
                                 "Could not open AER file: "
            + filename);
    }

    Time_T intervalSize = (stop - start) / repetitions;
    if ((stop - start) % repetitions != 0) {
        std::cout << "start: " << start << std::endl;
        std::cout << "stop: " << stop << std::endl;
        std::cout << "repetitions: " << repetitions << std::endl;
        throw std::runtime_error("DVS_Database::loadAerStimulusData: "
                                 " repetitions not multiple of stop-start");
    }

    unsigned int startCounter = 0;
    unsigned int stopCounter = 0;
    Time_T lastTime = stimu[stimu.size() - 1].time;
    Time_T startTime = 0;

    if (partialStimulus >= 1 && partialStimulus <= 3) {
        if (partialStimulus == 1) {
            lastTime = std::floor(0.33 * stimu[stimu.size() - 1].time);
            startTime = 0;
        }
        else if (partialStimulus == 2) {
            lastTime = std::floor(0.66 * stimu[stimu.size() - 1].time);
            startTime = std::floor(0.33 * stimu[stimu.size() - 1].time);
        }
        else if (partialStimulus == 3) {
            lastTime = std::floor(stimu[stimu.size() - 1].time);
            startTime = std::floor(0.66 * stimu[stimu.size() - 1].time);
        }
        else {
            std::cout << partialStimulus << std::endl;
            throw std::runtime_error("DVS_Database::loadAerStimulusData: "
                                     " partialStimulus value invalid");
        }
        for (std::vector<AerReadEvent>::iterator it = stimu.begin();
             it != stimu.end(); ++it) {
            if ((*it).time <= startTime) {
                startCounter++;
            }
        }

        stimu.erase(stimu.begin(), stimu.begin() + startCounter);

        for (std::vector<AerReadEvent>::iterator it = stimu.begin();
             it != stimu.end(); ++it) {
            if ((*it).time <= lastTime) {
                stopCounter++;
            }
        }

        stimu.erase(stimu.begin() + stopCounter, stimu.end());
    }

    double scalingFactor = ((double)intervalSize) / (lastTime - startTime);

    for (unsigned int i = 0; i < repetitions; ++i) {
        for (std::vector<AerReadEvent>::iterator it = stimu.begin();
             it != stimu.end(); ++it) {
            Time_T scaledTime = std::floor(
                (((*it).time - startTime) * scalingFactor) + i * intervalSize);

            aerData.push_back(
                AerReadEvent((*it).x, (*it).y, (*it).channel, scaledTime));
        }
    }*/
}

void N2D2::DVS_Database::segmentFile(const std::string&
        fileName) // maybe implement start/end clipping for each file (I.e.
                  // separate file stored indicating desired start + end)
{
    std::ifstream data(fileName.c_str(), std::fstream::binary);
    if (!data.good())
        throw std::runtime_error("Could not open AER file: " + fileName);
    if (readVersion(data) != mVersion)
        throw std::runtime_error(fileName + " is of inconsistent DVS format");

    std::pair<Time_T, Time_T> times = getTimes(fileName);
    Time_T startTime = times.first;
    Time_T endTime = times.second;
    unsigned int nbSegments = (endTime - mSegmentSize) % mSegmentStepSize;

    AerEvent event(mVersion);
    unsigned int nbEvents = 0;
    Time_T lastSegmentStart = startTime;
    unsigned int segmentCounter = 0;
    Time_T lastTime = startTime;

    while ((event.read(data).good()) && (segmentCounter < nbSegments)) {
        // Tolerate a lag of 100ms because real AER retina captures are not
        // always non-monotonic
        if (event.time > startTime
                + segmentCounter
                    * mSegmentStepSize) { // add robustness to end of file;
            // if event time is after segment start, assign new stimuli & record
            // stream position (-1 event as just read one)
            mStimuli.push_back(Stimulus(
                fileName.c_str())); // change filename, maybe add _1, _2 etc.

            StimulusID id = mStimuli.size() - 1;
            mStimuliSets(Unpartitioned).push_back(id);
            mStartPositions[id] = std::make_pair(
                lastSegmentStart, (std::streamoff)data.tellg() - event.size());

            ++segmentCounter;
        }

        if (event.time + 100 * TimeMs >= lastTime) {
            // Take the MAX because event.time can be non-monotonic because
            // of AER lag or added jitter
            lastTime = std::max(event.time, lastTime);
            ++nbEvents;
        }
        else if (nbEvents > 0) {
            std::cout << "Current event time is " << event.time / TimeUs
                      << " us, last event time was " << lastTime / TimeUs
                      << " us" << std::endl;
            throw std::runtime_error(
                "Non-monotonic AER data in file: " + fileName);
        }
    }

    std::cout << "Total number of events in file was " << nbEvents << std::endl;
}

std::pair<N2D2::Time_T, N2D2::Time_T> N2D2::DVS_Database::getTimes(
    const std::string& fileName) const
{
    std::ifstream data(fileName.c_str(), std::fstream::binary);

    if (!data.good())
        throw std::runtime_error("Could not open AER file: " + fileName);

    AerEvent event(readVersion(data));

    if (!event.read(data).good())
        throw std::runtime_error("Invalid AER file: " + fileName);

    const Time_T timeStart = event.time;

    data.seekg(-event.size(), std::ios::end);

    if (!event.read(data).good())
        throw std::runtime_error("Invalid AER file: " + fileName);

    return std::make_pair(timeStart, event.time);
}

double N2D2::DVS_Database::readVersion(std::ifstream& data) const
{
    std::string line;
    double version = 1.0; // Default version

    while (data.peek() == '#' && getline(data, line)) {
        if (line.compare(0, 9, "#!AER-DAT") == 0) {
            std::stringstream versionStr(line.substr(9));
            versionStr >> version;
        }
    }

    return version;
}