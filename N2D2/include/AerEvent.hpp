/*
    (C) Copyright 2010 CEA LIST. All Rights Reserved.
    Contributor(s): Olivier BICHLER (olivier.bichler@cea.fr)
                    Damien QUERLIOZ (damien.querlioz@cea.fr)

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

#ifndef N2D2_AEREVENT_H
#define N2D2_AEREVENT_H

#include <fstream>
#include <stdexcept>

#include "Network.hpp"
#include "utils/Utils.hpp"

#include <bitset>

namespace N2D2 {
/**
 * This class is a wrapper for reading and writing AER files, used internally by
 * Environment::readAer(), Environment::saveAer()
 * and Environment::aerViewer().
 */
struct AerEvent {
    enum AerFormat {
        N2D2Env,
        Dvs128,
        Dvs240c // EDITED BY ME
    };

    AerEvent(double version = 3.0);
    std::ifstream& read(std::ifstream& data);
    std::ofstream& write(std::ofstream& data) const;
    int size() const;
    void maps(AerFormat format = N2D2Env);
    void unmaps(AerFormat format = N2D2Env);
    static unsigned int unmaps(
        unsigned int map, unsigned int channel, unsigned int node);

    Time_T time;
    unsigned int addr;

    unsigned int map;
    unsigned int channel;
    unsigned int node;
    bool frame;

private:
    template <class T1, class T2>
    typename std::enable_if<std::is_unsigned<T2>::value, std::ifstream&>::type
    read(std::ifstream& data);
    template <class T1, class T2>
    typename std::enable_if<!std::is_unsigned<T2>::value, std::ifstream&>::type
    read(std::ifstream& data);
    template <class T1, class T2>
    std::ofstream& write(std::ofstream& data) const;

    const double mVersion;
    bool mRawTimeNeg;
    unsigned long long int mRawTimeOffset;
};
}

template <class T1, class T2>
typename std::enable_if<std::is_unsigned<T2>::value, std::ifstream&>::type
N2D2::AerEvent::read(std::ifstream& data)
{
    T1 rawAddr;
    T2 rawTime;

    data.read(reinterpret_cast<char*>(&rawAddr), sizeof(rawAddr));
    data.read(reinterpret_cast<char*>(&rawTime), sizeof(rawTime));

    if (!Utils::isBigEndian()) {
        Utils::swapEndian(rawAddr);
        Utils::swapEndian(rawTime);
    }

    addr = static_cast<unsigned int>(rawAddr);
    // AER version = 3
    time = rawTime;

    return data;
}

template <class T1, class T2>
typename std::enable_if<!std::is_unsigned<T2>::value, std::ifstream&>::type
N2D2::AerEvent::read(std::ifstream& data)
{
    frame = false;
    T1 rawAddr;
    T2 rawTime;

    data.read(reinterpret_cast<char*>(&rawAddr), sizeof(rawAddr));
    data.read(reinterpret_cast<char*>(&rawTime), sizeof(rawTime));

    if (!Utils::isBigEndian()) {
        Utils::swapEndian(rawAddr);
        Utils::swapEndian(rawTime);
    }

    if ((int)mVersion == 2) {
        unsigned int addr_temp = static_cast<unsigned int>(rawAddr);

        // unsigned  mask = (1 << 18) - 12;
        // unsigned int mask = 0x0000FFFF;
        // unsigned result = addr & mask;
        if (((addr_temp & (1 << (31))) >> 31) == 1) {
            //std::cout << "frame event found" << std::endl;
            frame = true;
        }

        unsigned xaddr = (((1 << 10) - 1) & (addr_temp >> (12)));
        unsigned yaddr = (((1 << 9) - 1) & (addr_temp >> (22)));

        // DEBUG - works
        // std::cout << std::bitset<32>(addr_temp) << std::endl;
        // std::cout << std::bitset<32>(xaddr) << std::endl;
        // std::cout << std::bitset<32>(yaddr) << std::endl;
        // std::cout << "x address: " << xaddr << std::endl;
        // std::cout << "y address: " << yaddr << std::endl;

        addr = xaddr + yaddr * 240;
        // std::cout << "Full Address: " << std::bitset<32>(addr_temp) <<
        // std::endl; std::cout << "Parsed Address: " << std::bitset<32>(addr) <<
        // std::endl;

        channel = (addr_temp & (1 << 11)) >> 11;
        // std::cout << "Channel: " << std::bitset<32>(channel) << std::endl;

        addr = (addr << 1) + channel; // WORKING FOR AEDAT2.0
        // std::cout << "Address: " << addr << std::endl;
        // std::cout << "New Address: " << std::bitset<32>(addr) << std::endl;
        // std::getchar();
    }

    else {
        addr = static_cast<unsigned int>(rawAddr);
    }

    // Check & correct for overflow
    // (required for "Tmpdiff128-2007-02-28T15-08-15-0800-0 3 flies 2m 1f.dat"
    // for example)
    if (rawTime < 0 && !mRawTimeNeg) {
        // std::cout << "AER time overflow detected! (time is " << rawTime << "
        // us, offset is "
        //          << mRawTimeOffset << " us)" << std::endl;
        mRawTimeOffset += (1ULL << 8 * sizeof(rawTime));
        mRawTimeNeg = true;
    }
    else if (rawTime >= 0 && mRawTimeNeg)
        mRawTimeNeg = false;

    time = (mRawTimeOffset + rawTime) * TimeUs;

    return data;
}

template <class T1, class T2>
std::ofstream& N2D2::AerEvent::write(std::ofstream& data) const
{
    T1 rawAddr = static_cast<T1>(addr);
    T2 rawTime = ((int)mVersion == 3) ? time : time / TimeUs;

    if (!Utils::isBigEndian()) {
        Utils::swapEndian(rawAddr);
        Utils::swapEndian(rawTime);
    }

    data.write(reinterpret_cast<char*>(&rawAddr), sizeof(rawAddr));
    data.write(reinterpret_cast<char*>(&rawTime), sizeof(rawTime));

    if (!data.good())
        throw std::runtime_error("AerEvent::write(): error writing data");

    return data;
}

#endif // N2D2_AEREVENT_H
