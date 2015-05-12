#include <stdio.h>
#include <errno.h>
#include "curve.h"
#include "trace.h"

int main()
{
  /*point_t cpt[] = {{1,1}, {1,2}, {1,3}, {2,3}, {3,3}, {4,3},
    {4,2}, {4,1}, {3,1}, {2,1}};*/
  point_t cpt[] = {{1,1}, {1,2}, {1,3}, {1,4}, {2,4}, {2,5},
    {3,5}, {4,5}, {5,5}, {5,4}, {6,5}};
  point_t *pt = cpt;

  path_t *p = path_new();
  privpath_t *pp = p->priv;
  pp->pt = pt;
  pp->len = sizeof(cpt)/sizeof(cpt[0]);
  pp->closed = 0;
  p->area = 100; // no speckle
  p->sign = '+';

  const potrace_param_t *param = potrace_param_default();
  progress_t prog;
  prog.callback = param->progress.callback;
  prog.data = param->progress.data;
  prog.min = param->progress.min;
  prog.max = param->progress.max;
  prog.epsilon = param->progress.epsilon;
  prog.d_prev = param->progress.min;

  #define TRY(x) if (x) goto try_error
  TRY(calc_sums(p->priv));
  TRY(calc_lon(p->priv));
  TRY(bestpolygon(p->priv));
  TRY(adjust_vertices(p->priv));
  TRY(smooth(&p->priv->curve, p->sign, param->alphamax));
  TRY(opticurve(p->priv, param->opttolerance));

  return 0;

try_error:
  printf("process_path failed with errno %d", errno);
  return 1;
}
