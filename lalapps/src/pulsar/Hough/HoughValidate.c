/*
*  Copyright (C) 2007 Badri Krishnan, Reinhard Prix
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/**
 * \file
 * \ingroup lalapps_pulsar_Hough
 * \author A.M. Sintes, B. Krishnan
 * \brief
 * Monte Carlo signal injections for several h_0 values and
 * compute the Hough transform for only one point in parameter space each time
 */

/* 
 The idea is that we would like to analize a 300 Hz band on a cluster of
 machines. Each process should analyze 1 Hz band  (or whatever).
 
 	- Read the  band to be analized and the wings needed to read the originals SFTs. 
	-Read the h_0 values to be analyzed in one go
	- Read the file  containing the times and velocities generated by
	DriveHoughColor or compute them 
	-loop over the MC injections:
		+ Generate random parameters (f, f', alpha, delata, i...)
		+ generate h(t), produce its FFT
		+ Add h(f) to SFT for a given h_o value (and all of them)
		+ get number count
		+ wite to file
	(note if one loop fails, should print error , but continue with the next
	value)
	
Input shoud be from
             SFT files 
	     band, wings, nh_0, h_01, h_02....
	     ephemeris info
             (it should also read the times and velocities used in
	     DriveHoughColor)
	     
   This code will output files containing the MC results and info about injected
   signals. 
*/



#include "./MCInjectComputeHough.h" /* proper path*/


#define VALIDATEOUT "./outHM1/skypatch_1/HM1validate"
#define VALIDATEIN  "./outHM1/skypatch_1/HM1templates"

#define TRUE (1==1)
#define FALSE (1==0)

/*************************************/

int main(int argc, char *argv[]){

  static LALStatus  status;  

  static LALDetector          *detector;
  static LIGOTimeGPSVector    timeV;
  static REAL8Cart3CoorVector velV;
  static REAL8Vector          timeDiffV;
  static REAL8                foft;
  static HoughPulsarTemplate  pulsarTemplate;

  EphemerisData   *edat = NULL;
  CHAR  *uvar_earthEphemeris = NULL; 
  CHAR  *uvar_sunEphemeris = NULL;
  SFTVector  *inputSFTs  = NULL;  
  REAL8 *alphaVec=NULL;
  REAL8 *deltaVec=NULL;
  REAL8 *freqVec=NULL;
  REAL8 *spndnVec=NULL;

  /* pgV is vector of peakgrams and pg1 is onepeakgram */
  static UCHARPeakGram    *pg1, **pgV; 
  UINT4  msp; /*number of spin-down parameters */
  CHAR   *uvar_ifo = NULL;
  CHAR   *uvar_sftDir = NULL; /* the directory where the SFT  could be */
  CHAR   *uvar_fnameOut = NULL;               /* The output prefix filename */
  CHAR   *uvar_fnameIn = NULL;  
  INT4   numberCount, ind;
  UINT8  nTemplates;   
  UINT4   mObsCoh;
  REAL8  uvar_peakThreshold;
  REAL8  f_min, f_max, fWings, timeBase;
  INT4  uvar_blocksRngMed;
  UINT4  sftlength; 
  INT4   sftFminBin;
  UINT4 loopId, tempLoopId;
  FILE  *fpOut = NULL;
  CHAR *fnameLog=NULL;
  FILE *fpLog = NULL;
  CHAR *logstr=NULL;
  /*REAL8 asq, bsq;*/ /* square of amplitude modulation functions a and b */

  /******************************************************************/
  /*    Set up the default parameters.      */
  /* ****************************************************************/

  /* LAL error-handler */
  lal_errhandler = LAL_ERR_EXIT;
  
  msp = 1; /*only one spin-down */
 
  /* memory allocation for spindown */
  pulsarTemplate.spindown.length = msp;
  pulsarTemplate.spindown.data = NULL;
  pulsarTemplate.spindown.data = (REAL8 *)LALMalloc(msp*sizeof(REAL8));
 
 
  uvar_peakThreshold = THRESHOLD;

  uvar_earthEphemeris = (CHAR *)LALMalloc(1024*sizeof(CHAR));
  strcpy(uvar_earthEphemeris,EARTHEPHEMERIS);

  uvar_sunEphemeris = (CHAR *)LALMalloc(1024*sizeof(CHAR));
  strcpy(uvar_sunEphemeris,SUNEPHEMERIS);

  uvar_sftDir = (CHAR *)LALMalloc(1024*sizeof(CHAR));
  strcpy(uvar_sftDir,SFTDIRECTORY);

  uvar_fnameOut = (CHAR *)LALMalloc(1024*sizeof(CHAR));
  strcpy(uvar_fnameOut,VALIDATEOUT);

  uvar_fnameIn = (CHAR *)LALMalloc(1024*sizeof(CHAR));
  strcpy(uvar_fnameIn,VALIDATEIN);

  uvar_blocksRngMed = BLOCKSRNGMED;

  /* register user input variables */
  LAL_CALL( LALRegisterSTRINGUserVar( &status, "ifo", 'i', UVAR_OPTIONAL, "Detector GEO(1) LLO(2) LHO(3)", &uvar_ifo ), &status);
  LAL_CALL( LALRegisterSTRINGUserVar( &status, "earthEphemeris", 'E', UVAR_OPTIONAL, "Earth Ephemeris file", &uvar_earthEphemeris), &status);
  LAL_CALL( LALRegisterSTRINGUserVar( &status, "sunEphemeris", 'S', UVAR_OPTIONAL, "Sun Ephemeris file", &uvar_sunEphemeris), &status);
  LAL_CALL( LALRegisterSTRINGUserVar( &status, "sftDir", 'D', UVAR_OPTIONAL, "SFT Directory", &uvar_sftDir), &status);
  LAL_CALL( LALRegisterSTRINGUserVar( &status, "fnameIn", 'T', UVAR_OPTIONAL, "Input template file", &uvar_fnameIn), &status);
  LAL_CALL( LALRegisterSTRINGUserVar( &status, "fnameOut", 'o', UVAR_OPTIONAL, "Output filename", &uvar_fnameOut), &status);
  LAL_CALL( LALRegisterINTUserVar( &status, "blocksRngMed", 'w', UVAR_OPTIONAL, "RngMed block size", &uvar_blocksRngMed), &status);
  LAL_CALL( LALRegisterREALUserVar( &status, "peakThreshold", 't', UVAR_OPTIONAL, "Peak selection threshold", &uvar_peakThreshold), &status);

  /* read all command line variables */
  BOOLEAN should_exit = 0;
  LAL_CALL( LALUserVarReadAllInput(&status, &should_exit, argc, argv), &status);
  if (should_exit)
    exit(1);
 
  /* open log file for writing */
  fnameLog = LALCalloc( (strlen(uvar_fnameOut) + strlen(".log") + 10),1);
  strcpy(fnameLog,uvar_fnameOut);
  strcat(fnameLog,".log");
  if ((fpLog = fopen(fnameLog, "w")) == NULL) {
    fprintf(stderr, "Unable to open file %s for writing\n", fnameLog);
    LALFree(fnameLog);
    exit(1);
  }
  
  /* get the log string */
  LAL_CALL( LALUserVarGetLog(&status, &logstr, UVAR_LOGFMT_CFGFILE), &status);  

  fprintf( fpLog, "## Log file for HoughValidate\n\n");
  fprintf( fpLog, "# User Input:\n");
  fprintf( fpLog, "#-------------------------------------------\n");
  fprintf( fpLog, "%s", logstr);
  LALFree(logstr);

  /* append an ident-string defining the exact CVS-version of the code used */
  {
    CHAR command[1024] = "";
    fprintf (fpLog, "\n\n# CVS-versions of executable:\n");
    fprintf (fpLog, "# -----------------------------------------\n");
    fclose (fpLog);
    
    sprintf (command, "ident %s | sort -u >> %s", argv[0], fnameLog);
    /* we don't check this. If it fails, we assume that */
    /* one of the system-commands was not available, and */
    /* therefore the CVS-versions will not be logged */
    if ( system(command) ) fprintf (stderr, "\nsystem('%s') returned non-zero status!\n\n", command );

    LALFree(fnameLog);
  }

  /* open output file for writing */
  fpOut= fopen(uvar_fnameOut, "w");
  /*setlinebuf(fpOut);*/  /* line buffered on */  
  setvbuf(fpOut, (char *)NULL, _IOLBF, 0);

  /*****************************************************************/
  /* read template file */
  /*****************************************************************/
  {
    FILE  *fpIn = NULL;
    INT4   r;
    REAL8  temp1, temp2, temp3, temp4, temp5;
    UINT8  templateCounter; 
    
    fpIn = fopen(uvar_fnameIn, "r");
    if ( !fpIn )
      {
	fprintf(stderr, "Unable to fine file %s\n", uvar_fnameIn);
	return DRIVEHOUGHCOLOR_EFILE;
      }


    nTemplates = 0;
    do 
      {
	r=fscanf(fpIn,"%lf%lf%lf%lf%lf\n", &temp1, &temp2, &temp3, &temp4, &temp5);
	/* make sure the line has the right number of entries or is EOF */
	if (r==5) nTemplates++;
      } while ( r != EOF);
    rewind(fpIn);
    
    alphaVec = (REAL8 *)LALMalloc(nTemplates*sizeof(REAL8));
    deltaVec = (REAL8 *)LALMalloc(nTemplates*sizeof(REAL8));     
    freqVec = (REAL8 *)LALMalloc(nTemplates*sizeof(REAL8));
    spndnVec = (REAL8 *)LALMalloc(nTemplates*sizeof(REAL8));     
    
    
    for (templateCounter = 0; templateCounter < nTemplates; templateCounter++)
      {
	r=fscanf(fpIn,"%lf%lf%lf%lf%lf\n", &temp1, alphaVec + templateCounter, deltaVec + templateCounter, 
		 freqVec + templateCounter,  spndnVec + templateCounter);
      }     
    fclose(fpIn);      
  }


  /**************************************************/
  /* read sfts */     
  /*************************************************/
  f_min = freqVec[0];     /* initial frequency to be analyzed */
  /* assume that the last frequency in the templates file is also the highest frequency */
  f_max = freqVec[nTemplates-1] ; 
  
  /* we need to add wings to fmin and fmax to account for 
     the Doppler shift, the size of the rngmed block size
     and also nfsizecylinder.  The block size and nfsizecylinder are
     specified in terms of frequency bins...this goes as one of the arguments of 
     LALReadSFTfiles */
  /* first correct for Doppler shift */
  fWings =  f_max * VTOT; 
  f_min -= fWings;    
  f_max += fWings; 
  
  /* create pattern to look for in SFT directory */   

  {
    CHAR *tempDir = NULL;
    SFTCatalog *catalog = NULL;
    static SFTConstraints constraints;

    /* set detector constraint */
    constraints.detector = NULL;
    if ( LALUserVarWasSet( &uvar_ifo ) )    
      constraints.detector = XLALGetChannelPrefix ( uvar_ifo );

    /* get sft catalog */
    tempDir = (CHAR *)LALCalloc(512, sizeof(CHAR));
    strcpy(tempDir, uvar_sftDir);
    strcat(tempDir, "/*SFT*.*");
    LAL_CALL( LALSFTdataFind( &status, &catalog, tempDir, &constraints), &status);
    
    detector = XLALGetSiteInfo( catalog->data[0].header.name);

    mObsCoh = catalog->length;
    timeBase = 1.0 / catalog->data->header.deltaF;

    LAL_CALL( LALLoadSFTs ( &status, &inputSFTs, catalog, f_min, f_max), &status);

    LAL_CALL( LALNormalizeSFTVect (&status, inputSFTs, uvar_blocksRngMed), &status);

    if ( LALUserVarWasSet( &uvar_ifo ) )    
      LALFree( constraints.detector );
    LALFree( tempDir);
    LAL_CALL( LALDestroySFTCatalog( &status, &catalog ), &status);  	

  }



  sftlength = inputSFTs->data->data->length;
  {
    INT4 tempFbin;
    sftFminBin = floor( (REAL4)(timeBase * inputSFTs->data->f0) + (REAL4)(0.5));
    tempFbin = floor( timeBase * inputSFTs->data->f0 + 0.5);

    if (tempFbin - sftFminBin)
      {
	fprintf(stderr, "Rounding error in calculating fminbin....be careful! \n");
      }
  }

  /* loop over sfts and select peaks */

  /* first the memory allocation for the peakgramvector */
  pgV = NULL;
  pgV = (UCHARPeakGram **)LALMalloc(mObsCoh*sizeof(UCHARPeakGram *));  

  /* memory for  peakgrams */
  for (tempLoopId=0; tempLoopId < mObsCoh; tempLoopId++)
    {
      pgV[tempLoopId] = (UCHARPeakGram *)LALMalloc(sizeof(UCHARPeakGram));
      pgV[tempLoopId]->length = sftlength; 
      pgV[tempLoopId]->data = NULL; 
      pgV[tempLoopId]->data = (UCHAR *)LALMalloc(sftlength* sizeof(UCHAR));
   
      LAL_CALL (SFTtoUCHARPeakGram( &status, pgV[tempLoopId], inputSFTs->data + tempLoopId, uvar_peakThreshold), &status);
    }


  /* having calculated the peakgrams we don't need the sfts anymore */
  LAL_CALL(LALDestroySFTVector(&status, &inputSFTs),&status );


  
  /* ****************************************************************/
  /* setting timestamps vector */
  /* ****************************************************************/
  timeV.length = mObsCoh;
  timeV.data = NULL;  
  timeV.data = (LIGOTimeGPS *)LALMalloc(mObsCoh*sizeof(LIGOTimeGPS));
  
  { 
    UINT4    j; 
    for (j=0; j < mObsCoh; j++){
      timeV.data[j].gpsSeconds = pgV[j]->epoch.gpsSeconds;
      timeV.data[j].gpsNanoSeconds = pgV[j]->epoch.gpsNanoSeconds;
    }    
  }
  
  /******************************************************************/
  /* compute the time difference relative to startTime for all SFTs */
  /******************************************************************/
  timeDiffV.length = mObsCoh;
  timeDiffV.data = NULL; 
  timeDiffV.data = (REAL8 *)LALMalloc(mObsCoh*sizeof(REAL8));
  
  {   
    REAL8   t0, ts, tn, midTimeBase;
    UINT4   j; 

    midTimeBase=0.5*timeBase;
    ts = timeV.data[0].gpsSeconds;
    tn = timeV.data[0].gpsNanoSeconds * 1.00E-9;
    t0=ts+tn;
    timeDiffV.data[0] = midTimeBase;

    for (j=1; j< mObsCoh; ++j){
      ts = timeV.data[j].gpsSeconds;
      tn = timeV.data[j].gpsNanoSeconds * 1.00E-9;  
      timeDiffV.data[j] = ts + tn -t0 + midTimeBase; 
    }  
  }

  /******************************************************************/ 
  /*   setting of ephemeris info */ 
  /******************************************************************/ 
  edat = (EphemerisData *)LALMalloc(sizeof(EphemerisData));
  (*edat).ephiles.earthEphemeris = uvar_earthEphemeris;
  (*edat).ephiles.sunEphemeris = uvar_sunEphemeris;
  
  /******************************************************************/
  /* compute detector velocity for those time stamps                */
  /******************************************************************/
  velV.length = mObsCoh;
  velV.data = NULL;
  velV.data = (REAL8Cart3Coor *)LALMalloc(mObsCoh*sizeof(REAL8Cart3Coor));
  
  {  
    VelocityPar   velPar;
    REAL8     vel[3]; 
    UINT4     j; 

    velPar.detector = *detector;
    velPar.tBase = timeBase;
    velPar.vTol = ACCURACY;
    velPar.edat = NULL;

    /* read in ephemeris data */
    LAL_CALL( LALInitBarycenter( &status, edat), &status);
    velPar.edat = edat;

    /* now calculate average velocity */    
    for(j=0; j< velV.length; ++j){
      velPar.startTime.gpsSeconds     = timeV.data[j].gpsSeconds;
      velPar.startTime.gpsNanoSeconds = timeV.data[j].gpsNanoSeconds;
      
      LAL_CALL( LALAvgDetectorVel ( &status, vel, &velPar), &status );
      velV.data[j].x= vel[0];
      velV.data[j].y= vel[1];
      velV.data[j].z= vel[2];   
    }  
  }


  
  /* amplitude modulation stuff */
  {
    AMCoeffs amc; 
    AMCoeffsParams *amParams;
    EarthState earth;
    BarycenterInput baryinput;  /* Stores detector location and other barycentering data */

    /* detector location */
    baryinput.site.location[0] = detector->location[0]/LAL_C_SI;
    baryinput.site.location[1] = detector->location[1]/LAL_C_SI;
    baryinput.site.location[2] = detector->location[2]/LAL_C_SI;
    baryinput.dInv = 0.e0;
    /* alpha and delta must come from the skypatch */
    /* for now set it to something arbitrary */
    baryinput.alpha = 0.0;
    baryinput.delta = 0.0;

    /* Allocate space for amParams stucture */
    /* Here, amParams->das is the Detector and Source info */
    amParams = (AMCoeffsParams *)LALMalloc(sizeof(AMCoeffsParams));
    amParams->das = (LALDetAndSource *)LALMalloc(sizeof(LALDetAndSource));
    amParams->das->pSource = (LALSource *)LALMalloc(sizeof(LALSource));
    /* Fill up AMCoeffsParams structure */
    amParams->baryinput = &baryinput;
    amParams->earth = &earth; 
    amParams->edat = edat;
    amParams->das->pDetector = detector; 
    /* make sure alpha and delta are correct */
    amParams->das->pSource->equatorialCoords.latitude = baryinput.delta;
    amParams->das->pSource->equatorialCoords.longitude = baryinput.alpha;
    amParams->das->pSource->orientation = 0.0;
    amParams->das->pSource->equatorialCoords.system = COORDINATESYSTEM_EQUATORIAL;
    amParams->polAngle = amParams->das->pSource->orientation ; /* These two have to be the same!!*/

    /* timeV is start time ---> change to mid time */    
    LAL_CALL (LALComputeAM(&status, &amc, timeV.data, amParams), &status); 

    /* calculate a^2 and b^2 */
    /* for (ii=0, asq=0.0, bsq=0.0; ii<mObsCoh; ii++) */
    /*       { */
    /* 	REAL8 *a, *b; */
    /*        	a = amc.a + ii; */
    /* 	b = amc.b + ii; */
    /* 	asq += (*a) * (*a); */
    /* 	bsq += (*b) * (*b); */
    /*       } */
    
    /* free amParams */
    LALFree(amParams->das->pSource);
    LALFree(amParams->das);
    LALFree(amParams);

  }

  /* loop over templates */    
  for(loopId=0; loopId < nTemplates; ++loopId){
    
    /* set template parameters */    
    pulsarTemplate.f0 = freqVec[loopId];
    pulsarTemplate.longitude = alphaVec[loopId];
    pulsarTemplate.latitude = deltaVec[loopId];
    pulsarTemplate.spindown.data[0] = spndnVec[loopId];
	   
 
    {
      REAL8   f0new, vcProdn, timeDiffN;
      REAL8   sourceDelta, sourceAlpha, cosDelta, factorialN;
      UINT4   j, i, f0newBin; 
      REAL8Cart3Coor       sourceLocation;
      
      sourceDelta = pulsarTemplate.latitude;
      sourceAlpha = pulsarTemplate.longitude;
      cosDelta = cos(sourceDelta);
      
      sourceLocation.x = cosDelta* cos(sourceAlpha);
      sourceLocation.y = cosDelta* sin(sourceAlpha);
      sourceLocation.z = sin(sourceDelta);      

      /* loop for all different time stamps,calculate frequency and produce number count*/ 
      /* first initialize number count */
      numberCount=0;
      for (j=0; j<mObsCoh; ++j)
	{  
	  /* calculate v/c.n */
	  vcProdn = velV.data[j].x * sourceLocation.x
	    + velV.data[j].y * sourceLocation.y
	    + velV.data[j].z * sourceLocation.z;

	  /* loop over spindown values to find f_0 */
	  f0new = pulsarTemplate.f0;
	  factorialN = 1.0;
	  timeDiffN = timeDiffV.data[j];	  
	  for (i=0; i<msp;++i)
	    { 
	      factorialN *=(i+1.0);
	      f0new += pulsarTemplate.spindown.data[i]*timeDiffN / factorialN;
	      timeDiffN *= timeDiffN;
	    }
	  
	  f0newBin = floor(f0new*timeBase + 0.5);
	  foft = f0newBin * (1.0 +vcProdn) / timeBase;    
	  
	  /* get the right peakgram */      
	  pg1 = pgV[j];
	  
	  /* calculate frequency bin for template */
	  ind =  floor( foft * timeBase + 0.5 ) - sftFminBin; 
	  
	  /* update the number count */
	  numberCount+=pg1->data[ind]; 
	}      
      
    } /* end of block calculating frequency path and number count */      
    

    /******************************************************************/
    /* printing result in the output file */
    /******************************************************************/
    
    fprintf(fpOut,"%d %f %f %f %g \n", 
	    numberCount, pulsarTemplate.longitude, pulsarTemplate.latitude, pulsarTemplate.f0,
	    pulsarTemplate.spindown.data[0] );
    
  } /* end of loop over templates */

  /******************************************************************/
  /* Closing files */
  /******************************************************************/  
  fclose(fpOut); 

  
  /******************************************************************/
  /* Free memory and exit */
  /******************************************************************/

  LALFree(alphaVec);
  LALFree(deltaVec);
  LALFree(freqVec);
  LALFree(spndnVec);
  

  for (tempLoopId = 0; tempLoopId < mObsCoh; tempLoopId++){
    pg1 = pgV[tempLoopId];  
    LALFree(pg1->data);
    LALFree(pg1);
  }
  LALFree(pgV);
  
  LALFree(timeV.data);
  LALFree(timeDiffV.data);
  LALFree(velV.data);
  
  LALFree(pulsarTemplate.spindown.data);
   
  LALFree(edat->ephemE);
  LALFree(edat->ephemS);
  LALFree(edat);
  
  LAL_CALL (LALDestroyUserVars(&status), &status);

  LALCheckMemoryLeaks();
    
  return DRIVEHOUGHCOLOR_ENORM;
}


 

