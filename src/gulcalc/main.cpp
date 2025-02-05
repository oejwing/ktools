/*
* Copyright (c)2015 - 2016 Oasis LMF Limited
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*
*   * Neither the original author of this software nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
* THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*/
/*
Calculate the GUL
Author: Ben Matharu  email: ben.matharu@oasislmf.org
*/
#include <iostream>

#if defined(_MSC_VER)
#include "../wingetopt/wingetopt.h"
#else
#include <unistd.h>
#endif

#include "gulcalc.h"


// options
bool verbose = false;

char *progname;


void help()
{
	fprintf(stderr,
		"-S Sample size (default 0) \n"
		"-r use random number file\n"
		"-R [max random numbers] used to allocate array for random numbers default 1,000,000\n"
		"-c [output pipe] - coverage output\n"
		"-i [output pipe] - item output\n"
		"-d debug (output random numbers instead of gul)\n"
		"-s seed for random number generation (used for debugging)\n"
		"-A automatically hashed seed driven random number generation (default)\n"
		"-l legacy mechanism driven by random numbers generated dynamically per group - will be removed in future\n"
		"-L Loss threshold (default 0)\n"
		"-a alloc rule (default 0)\n"
		"-m execution mode (default 0) mode 1\n"
		"-v version\n"
		"-h help\n"
		);
}

int main(int argc, char *argv[])
{
	int opt;
	gulcalcopts gopt;
	gopt.loss_threshold = 0.000001;
	progname = argv[0];
	while ((opt = getopt(argc, argv, "Alvhdra:L:S:c:i:R:s:m:")) != -1) {
		switch (opt) {
		case 'S':
			gopt.samplesize = atoi(optarg);
			break;
		case 'l':
			gopt.rndopt = rd_option::usecachedvector;
			break;
		case 'A':
			gopt.rndopt = rd_option::usehashedseed;
			break;
		case 'a':
			gopt.mode = atoi(optarg);	// alloc rule and mode are the same thing
			break;
		case 'r':
			gopt.rndopt = rd_option::userandomnumberfile;
			break;
		case 'L':
			gopt.loss_threshold = atof(optarg);
			break;
		case 'R':
			gopt.rand_vector_size = atoi(optarg);
			break;
		case 'i':
			gopt.item_output = optarg;
			gopt.itemLevelOutput = true;
			break;
		case 'm':
			gopt.mode = atoi(optarg);
			break;
		case 'c':
			gopt.coverage_output = optarg;
			gopt.coverageLevelOutput = true;
			break;
		case 'd':
			gopt.debug = true;
			break;		
		case 's':
			gopt.rand_seed = atoi(optarg);
			break;
		case 'v':
			fprintf(stderr, "%s : version: %s\n", argv[0], VERSION);
			exit(EXIT_FAILURE);
			break;
		case 'h':
		default: /* '?' */
			help();
			exit(EXIT_FAILURE);
		}
	}
	
	if (gopt.itemLevelOutput == true) {
		if (gopt.item_output == "-") gopt.itemout = stdout;
		else gopt.itemout = fopen(gopt.item_output.c_str(), "wb");
	}
	if (gopt.coverageLevelOutput == true) {
		if (gopt.coverage_output == "-") gopt.covout = stdout;
		else gopt.covout = fopen(gopt.coverage_output.c_str(), "wb");
	}

	if (gopt.itemLevelOutput == false && gopt.coverageLevelOutput == false) {
		fprintf(stderr, "%s: No output option selected\n", argv[0]);
		exit(EXIT_FAILURE);
	}	

	if (gopt.mode > 1 || gopt.mode < 0) {
		fprintf(stderr, "%s: Invalid mode %d valid modes are 0 and 1\n", argv[0],gopt.mode);
		exit(EXIT_FAILURE);
	}

	try {
		initstreams();
		doit(gopt);
	}catch (std::bad_alloc) {
			fprintf(stderr, "%s: Memory allocation failed\n", progname);
			exit(EXIT_FAILURE);
	}

}
