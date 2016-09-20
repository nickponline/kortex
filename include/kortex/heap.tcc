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

#include "heap.h"

namespace kortex {

    template<typename HData>
    Heap<HData>::Heap() {
        m_sz   = 0;
        m_cap  = 0;
        m_type = HT_ASCENDING;
    }

    template<typename HData>
    Heap<HData>::~Heap() {
        m_memory.deallocate();
    }

    template<typename HData>
    void Heap<HData>::init( const size_t& init_cap, const HeapType& type ) {
        m_sz   = 0;
        m_cap  = 0;
        m_type = type;

        reserve( init_cap+1 );

        switch( m_type ) {
        case HT_DESCENDING: m_sentinel.heap_val =  DBL_MAX; cmp = heap_max; break;
        case HT_ASCENDING : m_sentinel.heap_val = -DBL_MAX; cmp = heap_min; break;
        }
        m_nodes[0] = &m_sentinel;
    }

    template<typename HData>
    void Heap<HData>::reserve( const size_t& new_cap ) {
        m_memory.resize( new_cap * sizeof(HNode<HData>*) );
        m_nodes = (HNode<HData>**)m_memory.get_buffer();
        m_cap   = new_cap;
    }

    template<typename HData>
    void Heap<HData>::release() {
        m_memory.deallocate();
        m_nodes = NULL;
        m_cap   = 0;
        m_sz    = 0;
    }

    template<typename HData>
    void Heap<HData>::insert( HNode<HData>* node ) {
        assert_pointer( cmp );
        m_sz++;
        if( is_full() ) {
            printf( "resizing heap! %d\n", (int)m_cap*2 );
            reserve( 2*m_cap );
        }
        m_nodes[m_sz] = node;
        upheap(m_sz);
    }

    template<typename HData>
    void Heap<HData>::upheap( size_t k ) {
        assert_pointer( cmp );
        HNode<HData>* v = m_nodes[k];

        while( (*cmp)( v, m_nodes[k>>1] ) ) {
            m_nodes[ k ] = m_nodes[ k >> 1 ];
            m_nodes[ k ]->heap_idx = k;
            k >>= 1;
        }
        m_nodes[k] = v;
        m_nodes[k]->heap_idx = k;

    }

    template<typename HData>
    void Heap<HData>::downheap( size_t k ) {
        assert_pointer( cmp );

        size_t j;
        HNode<HData>* v = m_nodes[k];

        while( k <= m_sz/2 ) {
            j = k << 1;
            if( j < m_sz && (*cmp)( m_nodes[j+1], m_nodes[j] ) )
                j++;
            if( (*cmp)( v, m_nodes[j] ) )
                break;
            m_nodes[k] = m_nodes[j];
            m_nodes[k]->heap_idx = k;
            k = j;
        }
        m_nodes[k] = v;
        m_nodes[k]->heap_idx = k;
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::pop( size_t k ) {
        assert_statement( k <= m_sz && k > 0, "oob" );
        HNode<HData>* node_k = m_nodes[k];
        assert_statement( node_k->heap_idx == k, "invalid node" );
        if( m_sz > 0 ) {
            m_nodes[k] = m_nodes[m_sz];
            m_nodes[k]->heap_idx = k;
            m_sz--;
            update(k);
        }
        return node_k;
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::pop( HNode<HData>* node ) {
        assert_pointer( node );
        return pop( node->heap_idx );
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::pop() {
        if( m_sz == 0 ) return NULL;
        HNode<HData>* top = m_nodes[1];
        if( m_sz > 0 ) {
            m_nodes[1] = m_nodes[m_sz];
            m_nodes[1]->heap_idx = 1;
            m_sz--;
            downheap(1);
        }

        return top;
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::pop_last() {
        if( m_sz == 0 ) return NULL;
        HNode<HData>* last = m_nodes[m_sz];
        m_sz--;
        return last;
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::peek( size_t k ) {
        if( m_sz == 0 ) return NULL;
        assert_statement( k>=1 && k<=m_sz, "invalid idx to pop" );
        return m_nodes[k];
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::peek() {
        if( m_sz == 0 ) return NULL;
        return m_nodes[1];
    }

    template<typename HData>
    void Heap<HData>::update( size_t k ) {
        assert_statement( k>0 && k<=m_sz, "invalid idx to update" );
        if( check_heap_condition(k) )
            upheap(k);
        else
            downheap(k);
    }

    template<typename HData>
    void Heap<HData>::update( HNode<HData>* node ) {
        assert_pointer( node );
        update( node->heap_idx );
    }

    template<typename HData>
    bool Heap<HData>::check_heap_condition( size_t k ) const {
        assert_pointer( cmp );

        HNode<HData>* v = m_nodes[k];
        assert_statement_g( v->heap_idx == k, "invalid node! [k %d]", k );

        if( 2*k+2 <= m_sz ) {
            if( (*cmp)( v, m_nodes[2*k+1] ) &&
                (*cmp)( v, m_nodes[2*k+2] ) )
                return true;
            else
                return false;
        } else if( 2*k+1 <= m_sz ) {
            if( (*cmp)( v, m_nodes[2*k+1] ) )
                return true;
            return false;
        }
        return true;
    }

    template<typename HData>
    HNode<HData>* Heap<HData>::end() {
        if( m_sz == 0 ) return NULL;
        return m_nodes[0];
    }

    template<typename HData>
    bool Heap<HData>::is_heap_healthy() const {
        for( size_t i=0; i<m_sz; i++ ) {
            if( m_nodes[i]->heap_idx != i ) {
                logman_error_g( "heap error: node [%d]", i );
                return false;
            }
        }
        return true;
    }


}



