/* matrix.h -- define types for matrices using Iliffe vectors
 *
 *************************************************************
 * HISTORY
 *
 * 02-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN
 *      Rewrite memory allocation to avoid memory alignment problems
 *      on some machines.
 */

typedef struct {
    int       lb1,
              ub1,
              lb2,
              ub2;
    char     *mat_sto;
    double  **el;
} dmat;

void      print_mat ();
dmat      newdmat ();
int       matmul ();
int       matcopy ();
int       transpose ();
double    matinvert ();
int       solve_system ();

#define freemat(m) free((m).mat_sto) ; free((m).el)
