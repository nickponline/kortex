#ifdef WITH_LAPACK

#include <kortex/lapack_externs.h>
namespace kortex {
    size_t SVD::req_mem( int nr, int nc, bool compute_U, bool compute_VT ) const {
        double work;
        char jobu  = 'N'; if( compute_U  ) jobu  = 'A';
        char jobvt = 'N'; if( compute_VT ) jobvt = 'A';
        int lwork = -1;
        int info;
        int m = nr;
        int n = nc;
        dgesvd_(&jobvt, &jobu, &m, &n, 0,  &m, 0, 0, &m, 0, &n, &work, &lwork, &info);
        return int(work)*sizeof(double);
    }

    void SVD::decompose(const double* A, int nr, int nc, int nld, bool compute_u, bool compute_vt) {

        set_params(nr, nc, compute_u, compute_vt);
        for(int y=0; y<nr; y++)
            m_A.set_row( y, A + y*nld, nc );

        char jobu  = 'N'; if( m_compute_u  ) jobu  = 'A';
        char jobvt = 'N'; if( m_compute_vt ) jobvt = 'A';

        double* work     = (double*)m_memory.get_buffer();
        int     work_sz  = m_memory.capacity()/sizeof(double);
        int     proxy_ld = nc;
        int     m = nr;
        int     n = nc;
        int     info;

        // dont forget - contents of m_A is destroyed here...
        dgesvd_( &jobvt, &jobu, &n, &m, m_A.get_pointer(), &proxy_ld,
                 m_Sd.get_pointer(), m_Vt.get_pointer(), &n, m_U.get_pointer(), &m, work, &work_sz,
                 &info );
    }
}
#endif

