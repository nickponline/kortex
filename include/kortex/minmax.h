// ---------------------------------------------------------------------------
//
// This file is part of the <kortex> library suite
//
// Copyright (C) 2013 Engin Tola
//
// See LICENSE file for license information.
//
// author: Engin Tola
// e-mail: engintola@gmail.com
// web   : http://www.engintola.com
//
// ---------------------------------------------------------------------------
#ifndef KORTEX_MINMAX_H
#define KORTEX_MINMAX_H

namespace kortex {

    int min( const float* arr, const int& narr, float& min_val );
    int max( const float* arr, const int& narr, float& max_val );

}

#endif