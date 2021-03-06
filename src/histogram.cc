// ---------------------------------------------------------------------------
//
// This file is part of the <kortex> library suite
//
// Copyright (C) 2016 Engin Tola
//
// See LICENSE file for license information.
//
// author: Engin Tola
// e-mail: engintola@gmail.com
// web   : http://www.engintola.com
// web   : http://www.aurvis.com
//
// ---------------------------------------------------------------------------

#include <kortex/histogram.h>
#include <kortex/minmax.h>
#include <kortex/check.h>

namespace kortex {

    void Histogram::reset( float mn_val, float mx_val, int num_bins ) {
        assert_statement( num_bins > 1, "invalid n_bins" );
        m_n_samples = 0;
        m_min = mn_val;
        m_max = mx_val;
        // m_bin_step = ( mx_val - mn_val ) / (num_bins-1);
        m_bin_step = ( mx_val - mn_val ) / float(num_bins);
        m_bins.clear();
        m_bins.resize( num_bins, 0 );
    }
    void Histogram::clear_bins() {
        int nbins = (int)m_bins.size();
        m_bins.clear();
        m_bins.resize( nbins, 0 );
    }

    int  Histogram::bin_id( const float& val ) const {
        if( val >= m_max-m_bin_step ) return n_bins()-1;
        if( val <= m_min            ) return 0;
        int bid = (int) ( (val-m_min)/m_bin_step );
        assert_statement_g( bid >= 0 && bid < n_bins(),
                            "oob [%d/%d] [val %f] [mn/mx %f %f]",
                            bid, n_bins(), val, m_min, m_max );
        return bid;
    }
    void Histogram::insert( const float& val ) {
        int bid = bin_id( val );
        assert_boundary( bid, 0, n_bins() );
        m_bins[bid]++;
        m_n_samples++;
    }
    void Histogram::print() const {
        static const int bufsz = 2560;
        int  nstr = 0;
        char buf[bufsz];
        for( int i=0; i<=n_bins(); i++ )
            nstr += sprintf( buf+nstr, " %6.2f", m_min + float(i)*m_bin_step );
        logman_log( buf );
        nstr = 0;
        for( int i=0; i<n_bins(); i++ )
            nstr += sprintf( buf+nstr, " % 6d", m_bins[i] );
        logman_log( buf );
    }

    void Histogram::compute( const vector<float>& arr ) {
        clear_bins();
        for( unsigned i=0; i<arr.size(); i++ ) {
            this->insert( arr[i] );
        }
    }

    void Histogram::compute( const vector<float>& arr, const float& min_value_th ) {
        clear_bins();
        for( unsigned i=0; i<arr.size(); i++ ) {
            if( arr[i] <= min_value_th )
                continue;
            this->insert( arr[i] );
        }
    }

    void Histogram::compute( const float* arr, const size_t& narr, const float& min_value_th ) {
        assert_pointer( arr );
        assert_statement( narr > 0, "passing 0 sized array" );
        clear_bins();
        for( size_t i=0; i<narr; i++ ) {
            if( arr[i] <= min_value_th )
                continue;
            this->insert( arr[i] );
        }
    }


    /// returns the approximate value of the percentage point. it returns the
    /// closest lower bound of the bin which surpasses percentage of samples;
    float Histogram::approximate_value( const float& percentage ) const {

        assert_statement_g( percentage >= 0.0f && percentage <= 100.0f,
                            "invalid percentage request [%f]", percentage );
        passert_statement( m_n_samples > 0, "improper initialization" );
        float rval = m_min;
        float n = 0.0f;
        for( int i=0; i<n_bins(); i++ ) {
            if( n/float(m_n_samples) > percentage/100.0f )
                return rval;
            n += float(m_bins[i]);
            rval = float(i)*m_bin_step + m_min;
        }
        return m_max;
    }

    int Histogram::bin_value( int bid ) const {
        assert_boundary( bid, 0, n_bins() );
        return m_bins[bid];
    }

    int Histogram::max_value() const {
        int max_val=0;
        kortex::max( m_bins, max_val );
        return max_val;
    }

    int Histogram::integrate_till( const int& bid ) const {
        int N = std::min( bid, n_bins() );
        int sv = 0;
        for( int i=0; i<N; i++ )
            sv += m_bins[i];
        return sv;
    }


}
