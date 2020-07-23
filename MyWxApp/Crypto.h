/*
** Copyright (c) 2020 - Jim Brosnahan
**
** This demonstration source is distributed in the hope it will be useful,
** BUT WITHOUT ANY WARRANTY.
**
**
*/

#pragma once
using namespace std;

#include <string>

enum class Modes { ENCRYPT, DECRYPT };

class Crypto
{

public:
    unsigned Process(Modes m, string infile, string outfile, string passphrase);
    unsigned getFileLength(const char* filename);
};

