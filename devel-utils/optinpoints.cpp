#include <exception>
#include <iostream>
#include <sys/times.h>
#include <stdlib.h>
#include <math.h>
#include "Polygons.H"

extern "C"
{
#include "auxiliary.h"
#include "lists.h"
}

int main(void)
{
  try {
  perfcheck();
  } catch (std::exception e) {
    std::cout << "exception caught:" << e.what() << std::endl;
  }
}

void perfcheck(void)
{
  path_t *plist = NULL;		/* linked list of path objects */
  path_t **hook = &plist;	/* used to speed up appending to linked list */

  Polygons::JOINENDPOINTLIST pointlist;
  dpoint_t min = {INFINITY, INFINITY}, max = {-INFINITY, -INFINITY};
  path_t* p;

  printf("generating points\n");
  for(int i = 0; i < 400000; i++)
  {
    p = path_new ();
    if(!p)
    {
      perror("path_new");
      exit(1);
    }
    privpath_t *pp = p->priv;
    /* pp->pt = pt;
       pp->len = len;
       pp->closed = pathsiterator->isClosed ();
       p->area = 100;		// no speckle
       p->sign = '+'; */
    if(privcurve_init (&pp->curve, 2))
    {
      perror("path_new");
      exit(1);
    }
    privcurve_t* curve = &pp->curve;

    dpoint_t f, b;
    f.x = random()*10000/RAND_MAX;
    f.y = random()*10000/RAND_MAX;
    b.x = random()*10000/RAND_MAX;
    b.y = random()*10000/RAND_MAX;
    pointlist.push_back (Polygons::JOINENDPOINT (f, Polygons::FRONT, p));
    pointlist.push_back (Polygons::JOINENDPOINT (b, Polygons::BACK, p));
    if (f.x > max.x)
      max.x = f.x;
    if (f.x < min.x)
      min.x = f.x;
    if (f.y > max.y)
      max.y = f.y;
    if (f.y < min.y)
      min.y = f.y;
    if (b.x > max.x)
      max.x = b.x;
    if (b.x < min.x)
      min.x = b.x;
    if (b.y > max.y)
      max.y = b.y;
    if (b.y < min.y)
      min.y = b.y;

    curve->vertex[0] = f;
    curve->vertex[1] = b;

    list_insert_beforehook (p, hook);
  }
  printf("done generating points\n");

  Polygons::JOINOPLIST ops;
  Polygons poll;
  struct tms before, after;

  for(int npoint_max = 10; npoint_max < 90; npoint_max += 5)
  {
    poll.NPOINTS_MAX = npoint_max;
    if(times(&before) == -1)
    {
      perror("times before");
      exit(1);
    }
    poll.compdists (pointlist, ops, min, max, true, 0, 50, 12);
    if(times(&after) == -1)
    {
      perror("times after");
      exit(1);
    }
    ops.clear();
    printf("NPOINTS_MAX = %d; time = %lu\n", npoint_max, after.tms_utime-before.tms_utime);
  }
}

