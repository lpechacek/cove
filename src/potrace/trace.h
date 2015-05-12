/* Copyright (C) 2001-2005 Peter Selinger.
   This file is part of potrace. It is free software and it is covered
   by the GNU General Public License. See the file COPYING for details. */

/* $Id: trace.h,v 1.1 2006/01/19 13:15:09 farco Exp $ */

#ifndef TRACE_H
#define TRACE_H

#include "potracelib.h"
#include "progress.h"

int process_path(path_t *plist, const potrace_param_t *param, progress_t *progress);
int calc_sums(privpath_t *pp);
int calc_lon(privpath_t *pp);
int bestpolygon(privpath_t *pp);
int adjust_vertices(privpath_t *pp);
int smooth(privcurve_t *curve, int sign, double alphamax);
int opticurve(privpath_t *pp, double opttolerance);

#endif /* TRACE_H */
