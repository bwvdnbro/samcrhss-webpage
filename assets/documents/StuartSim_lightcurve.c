
/* This is a simple code to illustrate lightcurve calcualtions in homologous explosion */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gsl/gsl_rng.h>

#define CLIGHT 2.99792458e10 /* speed of light in cm/s */
#define DAY 86400.0 /* seconds in a day */


gsl_rng *rng; /* pointer for random number generator */
unsigned long int zseed; /* rnum generator seed */

/* To compute a new radius following the propagation of a packet for a time = time_prop */
  double
find_new_r (old_r, old_mu, time_prop)
  double old_r, old_mu, time_prop;
{   
  double new_r;
  new_r = sqrt( old_r*old_r + (CLIGHT*CLIGHT*time_prop*time_prop) + (2.*time_prop*CLIGHT*old_r*old_mu));
  return(new_r);
}

/* To compute a new direction cosine following the propagation of a packet for a time = time_prop */
  double
find_new_mu (old_r, new_r, time_prop)
  double old_r, new_r, time_prop;
{   
  double new_mu;
  new_mu = (old_r*old_r - (time_prop*time_prop*CLIGHT*CLIGHT) - (new_r*new_r)) / (-2. * new_r * time_prop*CLIGHT);
  return(new_mu);
}


/* main */

  int
main()
{
  int npackets;
  int n, nts;
  double zrand; 
  double rho_ref, t_ref, current_rho;
  double vmax, vsource;
  double kappa;
  double time_to_boundary, time_to_nts, time_to_interaction;
  double new_r, new_mu;
  char filename[100];

  /* some additional counters and flags */
  int escaped; /* initialise to 0; set to 1 to identify an escaped packet*/
  int active; /* 1 means still active in this time step, 0 means ready for next time step */

  /* file pointer to the packet information file */
  FILE *info_file;
  FILE *out_file;

  /* Time step stuff */
  int ntimesteps;
  double start_time, end_time, delta_t;
  double *t_start, *t_end;

  /* Packets details */
  double *this_energy;
  double *this_time_injected;
  double *this_r_injected;
  double *this_mu_injected;
  int *this_done;
  double *this_r;
  double *this_mu;
  double *this_t_current;

  /*set numerical parameters */
  npackets = 1000000;
  ntimesteps = 100;

  /* allocate arrays of packet properties */
  this_energy = malloc(npackets * sizeof(double));/* current energy for packet */
  this_time_injected = malloc(npackets * sizeof(double)); /* time when packet will be injected (activates)*/
  this_r_injected = malloc(npackets * sizeof(double)); /* radius where packet will be injected (activates)*/
  this_mu_injected = malloc(npackets * sizeof(double)); /* (observer frame) direction (cosine) where with which packet will be injected (activates)*/
  this_done =  malloc(npackets *sizeof(int)); /* -1 means not yet active; 0 means active; 1 means finished [escaped] */
  this_r = malloc(npackets * sizeof(double)); /* current radial position of active packet */
  this_mu = malloc(npackets * sizeof(double)); /* current (observer frame) direction (cosine) of active packet */
  this_t_current = malloc(npackets * sizeof(double)); /* current time for active packet */

  /* arrays to store the time step boundaries */
  t_start = malloc(ntimesteps *sizeof(double));
  t_end = malloc(ntimesteps *sizeof(double));

  /* define model */
  vmax = 0.03 * CLIGHT; //outer boundary velocity
  vsource = vmax / 2.; //outer boundary of energy source region
  rho_ref = 1.e-6;  //reference value of density [g / cm^3]
  t_ref = 10000; //time associated with reference density [s]
  kappa = 0.1; //opacity co-efficient [tau = kappa * rho * path_length]
  start_time = 2 * DAY; //when to start simulation [s]
  end_time = 100 * DAY; //when to stop simulation [s]

  /* start by setting up the randon number generator */
  zseed = 69387;
  rng = gsl_rng_alloc (gsl_rng_ran3);
  gsl_rng_set ( rng, zseed);


  /* Set up packet initial properties. 
     Packets need an energy, a time when they are injected, a position where they are injected and an initial direction. */
  for (n = 0; n < npackets; n++)
  {
    /* assign all packets equal energy -  might as well normalise! */
    this_energy[n] = 1./npackets;
    /* select packet injection time - based on exponential law */
    zrand = gsl_rng_uniform(rng) * (1. - exp((start_time - end_time)/(113.7*DAY)));
    this_time_injected[n] = start_time - log(1. - zrand)*113.7*DAY;
    /* select radius of injection - randomly inside source volume for the injection time */
    zrand = gsl_rng_uniform(rng);
    this_r_injected[n] = vsource*this_time_injected[n] * pow(zrand, 1./3);
    /* chose direction - isotropic in the fluid frame... */
    zrand = gsl_rng_uniform(rng);
    new_mu = (2.*zrand) - 1.;
    /* ...use frame transformation to get value in oberver frame */
    this_mu_injected[n] = (new_mu + (this_r_injected[n] / (CLIGHT * this_time_injected[n])))/(1. + (new_mu * this_r_injected[n] / (CLIGHT * this_time_injected[n])));
    /* set flag appropriate for packet that has not started to propagate yet*/
    this_done[n] = -1;

    /* Prepare packets with their initial values for launching.*/
    this_t_current[n] = this_time_injected[n];
    this_r[n] = this_r_injected[n];
    this_mu[n] = this_mu_injected[n];
  }

  /* Now set up logarithmic time grid */
  delta_t = (log(end_time) - log(start_time)) / ntimesteps;
  for (n = 0; n < ntimesteps; n++)
  {
    t_start[n]=start_time * exp(n * delta_t);
    t_end[n] = start_time * exp((n + 1) * delta_t);
  }


  /* Run calcualtion: sequence of time steps with loop over packets */

  for (nts = 0; nts < ntimesteps; nts++)
  {
    current_rho = rho_ref * pow( (2. * t_ref / (t_start[nts] + t_end[nts])), 3.0);
    escaped = 0;
    
    for (n = 0; n < npackets; n++)
    {    
      /* First check if packet is active yet, but not done - if not, can skip */
      if (this_time_injected[n] < t_end[nts] && this_done[n] != 1)
      {
	/* set the status of the packet to say that it is in the propagation phase */
	this_done[n] = 0;
	/* Mark is as active: this used to break the propagation "while" loop, below: */
	active = 1;

	/* Now the main while loop that governs the propagation of a packet during a time step */
	while (active == 1)
	  {
	    /* Calculate three limiting distances: to boundary; to end of time step; to interaction */
	    /* Time for packet to reach intersection with grid boundary */
	    time_to_boundary = (sqrt( pow( (vmax * vmax * this_t_current[n] - this_r[n] * CLIGHT * this_mu[n]), 2) - ((CLIGHT * CLIGHT - vmax * vmax) * (this_r[n] * this_r[n] - vmax * vmax * this_t_current[n] * this_t_current[n]))) -  this_r[n] * CLIGHT * this_mu[n] + vmax * vmax * this_t_current[n]) / (CLIGHT * CLIGHT - vmax * vmax);
	    /* Time for packet to reach end of this time step */
	    time_to_nts = t_end[nts] - this_t_current[n];
	    /* Time until randomly selected interaction optical depth is reached (need random number first) */
	    zrand = gsl_rng_uniform(rng);
	    time_to_interaction = -1. * log(zrand) / (kappa * current_rho * CLIGHT * (1. - (this_mu[n] * this_r[n] / (CLIGHT * this_t_current[n]))));

	    /* All three distances known */
	    /* Select shorted time */
	    if ((time_to_boundary < time_to_nts) && (time_to_boundary < time_to_interaction))
	      {
		/*it escapes - time to boundary is shortest */
		/* update time and calcualted the position and cosine at the new point */
		this_t_current[n] += time_to_boundary;
		new_r = find_new_r(this_r[n], this_mu[n], time_to_boundary);
		new_mu = find_new_mu(this_r[n], new_r, time_to_boundary);

		/* update position of packet (to boundary) and mark escaped*/
		this_r[n] = new_r;
		this_mu[n] = new_mu;
		/* increment counter for information */
		escaped += 1;
		/* set flag that this packet is finished with */
		this_done[n] = 1;
		active = 0;
	      }
	    else if (time_to_nts < time_to_interaction)
	      {
		/* runs out of time in this time step */
		/* update time and calcualted the position and cosine at the new point */
		this_t_current[n] += time_to_nts;
		new_r = find_new_r(this_r[n], this_mu[n], time_to_nts);
		new_mu = find_new_mu(this_r[n], new_r, time_to_nts);

		/* update position of packet to end of timestep*/
		this_r[n] = new_r;
		this_mu[n] = new_mu;
		active = 0;

	      }
	    else
	      /* time to interaction must be shortest - so interact! */
	      {
		/* update time and calculate the position and cosine at the new point */
		this_t_current[n] += time_to_interaction;
		new_r = find_new_r(this_r[n], this_mu[n], time_to_interaction);
		new_mu = find_new_mu(this_r[n], new_r, time_to_interaction);
	        /* set values at interaction point */
		this_r[n] = new_r;
		this_mu[n] = new_mu;
		/*  Now need to do interaction: */
		/* draw new random direction in fluid frame */
		zrand = gsl_rng_uniform(rng);
		new_mu = -1. + (2.*zrand);
		/* convery to observer frame mu */
		new_mu = (new_mu + (new_r / (CLIGHT * this_t_current[n])))/(1. + (new_mu * new_r / (CLIGHT * this_t_current[n])));
		/* update energy (conservation of energy in fluid frame */
		this_energy[n] = this_energy[n] * (1. - (this_mu[n] * new_r / (CLIGHT * this_t_current[n]))) / (1. - (new_mu * new_r / (CLIGHT * this_t_current[n])));
		/* set the post-interaction direction in the packet array */
		this_mu[n] = new_mu;
	      }
	    /* ends while loop over interactions for this packet */
	  }
	/*ends if loop for activation */
      }
      /* ends loop over packets */
    }

    printf("During timestep %d, %d packets escaped.\n", nts, escaped);

    sprintf(filename,"info_%.4d.out",nts);
    if ((info_file = fopen(filename, "w+")) == NULL){
      printf("Cannot open info file.\n");
      exit(0);
    }
    
    for (n = 0; n < npackets; n++)
      {
	fprintf(info_file, "%d %d %g %g %g %g %g %g\n", n, this_done[n], this_t_current[n]/DAY, this_energy[n], this_r[n], this_mu[n], this_time_injected[n]/DAY, this_r_injected[n]);
      }
    
    fclose(info_file);

    /*ends loop over timestep */
  }
	

  if ((out_file = fopen("packets.out", "w+")) == NULL){
    printf("Cannot open packets.txt.\n");
    exit(0);
  }

  for (n = 0; n < npackets; n++)
  {
    fprintf(out_file, "%d %d %g %g %g %g %g %g\n", n, this_done[n], this_t_current[n]/DAY, this_energy[n], this_r[n], this_mu[n], this_time_injected[n]/DAY, this_r_injected[n]);
  }

  fclose(out_file);


  free(this_energy);
  free(this_time_injected);
  free(this_r_injected);
  free(this_mu_injected);
  free(this_done);
  free(this_r);
  free(this_mu);
  free(this_t_current);
  free(t_start);
  free(t_end);

  return 0;
}
