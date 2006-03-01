/** \file
 * \ingroup std
 * \author Cannon, K. C.
 * \brief XLAL routines for computing the sidereal time.
 */


#include <math.h>
#include <lal/Date.h>


/*
 * Returns the Greenwich Sidereal Time IN RADIANS corresponding to a
 * specified GPS time.  Aparent sidereal time is computed by providing the
 * equation of equinoxes in units of seconds.  For mean sidereal time, set
 * this parameter to 0.
 *
 * The output of this code is in radians in the range [0,2pi).
 *
 * Inspired by the function sidereal_time() in the NOVAS-C library, version
 * 2.0.1, which is dated December 10th, 1999, and carries the following
 * references:
 *
 * Aoki, et al. (1982) Astronomy and Astrophysics 105, 359-361.
 * Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 *   Subroutines"; USNO internal document dated 20 Oct 1988;
 *   revised 15 Mar 1990.
 *
 * See http://aa.usno.navy.mil/software/novas for more information.
 *
 * Note:  rather than maintaining this code separately, it would be a good
 * idea for LAL to simply link to the NOVAS-C library directly.  Something
 * to do when we have some spare time.
 */

REAL8 XLALGreenwichSiderealTime(
	const LIGOTimeGPS *gpstime,
	REAL8 equation_of_equinoxes
)
{
	static const char *func = "XLALGreenwichSiderealTime";
	struct tm utc;
	double julian_day;
	double t_hi, t_lo;
	double t, t_squared, t_cubed;
	double sidereal_time;

	/*
	 * Convert GPS seconds to UTC.  This is where we pick up knowledge
	 * of leap seconds which are required for the mapping of atomic
	 * time scales to celestial time scales.  We deal only with integer
	 * seconds.
	 */

	XLALGPSToUTC(&utc, gpstime->gpsSeconds);

	/*
	 * And now to Julian day number.  Again, only accurate to integer
	 * seconds.
	 */

	julian_day = XLALJulianDay(&utc);

	/*
	 * Convert Julian day number to the number of centuries since the
	 * Julian epoch (1 century = 36525.0 days).  Here, we incorporate
	 * the fractional part of the seconds.  For precision, we keep
	 * track of the most significant and least significant parts of the
	 * time separately.  The original code in NOVAS-C determined t_hi
	 * and t_lo from Julian days, with t_hi receiving the integer part
	 * and t_lo the fractional part.  Because LAL's Julian day routine
	 * is accurate to the second, here the hi/lo split is most
	 * naturally done at the integer seconds boundary.  Note that the
	 * "hi" and "lo" components have the same units and so the split
	 * can be done anywhere.
	 */

	t_hi = (julian_day - XLAL_EPOCH_J2000_0_JD) / 36525.0;
	t_lo = gpstime->gpsNanoSeconds / (1e9 * 36525.0 * 86400.0);

	/*
	 * Compute sidereal time in seconds.  (magic)
	 */

	t = t_hi + t_lo;
	t_squared = t * t;
	t_cubed = t_squared * t;

	sidereal_time = equation_of_equinoxes - 6.2e-6 * t_cubed + 0.093104 * t_squared + 67310.54841
		+ 8640184.812866 * t_lo
		+ 3155760000.0 * t_lo
		+ 8640184.812866 * t_hi
		+ 3155760000.0 * t_hi;

	/*
	 * Convert from seconds to fraction of day.
	 */

	sidereal_time = fmod(sidereal_time / 86400.0, 1.0);
	if(sidereal_time < 0.0)
		sidereal_time += 1.0;
  
	/*
	 * Return radians
	 */

	return sidereal_time * 2.0 * LAL_PI;
}


REAL8 XLALGreenwichMeanSiderealTime(
	const LIGOTimeGPS *gpstime
)
{
	return XLALGreenwichSiderealTime(gpstime, 0.0);
}
