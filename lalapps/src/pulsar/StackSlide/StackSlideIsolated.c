/*********************************** <lalVerbatim file="StackSlideIsolatedCV">
Authors: Mendell, Gregory
$Id$
************************************ </lalVerbatim> */

/* REVISIONS: */
/* 04/12/05 gam; Move from using StackSlideOld to using StackSlide function. */
/* 04/12/05 gam; add RunStackSlideIsolatedMonteCarloSimulation to StackSlideIsolated.c. Previous REVISIONS: */
/* 05/11/04 gam; Add RunStackSlideMonteCarloSimulation to software inject signals into the SFTs for Monte Carlo simulations */
/* 05/21/04 gam; Normalize the SFTs from LALSignalToSFTs as per the normalization in makefakedata_v2.c and lalapps/src/pulsar/make_sfts.c. */
/* 05/21/04 gam; Save the input SFTs and Parameter space; run Monte Carlo simulation on the parameter space with random offsets */
/* 05/26/04 gam; Continue work on Monto Carlo code. */
/* 05/26/04 gam; Change finishedSUMs to finishedSUMs; add startSUMs; defaults are TRUE; use to control I/O during Monte Carlo */
/* 05/26/04 gam; Add whichMCSUM = which Monte Carlo SUM; default is -1. */
/* 05/28/04 gam; Use LALUniformDeviate from LAL utilities package (include <lal/Random.h>) to generate random mismatch during Monte Carlo. */
/* 06/01/04 gam; Make sure CHECKSTATUSPTR called after every LAL function call */
/* 06/01/04 gam; pPulsarSignalParams->startTimeGPS and duration should be based on gpsStartTime and duration, not actualStartTime */
/* 06/01/04 gam; For now fix DeltaRA, DeltaDec,and DeltaFDeriv1 to default values when finding mismatch during Monte Carlo */
/* 07/14/04 gam; add option to use function LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs; see LAL inject packge GeneratePulsarSignal.c and .h */
/* 07/19/04 gam; if (params->testFlag & 4) > 0 use LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs for Monte Carlo simulations */
/* 08/02/04 gam; if (params->testFlag & 4) > 0 ComputeSky uses reference time GPSin: params->timeStamps[0].gpsSeconds, params->timeStamps[0].gpsNanoSeconds */
/* 08/02/04 gam; Set pSFTandSignalParams->resTrig = 0 to avoid serious bug in LALFastGeneratePulsarSFTs when using lookup tables (LUTs) for trig calls. */
/* 12/06/04 gam; get params->sampleRate, = effective sample rate, from the SFTs; calculate params->deltaT after reading SFTs. */
/* 12/06/04 gam; add params->gpsEpochStartTimeNan; get gpsEpochStartTime, gpsEpochStartTimeNan, and gpsStartTime from command line; */ 
/* 12/06/04 gam; if (params->testFlag & 8) > 0 use fixed values for psi and cosIota during Monte Carlo simulations */
/* 05/06/05 gam; If params->debugOptionFlag & 128 > 0 and isolated case, just creates a SUM from the STKs without sliding */
/* 05/19/05 gam; Add INT4 *sumBinMask; params->sumBinMask == 0 if bin should be excluded from search or Monte Carlo due to cleaning */
/* 05/24/05 gam; make maxPower and totalEventCount part of params; change finishSUMs to finishPeriodicTable; end xml in FinalizeSearch */
/* 05/24/05 gam; if (params->testFlag & 16 > 0) use results from prior jobs in the pipeline and report on current MC results */
/* 05/24/05 gam; if (params->testFlag & 32 > 0) iterate MC up to 10 times to converge on desired confidence */
/* 05/24/05 gam; if (params->debugOptionFlag & 32 > 0) print Monte Carlo Simulation results to stdout */
/* 05/24/05 gam; add StackSlideMonteCarloResultsTable */

/*********************************************/
/*                                           */
/* START SECTION: define preprocessor flags  */
/*                                           */
/*********************************************/
#define INCLUDE_PRINT_PEAKS_TABLE_CODE
#define INCLUDE_PRINT_REPORTMCRESULTS_CODE
/* #define DEBUG_STACKSLIDE_ISOLATED */
/* #define DEBUG_SUM_TEMPLATEPARAMS */
/* #define PRINT_STACKSLIDE_BINOFFSETS */
/* #define PRINT_STACKSLIDE_BINMISMATCH */
/* #define PRINT_STACKSLIDE_SPECIAL_DEBUGGING_INFO */
#define INCLUDE_RUNSTACKSLIDEISOLATEDMONTECARLO_CODE
/* #define DEBUG_MONTECARLOTIMEDOMAIN_DATA */
/* #define PRINT_MONTECARLOTIMEDOMAIN_DATA */
/* #define DEBUG_MONTECARLOSFT_DATA */
/* #define PRINT_MONTECARLOSFT_DATA */
/* #define PRINTCOMPARISON_INPUTVSMONTECARLOSFT_DATA */
/* #define DEBUG_RANDOMTRIALPARAMETERS */
/* #define DEBUG_LALFASTGENERATEPULSARSFTS */
/* #define DEBUG_SETFIXED_RANDVAL */
/* #define PRINT_ONEMONTECARLO_OUTPUTSFT */
/* #define PRINT_MAXPOWERANDBINEACHSFT */
#define PRINT_SLIDINGTURNEDOFF_WARNING
/*********************************************/
/*                                           */
/* END SECTION: define preprocessor flags    */
/*                                           */
/*********************************************/
/*********************************************/
/*                                           */
/* START SECTION: include header files       */
/* (Note that most include statements        */
/*  are in the header files below.)          */
/*                                           */
/*********************************************/
#include "StackSlideIsolated.h"
/*********************************************/
/*                                           */
/* END SECTION: include header files         */
/*                                           */
/*********************************************/

NRCSID( STACKSLIDEISOLATEDC,  "$Id$");

/*********************************************************************************/
/*              START FUNCTION: StackSlideIsolated                               */
/*********************************************************************************/
void StackSlideIsolated (
    LALStatus                        *status,
    SnglStackSlidePeriodicTable      *loudestPeaksArray,
    LALFindStackSlidePeakOutputs     *pLALFindStackSlidePeakOutputs,
    LALFindStackSlidePeakParams      *pLALFindStackSlidePeakParams,
    LALUpdateLoudestStackSlideParams *pLALUpdateLoudestStackSlideParams,
    LALDetector                      *cachedDetector,
    StackSlideParams                 *stksldParams,
    StackSlideSearchParams           *params
)
{
 INT4 iSky, iFreqDeriv, k;
 INT4 kSUM = -1;
  
 SnglStackSlidePeriodicTable *peakSav;
  
 /* 04/12/05 gam; add parameters for StackSlideComputeSky and StackSlide */
 StackSlideSkyParams *csParams;
 TdotsAndDeltaTs *pTdotsAndDeltaTs;
 EarthState earth;
 EmissionTime emit;
  
 #ifdef DEBUG_STACKSLIDE_ISOLATED
   fprintf(stdout, "\nStart StackSlideIsolated\n\n");
   fflush(stdout);
 #endif    

 INITSTATUS( status, "StackSlideIsolated", STACKSLIDEISOLATEDC );
 ATTATCHSTATUSPTR(status);
  
 /* 04/12/05 gam; allocate memory and initialize parameters for StackSlideComputeSky and StackSlide */
 csParams=(StackSlideSkyParams *)LALMalloc(sizeof(StackSlideSkyParams));
 csParams->skyPos=(REAL8 *)LALMalloc(2*sizeof(REAL8));
 /* 12/05/05 gam; IMPORTANT, must set these to the epoch that gives T0 at SSB; this differs when running Monte Carlo */
 if ( (params->testFlag & 4) > 0 ) {
      stksldParams->gpsStartTimeSec = (UINT4)params->timeStamps[0].gpsSeconds;
      stksldParams->gpsStartTimeNan = (UINT4)params->timeStamps[0].gpsNanoSeconds;
      csParams->gpsStartTimeSec     = (UINT4)params->timeStamps[0].gpsSeconds;
      csParams->gpsStartTimeNan     = (UINT4)params->timeStamps[0].gpsNanoSeconds;
 } else {
      stksldParams->gpsStartTimeSec = params->gpsEpochStartTimeSec;
      stksldParams->gpsStartTimeNan = params->gpsEpochStartTimeNan;
      csParams->gpsStartTimeSec     = params->gpsEpochStartTimeSec;
      csParams->gpsStartTimeNan     = params->gpsEpochStartTimeNan;
 }
 csParams->tGPS=params->timeStamps;
 csParams->spinDwnOrder=params->numSpinDown;
 csParams->mObsSFT=params->numSTKs;
 csParams->tSFT=params->tSTK;
 csParams->edat=params->edat;
 csParams->emit=&emit;
 csParams->earth=&earth;
 csParams->baryinput = &params->baryinput;
 pTdotsAndDeltaTs=(TdotsAndDeltaTs *)LALMalloc(sizeof(TdotsAndDeltaTs));
 pTdotsAndDeltaTs->vecTDots = (REAL8 *)LALMalloc(sizeof(REAL8)*params->numSTKs);
 if (params->numSpinDown>0) {
    pTdotsAndDeltaTs->vecDeltaTs = (REAL8 **)LALMalloc(sizeof(REAL8 *)*params->numSTKs);          
    for(k=0;k<params->numSTKs;k++) {
        pTdotsAndDeltaTs->vecDeltaTs[k]  = (REAL8 *)LALMalloc(sizeof(REAL8)*params->numSpinDown);          
    }
 }

 /* Loop over sky positions */
 for(iSky=0;iSky<params->numSkyPosTotal;iSky++) {

   /* 10/28/04 gam; weight STKs; the weights depend on F_+ and F_x when params->weightSTKsIncludingBeamPattern is True */
   if (params->weightSTKsIncludingBeamPattern) {       
        /* 10/28/04 gam; get squared detector response F_+^2 or F_x^2 for one sky position, one polarization angle, for midpoints of a timeStamps */
        GetDetResponseTStampMidPts(status->statusPtr, params->detResponseTStampMidPts, params->timeStamps, params->numSTKs, params->tSTK,
             cachedDetector, params->skyPosData[iSky], params->orientationAngle, COORDINATESYSTEM_EQUATORIAL, params->plusOrCross);
         
        /* 10/28/04 gam; apply powerFlux style weights including detector beam pattern response */
        WeightSTKsIncludingBeamPattern(params->STKData,
              params->savSTKData,
              params->inverseSquareMedians,
              params->sumInverseSquareMedians,
              params->detResponseTStampMidPts,
              params->numSTKs, params->nBinsPerSTK, params->tSTK);
   }
   
   /* setup to call StackSlideComputeSky */

   csParams->skyPos[0]=params->skyPosData[iSky][0];
   csParams->skyPos[1]=params->skyPosData[iSky][1];
   csParams->baryinput->alpha=params->skyPosData[iSky][0];
   csParams->baryinput->delta=params->skyPosData[iSky][1];

   /* get Tdot and DeltaT's for this sky position */
   StackSlideComputeSky(status->statusPtr, pTdotsAndDeltaTs, csParams);
   CHECKSTATUSPTR (status);
      
   /* Loop over frequency derivatives */
   for(iFreqDeriv=0;iFreqDeriv<params->numFreqDerivIncludingNoSpinDown;iFreqDeriv++) {
    
    if (params->numSpinDown>0) {
       stksldParams->freqDerivData = params->freqDerivData[iFreqDeriv];
    }
    
    kSUM++; /* 04/12/05 gam; increment sum number */
    
    /* StackSlideOld(status->statusPtr,params->SUMData,params->STKData,stksldParams);*/ /*Feb 05/14 vir*/
    /* 04/12/05 gam; Move from using StackSlideOld to using StackSlide function. */
    /* 05/06/05 gam; If params->debugOptionFlag & 128 > 0 just creates a SUM from the STKs without sliding */
    if ((params->debugOptionFlag & 128) > 0 ) {
      #ifdef PRINT_SLIDINGTURNEDOFF_WARNING
        fprintf(stdout,"\n\n !!! WARNING: SLIDING TURNED OFF !!! \n\n");
        fflush(stdout);
      #endif
      SumStacks(status->statusPtr,params->SUMData,params->STKData,stksldParams);
    } else {
      StackSlide(status->statusPtr,params->SUMData,params->STKData,pTdotsAndDeltaTs,stksldParams);
    }
    CHECKSTATUSPTR (status);
    
    /* 02/25/05 gam; utility for printing one SUM to a file */
    if (params->outputSUMFlag > 0) {
      printOneStackSlideSUM(params->SUMData[0],params->outputSUMFlag,params->outputFile,kSUM,params->f0SUM,params->dfSUM,params->nBinsPerSUM,params->numSUMsTotal);
    }

    #ifdef DEBUG_SUM_TEMPLATEPARAMS
        fprintf(stdout,"\nTemplate parameters for SUM #%i:\n",kSUM);
        fprintf(stdout,"RA = %18.10f\n",params->skyPosData[iSky][0]);
        fprintf(stdout,"DEC = %18.10f\n",params->skyPosData[iSky][1]);
        for(k=0;k<params->numSpinDown;k++)
        {
           fprintf(stdout,"FREQDERIV%i = %18.10f\n",k+1,params->freqDerivData[iFreqDeriv][k]);
        } /* END for(k=0;k<params->numSpinDown;k++) */          
    #endif
    
    /* 01/21/04 gam; Call LALFindStackSlidePeaks only if thresholdFlag > 0 */
    if (params->thresholdFlag > 0) {
         /* 02/04/04 gam; Initialize list */ /* 02/11/04 gam; initial params->peaks here: */
         params->peaks = (SnglStackSlidePeriodicTable *)LALMalloc(sizeof(SnglStackSlidePeriodicTable));      
         params->peaks->next = NULL;
         peakSav = params->peaks; /* Save pointer to first peak */
         if (params->whichMCSUM > 0) {
           pLALFindStackSlidePeakParams->iSUM = params->whichMCSUM; /* 05/26/04 gam; this is the number of the Monte Carlo SUM. */
         } else {
           pLALFindStackSlidePeakParams->iSUM = kSUM; /* 02/04/04 gam added this and next 2 lines */
         }
         pLALFindStackSlidePeakParams->iSky = iSky;
         pLALFindStackSlidePeakParams->iDeriv = iFreqDeriv;
         pLALFindStackSlidePeakOutputs->pntrToPeaksPntr = &(params->peaks);  /* 03/02/04 gam; added this and changed next line */
         LALFindStackSlidePeaks(pLALFindStackSlidePeakOutputs, params->SUMData[0], pLALFindStackSlidePeakParams);
                 
         params->peaks = peakSav->next;  /* Go back to the beginning of the list */
         LALFree(peakSav);               /* No longer need this place holder */

         while (params->peaks) {
           peakSav = params->peaks;
           params->totalEventCount++; /* 05/25/05 gam */
           if (pLALFindStackSlidePeakParams->updateMeanStdDev) {
              /* 03/02/04 gam; update pw_mean_thissum, pw_stddev_thissum, pwr_snr using pwMeanWithoutPeaks and pwStdDevWithoutPeaks */              
              if ( (*(pLALFindStackSlidePeakOutputs->binsWithoutPeaks)) > 0) {
                /* Note that LALFindStackSlidePeaks ensures that pwStdDevWithoutPeaks is never 0. */
                params->peaks->pw_mean_thissum = *(pLALFindStackSlidePeakOutputs->pwMeanWithoutPeaks);
                params->peaks->pw_stddev_thissum = *(pLALFindStackSlidePeakOutputs->pwStdDevWithoutPeaks);
                params->peaks->pwr_snr = params->peaks->power / (*(pLALFindStackSlidePeakOutputs->pwStdDevWithoutPeaks));
              }
           }
           if (params->peaks->power > params->maxPower) {
              params->maxPower = params->peaks->power; /* 05/25/05 gam */ /* 02/09/04 gam; keep track for search summary */
           }
           
           /* 02/17/04 gam; keep an array of loudest peaks */
           if (params->outputLoudestFromPeaks) {
               LALUpdateLoudestFromPeaks(loudestPeaksArray, params->peaks, params->nBinsPerOutputEvent);
           } else {
              #ifdef INCLUDE_PRINT_PEAKS_TABLE_CODE
                if ((params->debugOptionFlag & 2) > 0 ) {
                  fprintf(stdout,"%11i %11i %10.6f %10.6f %14.6e %11.6f %10.3f %8.3f %12i \n",params->totalEventCount,params->peaks->sum_no,params->peaks->sky_ra,params->peaks->sky_dec,params->peaks->fderiv_1,params->peaks->frequency,params->peaks->power,params->peaks->pwr_snr,params->peaks->width_bins);
                  fflush(stdout);
                }
              #endif
              if (params->outputEventFlag > 0) {
                 /*  end previous row with comma */
                 if ( params->xmlStream->first ) {
                    params->xmlStream->first = 0;
                 } else { 
                    fprintf( params->xmlStream->fp, ",\n" );
                 }
                 /* print out the row */
                 /* 02/09/04 gam; remove fderiv_2-5; remove pw_max_thissum and freq_max_thissum; change false_alarm_prob_upperlimit to false_alarm_prob */
                 /* 02/12/04 gam; Add freq_index to SnglStackSlidePeriodicTable */
                 fprintf( params->xmlStream->fp, SNGL_LOCAL_STACKSLIDEPERIODIC_ROW,
		   params->peaks->sum_no,
		   params->peaks->event_no_thissum,
		   params->peaks->overlap_event,
		   params->peaks->frequency,
		   params->peaks->freq_index,
		   params->peaks->power,
		   params->peaks->width_bins,
		   params->peaks->num_subpeaks,
		   params->peaks->pwr_snr,
		   params->peaks->false_alarm_prob,
		   params->peaks->goodness_of_fit,
		   params->peaks->sky_ra,
		   params->peaks->sky_dec,
		   params->peaks->fderiv_1,
		   params->peaks->pw_mean_thissum,
		   params->peaks->pw_stddev_thissum
                );
              } /* end if (params->outputEventFlag > 0 */
	   } /* end if (params->outputLoudestFromPeaks) */
           params->peaks = params->peaks->next;
           LALFree(peakSav);
         } /* end while (params->peaks) */
    /* 02/11/04 gam; next is end if (params->thresholdFlag > 0) */       
    } else if (params->outputLoudestFromSUMs) {
         /* 02/17/04 gam */
         if (params->whichMCSUM > 0) {
           pLALUpdateLoudestStackSlideParams->iSUM = params->whichMCSUM; /* 05/26/04 gam; this is the number of the Monte Carlo SUM. */
         } else {
           pLALUpdateLoudestStackSlideParams->iSUM = kSUM;
         }
         pLALUpdateLoudestStackSlideParams->iSky = iSky;
         pLALUpdateLoudestStackSlideParams->iDeriv = iFreqDeriv;
         LALUpdateLoudestFromSUMs(loudestPeaksArray, params->SUMData[0], pLALUpdateLoudestStackSlideParams);
    } /* end else if (params->outputLoudestFromSUMs) */
  } /* 04/12/05 gam; END for(iFreqDeriv=0;iFreqDeriv<params->numFreqDerivIncludingNoSpinDown;iFreqDeriv++) */
 } /* 04/12/05 gam; END for(iSky=0;iSky<params->numSkyPosTotal;iSky++) */

 /* 02/17/04 gam; output the loudest events */
 if ( (params->outputLoudestFromPeaks || params->outputLoudestFromSUMs) && (params->outputEventFlag > 0) ) {
    for (k=0;k<params->keepThisNumber; k++) {
       #ifdef INCLUDE_PRINT_PEAKS_TABLE_CODE
         /* 04/15/04 gam */
         if ((params->debugOptionFlag & 2) > 0 ) {
          fprintf(stdout,"%11i %11i %10.6f %10.6f %14.6e %11.6f %10.3f %8.3f %12i \n",k+1,loudestPeaksArray[k].sum_no,loudestPeaksArray[k].sky_ra,loudestPeaksArray[k].sky_dec,loudestPeaksArray[k].fderiv_1,loudestPeaksArray[k].frequency,loudestPeaksArray[k].power,loudestPeaksArray[k].pwr_snr,loudestPeaksArray[k].width_bins);
          fflush(stdout);
         }
       #endif    
       if (loudestPeaksArray[k].power > params->maxPower) {
              params->maxPower = loudestPeaksArray[k].power; /* 05/25/05 gam */ /* 02/20/04 gam; keep track for search summary */
       }       
       /*  end previous row with comma */
       if ( params->xmlStream->first ) {
          params->xmlStream->first = 0;
       } else { 
          fprintf( params->xmlStream->fp, ",\n" );
       }
       fprintf( params->xmlStream->fp, SNGL_LOCAL_STACKSLIDEPERIODIC_ROW,
        loudestPeaksArray[k].sum_no,
        loudestPeaksArray[k].event_no_thissum,
        loudestPeaksArray[k].overlap_event,
        loudestPeaksArray[k].frequency,
        loudestPeaksArray[k].freq_index,
        loudestPeaksArray[k].power,
        loudestPeaksArray[k].width_bins,
        loudestPeaksArray[k].num_subpeaks,
        loudestPeaksArray[k].pwr_snr,
        loudestPeaksArray[k].false_alarm_prob,
        loudestPeaksArray[k].goodness_of_fit,
        loudestPeaksArray[k].sky_ra,
        loudestPeaksArray[k].sky_dec,
        loudestPeaksArray[k].fderiv_1,
        loudestPeaksArray[k].pw_mean_thissum,
        loudestPeaksArray[k].pw_stddev_thissum
       );
    } /* end for (k = 0; k < arraySize; k++) */
 } /* end if ( (params->outputLoudestFromPeaks || params->outputLoudestFromSUMs) && (params->outputEventFlag > 0) ) */

  /* 04/12/05 gam; allocate memory for parameters for StackSlideComputeSky and StackSlide */
 LALFree(csParams->skyPos);
 LALFree(csParams);
 if (params->numSpinDown>0) {
     for(k=0;k<params->numSTKs;k++) {
        LALFree(pTdotsAndDeltaTs->vecDeltaTs[k]);
     }
     LALFree(pTdotsAndDeltaTs->vecDeltaTs);
 }
 LALFree(pTdotsAndDeltaTs->vecTDots);
 LALFree(pTdotsAndDeltaTs);
  
 CHECKSTATUSPTR (status);
 DETATCHSTATUSPTR (status);

} /* END StackSlideIsolated */
/*********************************************************************************/
/*              END FUNCTION: StackSlideIsolated                                 */
/*********************************************************************************/

/*************************************************************/
/*                                                           */
/* START FUNCTION: RunStackSlideIsolatedMonteCarloSimulation */
/* Injects fake signals and runs Monte Carlo simulation.     */
/*                                                           */
/*************************************************************/
void RunStackSlideIsolatedMonteCarloSimulation(LALStatus *status, StackSlideSearchParams *params, INT4 nSamples)
{
#ifdef INCLUDE_RUNSTACKSLIDEISOLATEDMONTECARLO_CODE
  INT4    i = 0; /* all purpose index */
  INT4    j = 0; /* all purpose index */
  INT4    k = 0; /* all purpose index */  
  PulsarSignalParams *pPulsarSignalParams = NULL;
  REAL4TimeSeries *signal = NULL;
  LIGOTimeGPS GPSin;            /* reference-time for pulsar parameters at the detector; will convert to SSB! */
  LALDetector cachedDetector;
  REAL8 cosIota;                    /* cosine of inclination angle iota of the source */
  REAL8 h_0 = params->threshold4;   /* Source amplitude */
  SFTParams *pSFTParams = NULL;
  LIGOTimeGPSVector timestamps;
  SFTVector *outputSFTs = NULL;
  /* 07/14/04 gam; next two are needed by LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs */
  SkyConstAndZeroPsiAMResponse *pSkyConstAndZeroPsiAMResponse;
  SFTandSignalParams *pSFTandSignalParams;
  INT4 iLastSkyPos; /* 07/14/04 gam; keep track of index to the last sky position */
  REAL4 renorm;               /* Need to renormalize SFTs to account for different sample rates */  
  FFT **savBLKData = NULL;    /* 05/21/04 gam; save the input noise SFTs */
  REAL8 **savSkyPosData;      /* 05/26/04 gam; save Sky Position Data */
  REAL8 **savFreqDerivData;   /* 05/26/04 gam; save Frequency Derivative Data */
  INT4 numSkyPosTotal = 0;    /* 05/26/04 gam; total number of Sky positions to cover */
  INT4 numFreqDerivTotal = 0; /* 05/26/04 gam; total number of Frequency evolution models to cover */
  INT4 numParamSpacePts = 0;  /* 05/26/04 gam; total number of points in the parameter space to cover */
  INT4 numSUMsTotal = 0;      /* 05/26/04 gam; total Number of Sums output = numSUMsPerParamPt*numParamSpacePts */
  INT4 numSUMsTotalm1 = -1;                 /* 05/26/04 gam */
  INT4 numFreqDerivIncludingNoSpinDown = 0; /* 05/26/04 gam */
  INT4 kSUM = 0;                            /* 05/26/04 gam */
  INT4 iFreq = 0;                           /* 05/26/04 gam */  
  REAL8 f0SUM = 0.0;                        /* 05/26/04 gam */
  REAL8 bandSUM = 0.0;                      /* 05/26/04 gam */
  INT4 nBinsPerSUM = 0;                     /* 05/26/04 gam */
  INT4 nBinsPerSUMm1 = -1;                  /* 05/26/04 gam */  
  INT4 seed=0; /* 05/28/04 gam; next 14 are for computing random mismatch */
  REAL4 randval;
  RandomParams *randPar=NULL;
  FILE *fpRandom;
  INT4 rndCount;
  REAL8 cosTmpDEC;
  REAL8 tmpDeltaRA;
  /* REAL8 DeltaRA = params->stksldSkyPatchData->deltaRA;
  REAL8 DeltaDec = params->stksldSkyPatchData->deltaDec;
  REAL8 DeltaFDeriv1 = params->deltaFDeriv1; */
  /* 06/01/04 gam; For now fix DeltaRA, DeltaDec,and DeltaFDeriv1 to default values when finding mismatch during Monte Carlo */
  REAL8 DeltaRA = 0.01;
  REAL8 DeltaDec = 0.01;
  REAL8 DeltaFDeriv1 = -1.0e-10;
  REAL8 DeltaFDeriv2 = params->deltaFDeriv2;
  REAL8 DeltaFDeriv3 = params->deltaFDeriv3;
  REAL8 DeltaFDeriv4 = params->deltaFDeriv4;
  REAL8 DeltaFDeriv5 = params->deltaFDeriv5;
  INT4 firstNonExcludedBin = -1; /* 05/19/05 gam */
  INT4 lastNonExcludedBin  = -1; /* 05/19/05 gam */
  /* 05/24/05 gam; use results from prior jobs in the pipeline and report on current MC results */
  BOOLEAN reportMCResults = 0;
  REAL4 priorLoudestEvent = 0.0;
  REAL8 priorStartFreq = 0.0;
  REAL8 priorBand = 0.0;
  REAL8 priorConfidence = 0.0;
  REAL8 priorUL = 0.0;
  REAL8 priorUncertainty = 0.0;
  REAL8 nAboveLE = 0.0;
  REAL8 nTrials = 0.0;
  REAL8 *arrayULs = NULL;
  REAL8 *arrayConfs = NULL;
  INT4 *arrayConverged = NULL;
  INT4 iMC = 0;             /* which entire MC simulation is currently running */
  INT4 countMC = 0;         /* count of number of complted MC simulations */
  INT4 maxMC = 1;           /* maximum number of times to run the entire MC */
  REAL8 maxFracErr = 0.005; /* when interating, maximum fractional error to allow for convergence to desired confidence */
      
  INITSTATUS( status, "RunStackSlideIsolatedMonteCarloSimulation", STACKSLIDEISOLATEDC );
  ATTATCHSTATUSPTR(status);

  if ( (params->testFlag & 16) > 0 ) {
    /* 05/24/05 gam; use results from prior jobs in the pipeline and report on current MC results */
    reportMCResults = 1;
    /* get results for previous jobs in the pipeline */
    getStackSlidePriorResults(status->statusPtr,
                               &priorLoudestEvent,
                               &priorStartFreq,
                               &priorBand,
                               &priorConfidence,
                               &priorUL,
                               &priorUncertainty,
                               params->priorResultsFile);
    CHECKSTATUSPTR (status);
    #ifdef INCLUDE_PRINT_REPORTMCRESULTS_CODE
      if ((params->debugOptionFlag & 32) > 0 ) {
        fprintf(stdout,"Loudest Event, Start Frequency, Search Band, Confidence, Estimated Upper Limit, Uncertainty = \n");
        fprintf(stdout,"%20.6f %20.10f %20.10f %8.2f %20.10e %20.10e\n",priorLoudestEvent,priorStartFreq,priorBand,priorConfidence,priorUL,priorUncertainty);
        fflush(stdout);
      }
    #endif
    if ( (params->testFlag & 32) > 0 ) {
      /* 05/24/05 gam; iterate MC to converge on desired confidence */
      maxMC = params->maxMCinterations;
      maxFracErr = params->maxMCfracErr;
    }
    arrayULs   = (REAL8 *)LALMalloc(maxMC*sizeof(REAL8));
    arrayConfs = (REAL8 *)LALMalloc(maxMC*sizeof(REAL8));
    arrayConverged = (INT4 *)LALMalloc(maxMC*sizeof(INT4));
  }

  /* 05/21/04 gam; save the input noise SFTs */
  savBLKData=(FFT **)LALMalloc(params->numBLKs*sizeof(FFT *));
  for (i=0;i<params->numBLKs;i++)
  {
        savBLKData[i]=(FFT *)LALMalloc(sizeof(FFT));
        savBLKData[i]->fft=(COMPLEX8FrequencySeries *)LALMalloc(sizeof(COMPLEX8FrequencySeries));
        savBLKData[i]->fft->data=(COMPLEX8Vector *)LALMalloc(sizeof(COMPLEX8Vector));
        savBLKData[i]->fft->data->data=(COMPLEX8 *)LALMalloc(params->nBinsPerBLK*sizeof(COMPLEX8));

        savBLKData[i]->fft->epoch = params->BLKData[i]->fft->epoch;
        savBLKData[i]->fft->f0 = params->BLKData[i]->fft->f0;
        savBLKData[i]->fft->deltaF = params->BLKData[i]->fft->deltaF;
        savBLKData[i]->fft->data->length = params->BLKData[i]->fft->data->length;
        for(j=0;j<params->nBinsPerBLK;j++) {
           savBLKData[i]->fft->data->data[j].re = params->BLKData[i]->fft->data->data[j].re;
           savBLKData[i]->fft->data->data[j].im = params->BLKData[i]->fft->data->data[j].im;
        }
  }  
 
  /* 05/21/04 gam; save the input skyPosData */
  savSkyPosData=(REAL8 **)LALMalloc(params->numSkyPosTotal*sizeof(REAL8 *));
  for(i=0;i<params->numSkyPosTotal;i++)
  {
        savSkyPosData[i] = (REAL8 *)LALMalloc(2*sizeof(REAL8));
        savSkyPosData[i][0] = params->skyPosData[i][0];
        savSkyPosData[i][1] = params->skyPosData[i][1];
  }
  /* reallocate memory for the skyPosData structure */
  for(i=0;i<params->numSkyPosTotal;i++)
  {
      LALFree(params->skyPosData[i]);
  }
  LALFree(params->skyPosData);
  numSkyPosTotal = params->numSkyPosTotal;  /* sav original number of sky positions */
  params->numSkyPosTotal = 1;               /* Set up to run on one sky position at a time */
  params->skyPosData=(REAL8 **)LALMalloc(params->numSkyPosTotal*sizeof(REAL8 *));
  params->skyPosData[0] = (REAL8 *)LALMalloc(2*sizeof(REAL8));
  
  /* 05/21/04 gam; save the input freqDerivData */
  if (params->numSpinDown > 0) {
    savFreqDerivData=(REAL8 **)LALMalloc(params->numFreqDerivTotal*sizeof(REAL8 *));
    for(i=0;i<params->numFreqDerivTotal;i++)
    {
        savFreqDerivData[i] = (REAL8 *)LALMalloc(params->numSpinDown*sizeof(REAL8));
        for(j=0;j<params->numSpinDown;j++)
        {
          savFreqDerivData[i][j] = params->freqDerivData[i][j];
        }
    }
    /* reallocate memory for the freqDerivData structure */
    for(i=0;i<params->numFreqDerivTotal;i++)
    {
        LALFree(params->freqDerivData[i]);
    }
    LALFree(params->freqDerivData);
    numFreqDerivTotal = params->numFreqDerivTotal;
    params->numFreqDerivTotal = 1;
    params->freqDerivData=(REAL8 **)LALMalloc(params->numFreqDerivTotal*sizeof(REAL8 *));
    params->freqDerivData[0] = (REAL8 *)LALMalloc(params->numSpinDown*sizeof(REAL8));
  }
  numParamSpacePts = params->numParamSpacePts;
  numSUMsTotal = params->numSUMsTotal;
  params->numParamSpacePts = 1;
  params->numSUMsTotal = 1;
  if (numFreqDerivTotal != 0) {
     numFreqDerivIncludingNoSpinDown = numFreqDerivTotal;
  } else {
     numFreqDerivIncludingNoSpinDown = 1;  /* Even if numSpinDown = 0 still need to count case of zero spindown. */
  }
   
  /* Allocate memory for PulsarSignalParams and initialize */
  pPulsarSignalParams = (PulsarSignalParams *)LALMalloc(sizeof(PulsarSignalParams));
  pPulsarSignalParams->pulsar.position.system = COORDINATESYSTEM_EQUATORIAL;
  pPulsarSignalParams->pulsar.spindown = NULL;
  if (params->numSpinDown > 0) {
    LALDCreateVector(status->statusPtr, &(pPulsarSignalParams->pulsar.spindown),((UINT4)params->numSpinDown));
    CHECKSTATUSPTR (status);
  }
  pPulsarSignalParams->orbit = NULL; 
  pPulsarSignalParams->transfer = NULL;  
  /* Set up pulsar site */
  if (strstr(params->IFO, "LHO")) {
       cachedDetector = lalCachedDetectors[LALDetectorIndexLHODIFF];
  } else if (strstr(params->IFO, "LLO")) {
       cachedDetector = lalCachedDetectors[LALDetectorIndexLLODIFF];
  } else if (strstr(params->IFO, "GEO")) {
      cachedDetector = lalCachedDetectors[LALDetectorIndexGEO600DIFF];
  } else if (strstr(params->IFO, "VIRGO")) {
      cachedDetector = lalCachedDetectors[LALDetectorIndexVIRGODIFF];
  } else if (strstr(params->IFO, "TAMA")) {
      cachedDetector = lalCachedDetectors[LALDetectorIndexTAMA300DIFF];
  } else {
      /* "Invalid or null IFO" */
      /* ABORT( status, DRIVESTACKSLIDEH_EIFO, DRIVESTACKSLIDEH_MSGEIFO); */
  }    
  pPulsarSignalParams->site = &cachedDetector;     
  pPulsarSignalParams->ephemerides = params->edat;
  /* pPulsarSignalParams->startTimeGPS = params->actualStartTime; */ /* 06/01/04 gam; uses gpsStartTime requested; not actualStartTime */
  pPulsarSignalParams->startTimeGPS.gpsSeconds = (INT4)params->gpsStartTimeSec;
  pPulsarSignalParams->startTimeGPS.gpsNanoSeconds = (INT4)params->gpsStartTimeNan;
  pPulsarSignalParams->duration = (UINT4)params->duration;
  pPulsarSignalParams->samplingRate = (REAL8)ceil(2.0*params->bandBLK); /* Make sampleRate an integer so that T*samplingRate = integer for integer T */
  pPulsarSignalParams->fHeterodyne = params->f0BLK;  
  /* Find the time at the SSB that corresponds to the arrive time at the detector of first data requested. */
  GPSin.gpsSeconds = (INT4)params->gpsEpochStartTimeSec;     /* 12/06/04 gam; GPS epoch Sec that gives reference time in SSB */
  GPSin.gpsNanoSeconds = (INT4)params->gpsEpochStartTimeNan; /* 12/06/04 gam; GPS epoch Nan that gives reference time in SSB */
  
  /* Allocate memory for SFTParams and initialize */
  pSFTParams = (SFTParams *)LALMalloc(sizeof(SFTParams));
  pSFTParams->Tsft = params->tBLK;
  timestamps.length = params->numBLKs;
  timestamps.data = params->timeStamps; 
  pSFTParams->timestamps = &timestamps;
  pSFTParams->noiseSFTs = NULL;  
  
  /* 05/26/04 gam; initialize variables that keep track of which SUM we are working on */
  params->startSUMs = 1;   /* 05/26/04 gam; use to control I/O during Monte Carlo. Default is TRUE. */
  params->finishPeriodicTable = 0; /* 05/24/05 gam */ /* 05/26/04 gam; use to control I/O during Monte Carlo. Default is TRUE. */
  params->whichMCSUM = -1; /* 05/26/04 gam; which SUM the Monte Carlo Simulation is running on. Default is -1 */
  numSUMsTotalm1 = numSUMsTotal - 1;  /* Index of last SUM */
  
  /* 05/26/04 gam; initialize variables that keep track of which frequency we are working on */  
  f0SUM = params->f0SUM;
  bandSUM = params->bandSUM;
  nBinsPerSUM = params->nBinsPerSUM;  
  nBinsPerSUMm1 = nBinsPerSUM - 1;
  params->keepThisNumber = 1;      /* During MC only keep 1 event; this will be the injected event */
  
  /* check whether we are outputing just the loudest events */
  if ( ((params->outputEventFlag & 2) > 0) && (params->thresholdFlag <= 0) ) {
        params->bandSUM = params->dfSUM;
        params->nBinsPerSUM = 1;
  } else {
     /* TO DO: MAYBE SHOULD ABORT ?? */
  }
  
  /* 05/28/04 gam; Initial seed and randPar to use LALUniformDeviate to generate random mismatch during Monte Carlo. */
  fpRandom = fopen("/dev/urandom","r");
  rndCount = fread(&seed, sizeof(INT4),1, fpRandom);
  fclose(fpRandom);
  /* seed = 1234; */ /* Test value */
  LALCreateRandomParams(status->statusPtr, &randPar, seed);
  CHECKSTATUSPTR (status);
  
  /* 07/14/04 gam; allocate memory for structs needed by LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs */
  if ( (params->testFlag & 4) > 0 ) {
     #ifdef DEBUG_LALFASTGENERATEPULSARSFTS
          fprintf(stdout,"allocate memory for structs needed by LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs \n");
          fflush(stdout);
     #endif
     pSkyConstAndZeroPsiAMResponse = (SkyConstAndZeroPsiAMResponse *)LALMalloc(sizeof(SkyConstAndZeroPsiAMResponse));
     pSkyConstAndZeroPsiAMResponse->skyConst = (REAL8 *)LALMalloc((2*params->numSpinDown*(params->numBLKs+1)+2*params->numBLKs+3)*sizeof(REAL8));
     pSkyConstAndZeroPsiAMResponse->fPlusZeroPsi = (REAL4 *)LALMalloc(params->numBLKs*sizeof(REAL4));
     pSkyConstAndZeroPsiAMResponse->fCrossZeroPsi = (REAL4 *)LALMalloc(params->numBLKs*sizeof(REAL4));
     pSFTandSignalParams = (SFTandSignalParams *)LALMalloc(sizeof(SFTandSignalParams));
     /* create lookup table (LUT) values for doing trig */
     /* pSFTandSignalParams->resTrig = 64; */ /* length sinVal and cosVal; resolution of trig functions = 2pi/resTrig */
     pSFTandSignalParams->resTrig = 0; /* 08/02/04 gam; avoid serious bug when using LUTs for trig calls */
     pSFTandSignalParams->trigArg = (REAL8 *)LALMalloc((pSFTandSignalParams->resTrig+1)*sizeof(REAL8));
     pSFTandSignalParams->sinVal  = (REAL8 *)LALMalloc((pSFTandSignalParams->resTrig+1)*sizeof(REAL8));
     pSFTandSignalParams->cosVal  = (REAL8 *)LALMalloc((pSFTandSignalParams->resTrig+1)*sizeof(REAL8));
     for (k=0; k<=pSFTandSignalParams->resTrig; k++) {
       pSFTandSignalParams->trigArg[k]= ((REAL8)LAL_TWOPI) * ((REAL8)k) / ((REAL8)pSFTandSignalParams->resTrig);
       pSFTandSignalParams->sinVal[k]=sin( pSFTandSignalParams->trigArg[k] );
       pSFTandSignalParams->cosVal[k]=cos( pSFTandSignalParams->trigArg[k] );
     }
     pSFTandSignalParams->pSigParams = pPulsarSignalParams;
     pSFTandSignalParams->pSFTParams = pSFTParams;
     pSFTandSignalParams->nSamples = nSamples;
     pPulsarSignalParams->samplingRate = 2.0*params->bandBLK; /* can set samplingRate to exactly 2.0*bandBLK when using LALFastGeneratePulsarSFTs */
     GPSin.gpsSeconds = params->timeStamps[0].gpsSeconds;         /* 08/02/04 gam */
     GPSin.gpsNanoSeconds = params->timeStamps[0].gpsNanoSeconds; /* 08/02/04 gam */
  } else { 
     pSkyConstAndZeroPsiAMResponse = NULL;
     pSFTandSignalParams = NULL;
  }

  /* 05/19/05 gam; find first and last nonexcluded frequency bins */
  for(iFreq=0;iFreq<nBinsPerSUM;iFreq++) {     
     /* 05/19/05 gam; Only inject into bins that are not exclude */
     if (params->sumBinMask[iFreq] != 0) {
        if (firstNonExcludedBin == -1) {
           firstNonExcludedBin = iFreq;
        }
        lastNonExcludedBin = iFreq;
     }
  }

 /***********************************************************/
 /*                                                         */
 /* START SECTION: LOOP OVER ENTIRE MONTE CARLO SIMULATIONS */
 /*                                                         */
 /***********************************************************/
 for(iMC=0;iMC<maxMC;iMC++) {
   
  /* This is the big loop that reruns the entire MC maxMC times or until MC has converged */
  nAboveLE = 0.0; /* initialize at beginning of each MC */
  nTrials = 0.0;  /* initialize at beginning of each MC */

  /*********************************************************/
  /*                                                       */
  /* START SECTION: MONTE CARLO LOOP OVER PARAMETER SPACE  */
  /*                                                       */
  /*********************************************************/
  iLastSkyPos = -1; /* 07/14/04 gam initialize */
  for(kSUM=0;kSUM<numSUMsTotal;kSUM++) {
  
    params->whichMCSUM = kSUM; /* keep track in StackSlideApplySearch of which SUM we are injecting into. */  
    
    /* 05/26/04 gam; set up pointer to current sky position and spindown parameters */
    i = kSUM/numFreqDerivIncludingNoSpinDown;   /* index to params->skyPosData for this SUM; */
    j = kSUM % numFreqDerivIncludingNoSpinDown; /* index to params->freqDerivData for this SUM; */
    params->skyPosData[0][0] = savSkyPosData[i][0];
    params->skyPosData[0][1] = savSkyPosData[i][1];        
    LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
    #ifdef DEBUG_SETFIXED_RANDVAL
       randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
    #endif    
    pPulsarSignalParams->pulsar.position.latitude = params->skyPosData[0][1] + (((REAL8)randval) - 0.5)*DeltaDec;
    cosTmpDEC = cos(savSkyPosData[i][1]);
    if (cosTmpDEC != 0.0) {
          tmpDeltaRA = DeltaRA/cosTmpDEC;
    } else {
          tmpDeltaRA = 0.0; /* We are at a celestial pole */
    }
    LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
    #ifdef DEBUG_SETFIXED_RANDVAL
       randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
    #endif    
    pPulsarSignalParams->pulsar.position.longitude = params->skyPosData[0][0] + (((REAL8)randval) - 0.5)*tmpDeltaRA;
    /* pPulsarSignalParams->pulsar.position.longitude = params->skyPosData[0][0];
    pPulsarSignalParams->pulsar.position.latitude = params->skyPosData[0][1]; */
    #ifdef DEBUG_RANDOMTRIALPARAMETERS
        fprintf(stdout,"kSUM = %i, search RA = %23.10e, inject RA = %23.10e \n",kSUM,params->skyPosData[0][0],pPulsarSignalParams->pulsar.position.longitude);
        fprintf(stdout,"kSUM = %i, search DEC = %23.10e, inject DEC = %23.10e \n",kSUM,params->skyPosData[0][1],pPulsarSignalParams->pulsar.position.latitude);
        fflush(stdout);      
    #endif

    if (params->numSpinDown > 0) {
      for(k=0;k<params->numSpinDown;k++) {
        params->freqDerivData[0][k] = savFreqDerivData[j][k];
        LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
        #ifdef DEBUG_SETFIXED_RANDVAL
           randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
        #endif
        if (k == 0) {
          pPulsarSignalParams->pulsar.spindown->data[k] = params->freqDerivData[0][k] + (((REAL8)randval) - 0.5)*DeltaFDeriv1;
        } else if (k == 1) {
          pPulsarSignalParams->pulsar.spindown->data[k] = params->freqDerivData[0][k] + (((REAL8)randval) - 0.5)*DeltaFDeriv2;
        } else if (k == 2) {
          pPulsarSignalParams->pulsar.spindown->data[k] = params->freqDerivData[0][k] + (((REAL8)randval) - 0.5)*DeltaFDeriv3;
        } else if (k == 3) {
          pPulsarSignalParams->pulsar.spindown->data[k] = params->freqDerivData[0][k] + (((REAL8)randval) - 0.5)*DeltaFDeriv4;
        } else if (k == 4) {
          pPulsarSignalParams->pulsar.spindown->data[k] = params->freqDerivData[0][k] + (((REAL8)randval) - 0.5)*DeltaFDeriv5;
        } /* END if (k == 0) ELSE ... */
        /* pPulsarSignalParams->pulsar.spindown->data[k] = params->freqDerivData[0][k]; */
        #ifdef DEBUG_RANDOMTRIALPARAMETERS
          fprintf(stdout,"kSUM = %i, search fDeriv[%i] = %23.10e, inject fDeriv[%i] = %23.10e \n",kSUM,k,params->freqDerivData[0][k],k,pPulsarSignalParams->pulsar.spindown->data[k]);
          fflush(stdout);      
        #endif
      }
    }
    
    /* 07/14/04 gam; one per sky position update the reference time for this sky position */
    if ( iLastSkyPos != i ) {
      LALConvertGPS2SSB(status->statusPtr,&(pPulsarSignalParams->pulsar.tRef), GPSin, pPulsarSignalParams);
      CHECKSTATUSPTR (status);
      
      /* 07/14/04 gam; one per sky position fill in SkyConstAndZeroPsiAMResponse for use with LALFastGeneratePulsarSFTs */
      if ( (params->testFlag & 4) > 0 ) {
        #ifdef DEBUG_LALFASTGENERATEPULSARSFTS
            fprintf(stdout,"about to call LALComputeSkyAndZeroPsiAMResponse \n");
            fflush(stdout);
        #endif
        LALComputeSkyAndZeroPsiAMResponse (status->statusPtr, pSkyConstAndZeroPsiAMResponse, pSFTandSignalParams);
        CHECKSTATUSPTR (status);
      }      
    }    
    iLastSkyPos = i; /* Update the index to the last Sky Position */
    
    /****************************************************/
    /*                                                  */
    /* START SECTION: MONTE CARLO LOOP OVER FREQUENCIES */
    /*                                                  */
    /****************************************************/    
    for(iFreq=0;iFreq<nBinsPerSUM;iFreq++) {
    
     /* if (iFreq > 0) */ /* 05/19/05 gam */
     if (iFreq > firstNonExcludedBin) {
       params->startSUMs = 0;  /* Indicate that SUMs already started */
     }
    
     /* if ((kSUM == numSUMsTotalm1) && (iFreq == nBinsPerSUMm1)) */ /* 05/19/05 gam */
     /* 05/24/05 gam */ /* This is the last injection */
     /* 05/24/05 gam; leave params->finishPeriodicTable == 0; will finish periodic table below */
     /* if ((kSUM == numSUMsTotalm1) && (iFreq == lastNonExcludedBin)) {
         params->finishPeriodicTable = 1;
     } */

     /* 05/19/05 gam; Only inject into bins that are not exclude */
     if (params->sumBinMask[iFreq] != 0) {
     
      /* 12/06/04 gam */
      if ( (params->testFlag & 8) > 0 ) {
         pPulsarSignalParams->pulsar.psi = params->orientationAngle;
         cosIota =params->cosInclinationAngle;
      } else {
         /* get random value for psi */
         LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
         #ifdef DEBUG_SETFIXED_RANDVAL
            randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
         #endif
         pPulsarSignalParams->pulsar.psi = (randval - 0.5) * ((REAL4)LAL_PI_2);
         /* pPulsarSignalParams->pulsar.psi = 0.0; */
    
         /* get random value for cosIota */
         LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
         #ifdef DEBUG_SETFIXED_RANDVAL
            randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
         #endif
         cosIota = 2.0*((REAL8)randval) - 1.0;
         /* cosIota = 1.0; */
      }
      
      /* h_0 is fixed equal to params->threshold4 above; get A_+ and A_x from h_0 and random cosIota */
      pPulsarSignalParams->pulsar.aPlus = (REAL4)(0.5*h_0*(1.0 + cosIota*cosIota));
      pPulsarSignalParams->pulsar.aCross = (REAL4)(h_0*cosIota);

      /* get random value for phi0 */
      LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
      #ifdef DEBUG_SETFIXED_RANDVAL
         randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
      #endif
      pPulsarSignalParams->pulsar.phi0 = ((REAL8)randval) * ((REAL8)LAL_TWOPI);
      /* pPulsarSignalParams->pulsar.phi0 = 0.0; */

      pPulsarSignalParams->pulsar.f0 = f0SUM + iFreq*params->dfSUM; /* We add in the mismatch after adjusting params->f0SUM */
      /* check whether we are outputing just the loudest events */
      if ( ((params->outputEventFlag & 2) > 0) && (params->thresholdFlag <= 0) ) {
        params->f0SUM = pPulsarSignalParams->pulsar.f0;
      } else {
        /* TO DO: MAYBE SHOULD ABORT ABOVE AND ALWAY SET params->f0SUM as above ?? */
      }
      /* Now add mismatch to pPulsarSignalParams->pulsar.f0 */
      LALUniformDeviate(status->statusPtr, &randval, randPar); CHECKSTATUSPTR (status); /* 05/28/04 gam */
      #ifdef DEBUG_SETFIXED_RANDVAL
         randval = params->threshold5; /* Temporarily use threshold5 for this; need to add testParameters to commandline. */
      #endif      
      pPulsarSignalParams->pulsar.f0 = pPulsarSignalParams->pulsar.f0 + (((REAL8)randval) - 0.5)*params->dfSUM;
      
      #ifdef DEBUG_RANDOMTRIALPARAMETERS
        fprintf(stdout,"iFreq = %i, inject h_0 = %23.10e \n",iFreq,h_0);
        fprintf(stdout,"iFreq = %i, inject cosIota = %23.10e, A_+ = %23.10e, A_x = %23.10e \n",iFreq,cosIota,pPulsarSignalParams->pulsar.aPlus,pPulsarSignalParams->pulsar.aCross);
        fprintf(stdout,"iFreq = %i, inject psi = %23.10e \n",iFreq,pPulsarSignalParams->pulsar.psi);
        fprintf(stdout,"iFreq = %i, inject phi0 = %23.10e \n",iFreq,pPulsarSignalParams->pulsar.phi0);
        fprintf(stdout,"iFreq = %i, search f0 = %23.10e, inject f0 = %23.10e \n",iFreq,f0SUM + iFreq*params->dfSUM,pPulsarSignalParams->pulsar.f0);
        fflush(stdout);
      #endif
      #ifdef DEBUG_MONTECARLOTIMEDOMAIN_DATA
        fprintf(stdout,"iFreq = %i, params->f0SUM = %23.10e, pPulsarSignalParams->pulsar.f0 = %23.10e \n",iFreq,params->f0SUM,pPulsarSignalParams->pulsar.f0);
        fprintf(stdout,"nBinsPerSUM = %i, params->nBinsPerSUM = %i\n",nBinsPerSUM,params->nBinsPerSUM);
        fflush(stdout);	
      #endif
      
      /* 07/14/04 gam; check if using LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs */
      if ( (params->testFlag & 4) > 0 ) {
        #ifdef DEBUG_LALFASTGENERATEPULSARSFTS
          fprintf(stdout,"About to call LALFastGeneratePulsarSFTs \n");
          fflush(stdout);
        #endif
        /* 07/14/04 gam; use SkyConstAndZeroPsiAMResponse from LALComputeSkyAndZeroPsiAMResponse and SFTandSignalParams to generate SFTs fast. */
        LALFastGeneratePulsarSFTs (status->statusPtr, &outputSFTs, pSkyConstAndZeroPsiAMResponse, pSFTandSignalParams);
        CHECKSTATUSPTR (status);
        
        #ifdef PRINT_ONEMONTECARLO_OUTPUTSFT
          if (params->whichMCSUM == 0 && iFreq == 0) {
            REAL4  fPlus;
            REAL4  fCross;
            i=0; /* index of which outputSFT to output */
            fPlus = pSkyConstAndZeroPsiAMResponse->fPlusZeroPsi[i]*cos(pPulsarSignalParams->pulsar.psi) + pSkyConstAndZeroPsiAMResponse->fCrossZeroPsi[i]*sin(pPulsarSignalParams->pulsar.psi);
            fCross = pSkyConstAndZeroPsiAMResponse->fCrossZeroPsi[i]*cos(pPulsarSignalParams->pulsar.psi) - pSkyConstAndZeroPsiAMResponse->fPlusZeroPsi[i]*sin(pPulsarSignalParams->pulsar.psi);
            fprintf(stdout,"iFreq = %i, inject h_0 = %23.10e \n",iFreq,h_0);
            fprintf(stdout,"iFreq = %i, inject cosIota = %23.10e, A_+ = %23.10e, A_x = %23.10e \n",iFreq,cosIota,pPulsarSignalParams->pulsar.aPlus,pPulsarSignalParams->pulsar.aCross);
            fprintf(stdout,"iFreq = %i, inject psi = %23.10e \n",iFreq,pPulsarSignalParams->pulsar.psi);
            fprintf(stdout,"iFreq = %i, fPlus, fCross = %23.10e,  %23.10e \n",iFreq,fPlus,fCross);
            fprintf(stdout,"iFreq = %i, inject phi0 = %23.10e \n",iFreq,pPulsarSignalParams->pulsar.phi0);
            fprintf(stdout,"iFreq = %i, search f0 = %23.10e, inject f0 = %23.10e \n",iFreq,f0SUM + iFreq*params->dfSUM,pPulsarSignalParams->pulsar.f0);
            fprintf(stdout,"outputSFTs->data[%i].data->length = %i \n",i,outputSFTs->data[i].data->length);
            fflush(stdout);
            for(j=0;j<params->nBinsPerBLK;j++) {
               /* fprintf(stdout,"%i %g %g \n",j,outputSFTs->data[i].data->data[j].re,outputSFTs->data[i].data->data[j].im); */
               fprintf(stdout,"%i %g \n",j,outputSFTs->data[i].data->data[j].re*outputSFTs->data[i].data->data[j].re + outputSFTs->data[i].data->data[j].im*outputSFTs->data[i].data->data[j].im);
               fflush(stdout);
            }
          }
        #endif
        
        #ifdef PRINT_MAXPOWERANDBINEACHSFT
         {
           INT4 jMaxPwr;
           REAL4 maxPwr;
           REAL4 pwr;
           for(i=0;i<params->numBLKs;i++) {
             maxPwr = 0.0;
             jMaxPwr = -1;
             for(j=0;j<params->nBinsPerBLK;j++) {
               pwr = outputSFTs->data[i].data->data[j].re*outputSFTs->data[i].data->data[j].re + outputSFTs->data[i].data->data[j].im*outputSFTs->data[i].data->data[j].im;
               if (pwr > maxPwr) {
                  maxPwr = pwr;
                  jMaxPwr = j;
               }
             }
             fprintf(stdout,"Max power for SFT %i is in bin %i = %g \n",i,jMaxPwr,maxPwr);
             fflush(stdout);
           }
         }
        #endif

        /* Add outputSFTs with injected signal to input noise SFTs; no renorm should be needed */
        for(i=0;i<params->numBLKs;i++) {
          for(j=0;j<params->nBinsPerBLK;j++) {
             #ifdef PRINTCOMPARISON_INPUTVSMONTECARLOSFT_DATA
               fprintf(stdout,"savBLKData[%i]->fft->data->data[%i].re, outputSFTs->data[%i].data->data[%i].re = %g, %g\n",i,j,i,j,savBLKData[i]->fft->data->data[j].re,outputSFTs->data[i].data->data[j].re);
               fprintf(stdout,"savBLKData[%i]->fft->data->data[%i].im, outputSFTs->data[%i].data->data[%i].im = %g, %g\n",i,j,i,j,savBLKData[i]->fft->data->data[j].im,outputSFTs->data[i].data->data[j].im);
               fflush(stdout);
             #endif
             params->BLKData[i]->fft->data->data[j].re = savBLKData[i]->fft->data->data[j].re + outputSFTs->data[i].data->data[j].re;
             params->BLKData[i]->fft->data->data[j].im = savBLKData[i]->fft->data->data[j].im + outputSFTs->data[i].data->data[j].im;
          }
        }
        /* next is else for if ( (params->testFlag & 4) > 0 ) else... */
      } else {
        #ifdef PRINT_ONEMONTECARLO_OUTPUTSFT
          if (params->whichMCSUM == 0 && iFreq == 0) {
            /* To compare with LALFastGeneratePulsarSFTs, which uses ComputeSky, use first time stamp as reference time */
            GPSin.gpsSeconds = params->timeStamps[0].gpsSeconds;
            GPSin.gpsNanoSeconds = params->timeStamps[0].gpsNanoSeconds;
            LALConvertGPS2SSB(status->statusPtr,&(pPulsarSignalParams->pulsar.tRef), GPSin, pPulsarSignalParams); CHECKSTATUSPTR (status);
          }
        #endif

        signal = NULL; /* Call LALGeneratePulsarSignal to generate signal */
        LALGeneratePulsarSignal(status->statusPtr, &signal, pPulsarSignalParams);
        CHECKSTATUSPTR (status);

        #ifdef DEBUG_MONTECARLOTIMEDOMAIN_DATA
          fprintf(stdout,"signal->deltaT = %23.10e \n",signal->deltaT);
          fprintf(stdout,"signal->epoch.gpsSeconds = %i \n",signal->epoch.gpsSeconds);
          fprintf(stdout,"signal->epoch.gpsNanoSeconds = %i \n",signal->epoch.gpsNanoSeconds);
          fprintf(stdout,"pPulsarSignalParams->duration*pPulsarSignalParams->samplingRate, signal->data->length = %g %i\n",pPulsarSignalParams->duration*pPulsarSignalParams->samplingRate,signal->data->length);
          fflush(stdout);
        #endif
        #ifdef PRINT_MONTECARLOTIMEDOMAIN_DATA
          for(i=0;i<signal->data->length;i++)  {
            fprintf(stdout,"signal->data->data[%i] = %g\n",i,signal->data->data[i]);
            fflush(stdout);
          }
        #endif

        outputSFTs = NULL; /* Call LALSignalToSFTs to generate outputSFTs with injection data */
        LALSignalToSFTs(status->statusPtr, &outputSFTs, signal, pSFTParams);
        CHECKSTATUSPTR (status);

        #ifdef PRINT_ONEMONTECARLO_OUTPUTSFT
           if (params->whichMCSUM == 0 && iFreq == 0) {
              REAL4 realHetPhaseCorr;
              REAL4 imagHetPhaseCorr;
              REAL8 deltaTHeterodyne;
              i=0; /* index of which outputSFT to output */
              fprintf(stdout,"iFreq = %i, inject h_0 = %23.10e \n",iFreq,h_0);
              fprintf(stdout,"iFreq = %i, inject cosIota = %23.10e, A_+ = %23.10e, A_x = %23.10e \n",iFreq,cosIota,pPulsarSignalParams->pulsar.aPlus,pPulsarSignalParams->pulsar.aCross);
              fprintf(stdout,"iFreq = %i, inject psi = %23.10e \n",iFreq,pPulsarSignalParams->pulsar.psi);
              fprintf(stdout,"iFreq = %i, inject phi0 = %23.10e \n",iFreq,pPulsarSignalParams->pulsar.phi0);
              fprintf(stdout,"iFreq = %i, search f0 = %23.10e, inject f0 = %23.10e \n",iFreq,f0SUM + iFreq*params->dfSUM,pPulsarSignalParams->pulsar.f0);
              fprintf(stdout,"outputSFTs->data[%i].data->length = %i \n",i,outputSFTs->data[i].data->length);
              /* deltaTHeterodyne = the time from the refererence time to the start of an SFT */
              GPSin.gpsSeconds = params->timeStamps[i].gpsSeconds;
              GPSin.gpsNanoSeconds = params->timeStamps[i].gpsNanoSeconds;
              TRY (LALDeltaFloatGPS (status->statusPtr, &deltaTHeterodyne, &GPSin, &(pPulsarSignalParams->pulsar.tRef)), status);
              /* deltaTHeterodyne = 0.0; */ /* UNCOMMENT TO TURN OFF CORRECTION OF PHASE */
              fprintf(stdout,"t_h = %23.10e\n",deltaTHeterodyne);
              fflush(stdout);
              realHetPhaseCorr = cos(LAL_TWOPI*pPulsarSignalParams->fHeterodyne*deltaTHeterodyne);
              imagHetPhaseCorr = sin(LAL_TWOPI*pPulsarSignalParams->fHeterodyne*deltaTHeterodyne);
              renorm = ((REAL4)nSamples)/((REAL4)(outputSFTs->data[i].data->length - 1));
              for(j=0;j<params->nBinsPerBLK;j++) {
                /* fprintf(stdout,"%i %g %g \n",j,renorm*outputSFTs->data[i].data->data[j].re*realHetPhaseCorr - renorm*outputSFTs->data[i].data->data[j].im*imagHetPhaseCorr,renorm*outputSFTs->data[i].data->data[j].re*imagHetPhaseCorr + renorm*outputSFTs->data[i].data->data[j].im*realHetPhaseCorr); */
                fprintf(stdout,"%i %g \n",j,renorm*renorm*outputSFTs->data[i].data->data[j].re*outputSFTs->data[i].data->data[j].re + renorm*renorm*outputSFTs->data[i].data->data[j].im*outputSFTs->data[i].data->data[j].im);
                fflush(stdout);
              }
           }
        #endif

        #ifdef PRINT_MAXPOWERANDBINEACHSFT
         {
           INT4 jMaxPwr;
           REAL4 maxPwr;
           REAL4 pwr;
           for(i=0;i<params->numBLKs;i++) {
             maxPwr = 0.0;
             jMaxPwr = -1;
             renorm = ((REAL4)nSamples)/((REAL4)(outputSFTs->data[i].data->length - 1));
             for(j=0;j<params->nBinsPerBLK;j++) {
               pwr = outputSFTs->data[i].data->data[j].re*outputSFTs->data[i].data->data[j].re + outputSFTs->data[i].data->data[j].im*outputSFTs->data[i].data->data[j].im;
               pwr = renorm*renorm*pwr;
               if (pwr > maxPwr) {
                  maxPwr = pwr;
                  jMaxPwr = j;
               }
             }
             fprintf(stdout,"Max power for SFT %i is in bin %i = %g \n",i,jMaxPwr,maxPwr);
             fflush(stdout);
           }
         }
        #endif
        
        /* Add outputSFTs with injected signal to input noise SFTs; renorm is needed. */
        /* 05/21/04 gam; Normalize the SFTs from LALSignalToSFTs as per the normalization in makefakedata_v2.c and lalapps/src/pulsar/make_sfts.c. */
        renorm = ((REAL4)nSamples)/((REAL4)(outputSFTs->data[0].data->length - 1)); /* 05/21/04 gam; should be the same for all outputSFTs; note minus 1 is needed */
        for(i=0;i<params->numBLKs;i++) {
          /* renorm = ((REAL4)nSamples)/((REAL4)outputSFTs->data[i].data->length); */ /* 05/21/04 gam; do once correctly above */ /* Should be the same for all SFTs, but just in case recompute. */
          #ifdef DEBUG_MONTECARLOSFT_DATA  
            fprintf(stdout,"Mutiplying outputSFTs->data[%i] with renorm = %g \n",i,renorm);
            fflush(stdout);
          #endif  
          for(j=0;j<params->nBinsPerBLK;j++) {
             #ifdef PRINTCOMPARISON_INPUTVSMONTECARLOSFT_DATA
               fprintf(stdout,"savBLKData[%i]->fft->data->data[%i].re, outputSFTs->data[%i].data->data[%i].re = %g, %g\n",i,j,i,j,savBLKData[i]->fft->data->data[j].re,renorm*outputSFTs->data[i].data->data[j].re);
               fprintf(stdout,"savBLKData[%i]->fft->data->data[%i].im, outputSFTs->data[%i].data->data[%i].im = %g, %g\n",i,j,i,j,savBLKData[i]->fft->data->data[j].im,renorm*outputSFTs->data[i].data->data[j].im);
               fflush(stdout);
             #endif
             /* 05/21/04 gam; changed next two lines so that params->BLKData is sum of saved noise SFTs and SFTs with injection data. */
             params->BLKData[i]->fft->data->data[j].re = savBLKData[i]->fft->data->data[j].re + renorm*outputSFTs->data[i].data->data[j].re;
             params->BLKData[i]->fft->data->data[j].im = savBLKData[i]->fft->data->data[j].im + renorm*outputSFTs->data[i].data->data[j].im;
          }
        }
      } /* END if ( (params->testFlag & 4) > 0 ) else */

      #ifdef DEBUG_MONTECARLOSFT_DATA  
        fprintf(stdout,"params->numBLKs, outputSFTs->length = %i, %i \n",params->numBLKs,outputSFTs->length);
        fflush(stdout);
        for(i=0;i<outputSFTs->length;i++) {
          fprintf(stdout,"params->f0BLK, outputSFTs->data[%i].f0 = %g, %g\n",i,params->f0BLK,outputSFTs->data[i].f0);
          fprintf(stdout,"params->tBLK, 1.0/outputSFTs->data[%i].deltaF = %g, %g\n",i,params->tBLK,1.0/outputSFTs->data[i].deltaF);
          fprintf(stdout,"params->timeStamps[%i].gpsSeconds, outputSFTs->data[%i].epoch.gpsSeconds = %i, %i\n",i,i,params->timeStamps[i].gpsSeconds,outputSFTs->data[i].epoch.gpsSeconds);
          fprintf(stdout,"params->timeStamps[%i].gpsNanoSeconds, outputSFTs->data[%i].epoch.gpsNanoSeconds = %i, %i\n",i,i,params->timeStamps[i].gpsNanoSeconds,outputSFTs->data[i].epoch.gpsNanoSeconds);
          fprintf(stdout,"params->nBinsPerBLK, outputSFTs->data[%i].data->length = %i, %i\n",i,params->nBinsPerBLK,outputSFTs->data[i].data->length);
          fflush(stdout);
        }
      #endif
      #ifdef PRINT_MONTECARLOSFT_DATA
        for(i=0;i<outputSFTs->length;i++) {
          for(j=0;j<outputSFTs->data[i].data->length;j++) {
            fprintf(stdout,"outputSFTs->data[%i].data->data[%i].re, outputSFTs->data[%i].data->data[%i].im = %g, %g\n",i,j,i,j,outputSFTs->data[i].data->data[j].re,outputSFTs->data[i].data->data[j].im);
            fflush(stdout);
          }
        }
      #endif
      
      /* 07/14/04 gam; check if using LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs */
      if ( !((params->testFlag & 4) > 0) ) {
        LALDestroySFTVector(status->statusPtr, &outputSFTs);
        CHECKSTATUSPTR (status); /* 06/01/04 gam */
      
        LALFree(signal->data->data);
        LALFree(signal->data);
        LALFree(signal);
      }
      
      /* Call StackSlideApplySearch to analyze this MC injection */
      params->maxPower = 0.0; /* 05/24/05 gam; need to reinitialize */
      StackSlideApplySearch(status->statusPtr,params);
      CHECKSTATUSPTR (status);

      /* 05/24/05 gam */
      if (reportMCResults) {
         nTrials += 1.0;  /* count total number of MC trials run */
         /* priorLoudestEvent is the Loudest event from prior jobs in the pipeline */
         if (params->maxPower >= priorLoudestEvent) {
            nAboveLE += 1.0;  /* count number of MC trials that result in power >= to the loudest event */
         }
      }

     } /* END if (params->sumBinMask[k] != 0) */
    } /* END for(iFreq=0;iFreq<nBinsPerSUM;iFreq++) */
    /****************************************************/
    /*                                                  */
    /* END SECTION: MONTE CARLO LOOP OVER FREQUENCIES   */
    /*                                                  */
    /****************************************************/
  } /* END for(kSUM=0;kSUM<numSUMsTotal;kSUM++) */
  /*********************************************************/
  /*                                                       */
  /* END SECTION: MONTE CARLO LOOP OVER PARAMETER SPACE    */
  /*                                                       */
  /*********************************************************/

  /* 05/24/05 gam */  
  if (reportMCResults) {

     countMC++; /* count how many MC simulations have been completed */
     
     /* Compute confidence; note that h_0 == params->threshold4 == injection amplitude */
     arrayULs[iMC] = h_0;
     if (nTrials > 0.0) {
       arrayConfs[iMC] = nAboveLE/nTrials;
       /* Check, are we done? */     
       if ( fabs(priorConfidence - arrayConfs[iMC])/priorConfidence <= maxFracErr ) {
          arrayConverged[iMC] = 1;  /* converged on desired confidence */
          break; /* We are within maxFracErr; break out of the iMC loop */
       } else {
           arrayConverged[iMC] = 0; /* not converged on desired confidence */
       }       
     } else {
       arrayConfs[iMC] = -1;
       arrayConverged[iMC] = 0; /* not converged on desired confidence */
       break; /* MC not producing any trials; should not happen but stop if it does */
     }

     /* if (maxMC > 1) then interate entire MC to converge on desired confidence */
     if (maxMC > 1) {
        /* Compute new guess for h_0 */
        if ( ( iMC > 0 ) && ( (arrayConfs[iMC] - arrayConfs[iMC-1]) != 0.0 ) ) {
           /* linearly interpolate to next guess */
           h_0 = arrayULs[iMC] + (priorConfidence - arrayConfs[iMC])*(arrayULs[iMC]
                 - arrayULs[iMC-1])/(arrayConfs[iMC] - arrayConfs[iMC-1]);
        } else {
           if ( (priorConfidence - arrayConfs[iMC]) > 0.0 ) {
              h_0 = arrayULs[iMC] + priorUncertainty; /* need to make h_0 larger */
           } else {
              h_0 = arrayULs[iMC] - priorUncertainty; /* need to make h_0 smaller */
           }
        }
        if (h_0 <= 0.0) {
           h_0 = priorUncertainty; /* prevent making h_0 too small */
        }
        params->threshold4 = h_0; /* set params->threshold4 == h_0 to new guess */
     }
  } /* END if (reportMCResults) */

 } /* END for(iMC=0;iMC<maxMC;iMC++) */
 /***********************************************************/
 /*                                                         */
 /* END SECTION: LOOP OVER ENTIRE MONTE CARLO SIMULATIONS   */
 /*                                                         */
 /***********************************************************/

  /* 05/24/05 gam; always finishPeriodicTable table here */
  if (params->outputEventFlag > 0) {
      fprintf( params->xmlStream->fp, STACKSLIDE_XML_TABLE_FOOTER );
      params->xmlStream->table = no_table;
  }
  
  /* 05/24/05 gam */  
  if (reportMCResults) {
    if (params->outputEventFlag > 0) {
       /* write to the StackSlide Monte Carlo Results Table. */
       StackSlideMonteCarloResultsTable *stksldMonteCarloResultsTable;
       stksldMonteCarloResultsTable = (StackSlideMonteCarloResultsTable *) LALCalloc( 1, sizeof(StackSlideMonteCarloResultsTable) );
            
       /* print the table header */
       fprintf( params->xmlStream->fp, LIGOLW_XML_LOCAL_SEARCHRESULTS_STACKSLIDEMONTECARLO );

       /* print out the rows */
       for(iMC=0;iMC<countMC;iMC++) {
          stksldMonteCarloResultsTable->loudest_event = priorLoudestEvent; /* from prior jobs in the pipeline */
          stksldMonteCarloResultsTable->start_freq    = f0SUM;             /* saved value MC actually ran on  */
          stksldMonteCarloResultsTable->band          = bandSUM;           /* saved value MC actually ran on  */
          stksldMonteCarloResultsTable->upper_limit   = arrayULs[iMC];
          stksldMonteCarloResultsTable->confidence    = arrayConfs[iMC];
          stksldMonteCarloResultsTable->converged     = arrayConverged[iMC];
          stksldMonteCarloResultsTable->next          = NULL;
          if (iMC > 0) {
                 fprintf( params->xmlStream->fp, ",\n" ); /* end previous row with a comma */
          }
          fprintf( params->xmlStream->fp, LOCAL_SEARCHRESULTS_STACKSLIDEMONTECARLO_ROW,
            stksldMonteCarloResultsTable->loudest_event,
            stksldMonteCarloResultsTable->start_freq,
            stksldMonteCarloResultsTable->band,
            stksldMonteCarloResultsTable->upper_limit,
            stksldMonteCarloResultsTable->confidence,
            stksldMonteCarloResultsTable->converged
          );
       } /* END for(iMC=0;iMC<countMC;iMC++) */

       /* End the table: */
       fprintf( params->xmlStream->fp, STACKSLIDE_XML_TABLE_FOOTER );
       params->xmlStream->table = no_table;
       LALFree(stksldMonteCarloResultsTable);
    } /* END if (params->outputEventFlag > 0) */
    #ifdef INCLUDE_PRINT_REPORTMCRESULTS_CODE
      for(iMC=0;iMC<countMC;iMC++) {
        if ((params->debugOptionFlag & 32) > 0 ) {
              fprintf(stdout,"MC Trial #%i UL = %20.10e, Conf = %20.10f, Converged = %i\n",iMC,arrayULs[iMC],arrayConfs[iMC],arrayConverged[iMC]);
              fflush(stdout);
        }
      }
    #endif    
    LALFree(arrayULs);
    LALFree(arrayConfs);
    LALFree(arrayConverged);
  } /* END if (reportMCResults) */
  
  LALDestroyRandomParams(status->statusPtr, &randPar); /* 05/28/04 gam */
  CHECKSTATUSPTR (status);
  
  LALFree(pSFTParams);
      
  if (params->numSpinDown > 0) {
    LALDDestroyVector(status->statusPtr, &(pPulsarSignalParams->pulsar.spindown));
    CHECKSTATUSPTR (status);
  }
  LALFree(pPulsarSignalParams);

  /* 07/14/04 gam; deallocate memory for structs needed by LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs */
  if ( (params->testFlag & 4) > 0 ) {
    #ifdef DEBUG_LALFASTGENERATEPULSARSFTS
      fprintf(stdout,"deallocate memory for structs needed by LALComputeSkyAndZeroPsiAMResponse and LALFastGeneratePulsarSFTs \n");
      fflush(stdout);
    #endif
    LALDestroySFTVector(status->statusPtr, &outputSFTs);
    CHECKSTATUSPTR (status);
    LALFree(pSkyConstAndZeroPsiAMResponse->fCrossZeroPsi);
    LALFree(pSkyConstAndZeroPsiAMResponse->fPlusZeroPsi);
    LALFree(pSkyConstAndZeroPsiAMResponse->skyConst);
    LALFree(pSkyConstAndZeroPsiAMResponse);
    LALFree(pSFTandSignalParams->trigArg);
    LALFree(pSFTandSignalParams->sinVal);
    LALFree(pSFTandSignalParams->cosVal);
    LALFree(pSFTandSignalParams);
  }
          
  /* 05/21/04 gam; free memory of saved data */
  for (i=0;i<params->numBLKs;i++) {
      LALFree(savBLKData[i]->fft->data->data);
      LALFree(savBLKData[i]->fft->data);
      LALFree(savBLKData[i]->fft);
      LALFree(savBLKData[i]);
  }
  LALFree(savBLKData);
  
  /* deallocate memory for the savSkyPosData structure */
  for(i=0;i<numSkyPosTotal;i++)
  {
      LALFree(savSkyPosData[i]);
  }
  LALFree(savSkyPosData);
  
  if (params->numSpinDown > 0) {
    /* deallocate memory for the savFreqDerivData structure */
    for(i=0;i<numFreqDerivTotal;i++)
    {
        LALFree(savFreqDerivData[i]);
    }
    LALFree(savFreqDerivData);
  }
  
  CHECKSTATUSPTR (status);
  DETATCHSTATUSPTR (status);
#endif  
}
/*************************************************************/
/*                                                           */
/* END FUNCTION: RunStackSlideIsolatedMonteCarloSimulation   */
/* Injects fake signals and runs Monte Carlo simulation.     */
/*                                                           */
/*************************************************************/

/* 05/24/05 gam; Function that reads results from previous jobs in the pipeline */
void getStackSlidePriorResults(LALStatus *status,
                               REAL4 *priorLoudestEvent,
                               REAL8 *priorStartFreq,
                               REAL8 *priorBand,
                               REAL8 *priorConfidence,
                               REAL8 *priorUL,
                               REAL8 *priorUncertainty,
                               CHAR  *priorResultsFile)
{
  INT4 i;
  INT4 numWordsToIgnore = 43;
  CHAR buffer[256];
  FILE *fpPrior = NULL;
  
  INITSTATUS( status, "getStackSlidePriorResults", STACKSLIDEISOLATEDC );
  ATTATCHSTATUSPTR (status);
     
  fpPrior = fopen( priorResultsFile, "r");
  if (fpPrior == NULL) {
     ABORT( status, STACKSLIDEISOLATEDH_EBADRESULTSFILE, STACKSLIDEISOLATEDH_MSGEBADRESULTSFILE);
  }

  /* skip first numWordsToIgnore words */
  for(i=0;i<numWordsToIgnore;i++){
     fscanf(fpPrior,"%s",buffer);
  }
  /* data was written using matlab format string '%20.6f %20.10f %20.10f %8.2f %20.10e %20.10e\n' */
  fscanf(fpPrior,"%f%lf%lf%lf%le%le\n",priorLoudestEvent,priorStartFreq,priorBand,priorConfidence,priorUL,priorUncertainty);
  fclose(fpPrior);

  CHECKSTATUSPTR (status);
  DETATCHSTATUSPTR (status);
}
