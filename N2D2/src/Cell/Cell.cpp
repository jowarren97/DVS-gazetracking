/*
    (C) Copyright 2013 CEA LIST. All Rights Reserved.
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

#include "Cell/Cell.hpp"
#include "DeepNet.hpp"
#include "HeteroStimuliProvider.hpp"

unsigned int N2D2::Cell::mIdCnt = 1;

N2D2::Cell::Cell(const DeepNet& deepNet, const std::string& name, unsigned int nbOutputs)
    : mId(mIdCnt++),
      mDeepNet(deepNet),
      mName(name),
      mOutputsDims(std::vector<size_t>({1U, 1U, nbOutputs})),
      mGroupMap(0),
      mGroupMapInitialized(false)
{
    // ctor
}

void N2D2::Cell::addMultiscaleInput(HeteroStimuliProvider& sp,
                                    unsigned int x0,
                                    unsigned int y0,
                                    unsigned int width,
                                    unsigned int height,
                                    const Tensor<bool>& mapping)
{
    const unsigned int nbMaps = sp.size();

    if (!mapping.empty()) {
        unsigned int nbChannels = 0;

        for (unsigned int map = 0; map < nbMaps; ++map)
            nbChannels += sp[map]->getNbChannels();

        if (mapping.dimY() != nbChannels) {
            throw std::runtime_error("Cell::addMultiscaleInput(): number of "
                                     "mapping rows must be equal to the total "
                                     "number of input filters");
        }
    }

    if (width == 0)
        width = sp[0]->getSizeX() - x0;
    if (height == 0)
        height = sp[0]->getSizeY() - y0;

    unsigned int mappingOffset = 0;

    for (unsigned int map = 0; map < nbMaps; ++map) {
        const unsigned int nbChannels = sp[map]->getNbChannels();
        const double scale = sp[map]->getSizeX()
                             / (double)sp[0]->getSizeX(); // TODO: unsafe legacy
        // code adaptation
        const unsigned int scaledX0 = (unsigned int)(scale * x0);
        const unsigned int scaledY0 = (unsigned int)(scale * y0);
        const unsigned int scaledWidth = (unsigned int)(scale * width);
        const unsigned int scaledHeight = (unsigned int)(scale * height);

        addInput(
            *(sp[map]),
            scaledX0,
            scaledY0,
            scaledWidth,
            scaledHeight,
            (mapping.empty())
                ? Tensor<bool>()
                : mapping.rows(mappingOffset, nbChannels));
        mappingOffset += nbChannels;
    }
}

void N2D2::Cell::save(const std::string& dirName) const
{
    Utils::createDirectories(dirName);

    // Save parameters
    std::ostringstream fileName;
    fileName << dirName << "/" << getType() << ".cfg";
    saveParameters(fileName.str());

    // Save free parameters
    fileName.str(std::string());
    fileName << dirName << "/" << getType() << ".syn";
    saveFreeParameters(fileName.str());
}

void N2D2::Cell::load(const std::string& dirName)
{
    // Load parameters
    std::ostringstream fileName;
    fileName << dirName << "/" << getType() << ".cfg";
    loadParameters(fileName.str());

    // Load free parameters
    fileName.str(std::string());
    fileName << dirName << "/" << getType() << ".syn";
    loadFreeParameters(fileName.str());
}

void N2D2::Cell::setInputsDims(std::initializer_list<size_t> dims)
{
    setInputsDims(std::vector<size_t>(dims));
}

void N2D2::Cell::setInputsDims(const std::vector<size_t>& dims)
{
    if (mInputsDims.empty())
        mInputsDims = dims;
    else if (dims.size() != mInputsDims.size()) {
        std::stringstream msgStr;
        msgStr << "Cell::setInputsDims(): trying to connect an input of"
            " dimension " << dims.size() << " (" << dims << "), but another"
            " input of dimension " << mInputsDims.size() << " (" << mInputsDims
            << ") already exists!" << std::endl;
        throw std::runtime_error(msgStr.str());
    }
    else {
        for (unsigned int n = 0, size = dims.size() - 1; n < size; ++n) {
            if (dims[n] != mInputsDims[n]) {
                std::stringstream msgStr;
                msgStr << "Cell::setInputsDims(): trying to connect an input of"
                    " dims (" << dims << "), but another input already exists"
                    " with dims (" << mInputsDims << ")" << std::endl;
                throw std::runtime_error(msgStr.str());
            }
        }

        mInputsDims.back() += dims.back();
    }
}

size_t N2D2::Cell::groupMap() const
{
    if (!mGroupMapInitialized) {
        mGroupMap = getNbGroups(mMapping);
        mGroupMapInitialized = true;
    }

    return mGroupMap;
}

size_t N2D2::Cell::getNbGroups(const Tensor<bool>& map) const
{
    const size_t nbOutputs = map.dimX();
    const size_t nbChannels = map.dimY();

    // Determine the number of groups
    size_t nbChannelsPerGroup = 0;
    for(size_t ch = 0; ch < nbChannels; ch++) {
        if(map(0, ch)) {
            nbChannelsPerGroup++;
        }
    }

    size_t nbGroups = 0;
    if(nbChannelsPerGroup == 0) {
        throw std::runtime_error("nbChannelsPerGroup equals 0. " + std::to_string(nbChannels));
    }

    if (nbChannels % nbChannelsPerGroup == 0)
        nbGroups = nbChannels / nbChannelsPerGroup;

    // Check that there are really only groups, with nbGroups groups
    size_t outputGroupOffset = 0;
    size_t channelGroupOffset = 0;

    for (size_t group = 0; group < nbGroups; ++group) {
        const size_t outputGroupSize = (nbOutputs - outputGroupOffset)
                                            / (nbGroups - group);

        for (size_t output = outputGroupOffset;
            output < outputGroupOffset + outputGroupSize; ++output)
        {
            size_t channel = 0;

            for (; channel < channelGroupOffset; ++channel) {
                if (map(output, channel)) {
                    nbGroups = 0;
                    goto loops_break;
                }
            }

            for (; channel < channelGroupOffset + nbChannelsPerGroup;
                ++channel)
            {
                if (!map(output, channel)) {
                    nbGroups = 0;
                    goto loops_break;
                }
            }

            for (; channel < nbChannels; ++channel)
            {
                if (map(output, channel)) {
                    nbGroups = 0;
                    goto loops_break;
                }
            }
        }

        outputGroupOffset += outputGroupSize;
        channelGroupOffset += nbChannelsPerGroup;
    }

loops_break:
    return nbGroups;
}
