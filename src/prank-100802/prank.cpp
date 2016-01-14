/***************************************************************************
 *   Copyright (C) 2005 by Ari Loytynoja   *
 *   ari@ebi.ac.uk   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "progressivealignment.h"
#include "prank.h"

using namespace std;

int main(int argc, char *argv[])
{

    readArguments(argc, argv);
	int time1 = time(0);

    ProgressiveAlignment* pa = new ProgressiveAlignment(treefile,seqfile,dnafile);
    if (NOISE>=0)
		cout<<endl<<"Alignment done. Total time "<<(time(0)-time1)<<"s"<<endl;

    delete pa;
    delete hmm;

    cout<<endl;
    exit(0);
}


void readArguments(int argc, char *argv[])
{
    int seed = -1;


    // first see if 'noise' is defined.
    for (int i=1;i<argc;i++)
    {

        string s = string(argv[i]);

        // no debugging info
        if (s=="-quiet")
        {
            NOISE = -1;
            SCREEN = false;
        }
        // debugging info level
        else if (s.substr(0,7)=="-noise=")
        {
            NOISE = atoi(string(argv[i]).substr(7).c_str());
            SCREEN = false;
        }
    }

    // one argument only; should be sequence file!
    if (argc==2 && string(argv[1]).substr(0,1)!="-")
    {
        seqfile = string(argv[1]);
    }

    // otheriwise run in the normal manner
    else
    {
        for (int i=1;i<argc;i++) {

            string s = string(argv[i]);

            if (s=="-quiet")
            {
                // handled earlier
            }

            // debugging info level
            else if (s.substr(0,7)=="-noise=")
            {
                NOISE = atoi(string(argv[i]).substr(7).c_str());
                SCREEN = false;
            }

            // print help
            else if (s=="-help") {
                printHelp(true);
                exit(0);
            }

            /********* input/output: **********/

            // sequence data file
            else if (s.substr(0,3)=="-d=")
            {
                seqfile = string(argv[i]).substr(3);
            }

            // guide tree file
            else if (s.substr(0,3)=="-t=")
            {
                treefile = string(argv[i]).substr(3);
                TWICE = false;
            }

            // alignment output file
            else if (s.substr(0,3)=="-o=")
            {
                outfile = string(argv[i]).substr(3);
            }

            // structure model file
            else if (s.substr(0,3)=="-m=")
            {
                hmmname = string(argv[i]).substr(3);
                hmm = new HMModel();
                hmm->readModel(hmmname.c_str());
                HASHMM = true;
            }

            // guide tree as a string
            else if (s.substr(0,6)=="-tree=")
            {
                treefile = string(argv[i]).substr(6).c_str();
                TREESTRING = true;
                TWICE = false;
            }

            // mixture of existing and new alignments for Ziheng
            else if (s=="-partaligned")
            {
                PARTLYALIGNED = true;
            }

            // pre-aligned data, just reconstruct it (using a model if specified)
            else if (s=="-e")
            {
                PREALIGNED = true;
            }

            // backtranslate existing protein alignment to DNA
            else if (s.substr(0,5)=="-dna=")
            {
                dnafile = s.substr(5);
                BACKTRANSLATE = true;
            }

            /********* more input/output: **********/

            // estimate guide tree from input alignment (before realignment)
            else if (s=="-njtree")
            {
                TREEFROMALIGNMENT = true;
            }

            // output alignment format
            else if (s.substr(0,11)=="-outformat=")
            {
                string tmp = string(argv[i]).substr(11);
                format = parseFormat(tmp);
            }

            // output alignment format
            else if (s.substr(0,3)=="-f=")
            {
                string tmp = string(argv[i]).substr(3);
                format = parseFormat(tmp);
            }

            // do output backtabs
            else if (s=="-cute")
            {
                SCREEN = true;
            }

            // reporting interval
            else if (s.substr(0,4)=="-rl=")
            {
                reportLimit = atoi(string(argv[i]).substr(4).c_str());
            }

            // write reconstructed ancestral seqs
            else if (s=="-writeanc")
            {
                WRITEANC = true;
            }

            // write ancestral nodes as they are solved
            else if (s=="-printnodes")
            {
                PRINTNODES = true;
            }

            // don't print tree
            else if (s=="-notree")
            {
                PRINTTREE = false;
            }

            // don't write xml
            else if (s=="-noxml")
            {
                WRITEXML = false;
            }

            // print dots for insertions
            else if (s=="-dots" || s=="-esko")
            {
                DOTS = true;
            }

            // no align, convert only
            else if (s=="-convert")
            {
                CONVERT = true;
            }

            // use short names (until first space)
            else if (s=="-shortnames")
            {
                SHORTNAMES = true;
            }

            /********* model: gaps and F **********/

            // keep insertion forever
            else if (s=="-F" || s=="+F")
            {
                FOREVER = true;
                SKIPGAPANCH = true;
            }

            // not needed but still allowed option
            else if (s=="-no-F")
            {
                FOREVER = false;
            }

            // old implementation
            else if (s=="-F_old")
            {
                FOREVER_OLD = true;
                SKIPGAPANCH = true;
            }

            /********* model: substitutions, indels **********/

            else if (s.substr(0,10)=="-dnafreqs=")
            {
                dnaFreqs = string(argv[i]).substr(10);
            }

            else if (s.substr(0,9)=="-gaprate=")
            {
                gapRate = atof(string(argv[i]).substr(9).c_str());
            }

            else if (s.substr(0,8)=="-gapext=")
            {
                gapExt = atof(string(argv[i]).substr(8).c_str());
            }

            else if (s.substr(0,7)=="-kappa=")
            {
                kappa = atof(string(argv[i]).substr(7).c_str());
            }

            else if (s.substr(0,5)=="-rho=")
            {
                rho = atof(string(argv[i]).substr(5).c_str());
            }

            /********* model: other **********/

            // codon alignment
            else if (s=="-codon")
            {
                CODON = true;
            }

            // force dna alignment
            else if (s=="-DNA")
            {
                DNA = true;
            }

            // force protein alignment
            else if (s=="-protein")
            {
                PROTEIN = true;
            }

            // no posterior probabiliity calculation
            else if (s=="-nopost")
            {
                DOPOST = false;
            }

            // penalise terminal gaps
            else if (s=="-termgap")
            {
                NOTGAP = false;
            }

            else if(s=="-nomissing")
            {
                TERMF = true;
            }

            // run once
            else if (s=="-once")
            {
                TWICE = false;
            }

            // run twice
            else if (s=="-twice")
            {
                TWICE = true;
            }

            // prune the tree
            else if (s=="-prunetree")
            {
                PRUNETREE = true;
            }

            // don't use log values (slightly faster)
            else if (s=="-nologs")
            {
                LOGVALUES = false;
            }

            // use log values (slightly slower)
            else if (s=="-uselogs")
            {
                LOGVALUES = true;
            }

            // seed for random number generator
            else if (s.substr(0,6)=="-seed=")
            {
                seed = atoi(string(argv[i]).substr(6).c_str());
            }


            /********* more model: **********/

            // translate DNA to protein, then backtranslate
            else if (s=="-translate")
            {
              TRANSLATE = true;
            }

            // translate mtDNA to protein, then backtranslate
            else if (s=="-mttranslate")
            {
              TRANSLATE = true;
              MTTABLE = true;
            }

            // consider N or X identical to any
            else if (s=="-NX")
            {
                NXis1 = true;
            }

            // split probbailities for N and X
            else if (s=="-splitNX")
            {
                NXis1 = false;
            }

            // priors file (not really working)
            else if (s.substr(0,3)=="-p=")
            {
                annofile = string(argv[i]).substr(3);
                PRIORS= true;
            }


            /********* more model: pairwise alignment for guide tree **********/

            // expected pairwise distance
            else if (s.substr(0,8)=="-pwdist=")
            {
                pwDist = atof(string(argv[i]).substr(8).c_str());;
            }

            // expected pairwise distance
            else if (s.substr(0,11)=="-pwdnadist=")
            {
                pwDnaDist = atof(string(argv[i]).substr(11).c_str());;
            }

            /********* more model: branch lengths in guide tree **********/

            // scale branch lengths
            else if (s.substr(0,15)=="-scalebranches=")
            {
                branchScalingFactor = atof(string(argv[i]).substr(15).c_str());;
            }

            // set branch lengths
            else if (s.substr(0,15)=="-fixedbranches=")
            {
                fixedBranchLength = atof(string(argv[i]).substr(15).c_str());;
                FIXEDBRANCH = true;
            }

            // set branch lengths
            else if (s.substr(0,13)=="-maxbranches=")
            {
                fixedBranchLength = atof(string(argv[i]).substr(13).c_str());;
                MAXBRANCH =true;
            }

            // set branch lengths
            else if (s.substr(0,13)=="-maxpairdist=")
            {
                dnaMaxPairwiseLength = atof(string(argv[i]).substr(13).c_str());
                protMaxPairwiseLength = atof(string(argv[i]).substr(13).c_str());
            }

            // use real guidetree distances
            else if (s=="-adjustmodel")
            {
                ADJUSTMODEL = true;
            }

            // use real guidetree distances
            else if (s=="-noadjustmodel")
            {
                ADJUSTMODEL = false;
            }

            // use real guidetree distances
            else if (s=="-realbranches")
            {
                REALBRANCHES = true;
            }

            // correct guidetree distances
            else if (s=="-correctp")
            {
              CORRECTP = true;
            }

            /********* technical: hirschberg, full probability **********/

            // "band" full probability (less memory)
            else if (s=="-fb")
            {
                FULLBAND = true;
            }

            // complete full probability
            else if (s=="-ff")
            {
                FULLFULL = true;
            }

            // hirschberg band width (for hirschbergalignment)
            else if (s.substr(0,5)=="-hbw=")
            {
                HBW = atoi(string(argv[i]).substr(5).c_str());
            }

            // full probability band width (for fullprobability)
            else if (s.substr(0,5)=="-fbw=")
            {
                FBW = atoi(string(argv[i]).substr(5).c_str());
            }

            // skip insertions in postprobs
            else if (s=="-skipins")
            {
                SKIPINS = true;
            }

            /********* technical: anchoring **********/

            // use anchors
            else if (s.substr(0,3)=="-a")
            {
                ANCHORS = true;
            }

            // maximum anchor distance
            else if (s.substr(0,6)=="-maxd=")
            {
                maxAnchDist = atoi(string(argv[i]).substr(6).c_str());
            }

            // minimum anchor distance
            else if (s.substr(0,6)=="-mind=")
            {
                minAnchDist = atoi(string(argv[i]).substr(6).c_str());
            }

            // anchor skip distance
            else if (s.substr(0,7)=="-skipd=")
            {
                anchSkipDist = atoi(string(argv[i]).substr(7).c_str());
            }

            // anchor drop distance
            else if (s.substr(0,7)=="-dropd=")
            {
                anchDropDist = atoi(string(argv[i]).substr(7).c_str());
            }

            // don't infer gaps caused by missing data
            else if (s=="-nopatchdata")
            {
                PATCHMISSING = false;
            }

            // length of gap deemed as missing data
            else if (s.substr(0,11)=="-misslimit=")
            {
                missingLimit = atoi(string(argv[i]).substr(11).c_str());
            }

            // skip gaps in anchoring ancestral seqs (?)
            else if (s=="-gapanch")
            {
                if (!FOREVER)
                    SKIPGAPANCH = false;
                else
                    cout<<endl<<"unaccepted combination: -F -gapanch; disabling -gapanch"<<endl;
            }

            /********* technical: memory & speed efficiency **********/

            // matrix resize factor
            else if (s.substr(0,11)=="-matresize=")
            {
                resizeFactor = atof(string(argv[i]).substr(11).c_str());
            }

            // matrix initial factor
            else if (s.substr(0,13)=="-matinitsize=")
            {
                initialMatrixSize = atof(string(argv[i]).substr(13).c_str());
            }

            // use pwmatrix maximum size
            else if (s=="-pwmatmax")
            {
                PWMATRIXMAXSIZE = true;
            }

            // use pwmatrix maximum size
            else if (s=="-longseq")
            {
                PWMATRIXMAXSIZE = false;
            }


            /********* pairwise genomic alignment **********/

            // ``good'' paarmeters for pw genomic alignment
            else if (s=="-pwgenomic")
            {
                treefile="dummy";
                PWGENOMIC = true;
                gapRate = 0.1;
                gapExt = 0.9;
                TWICE = false;
            }

            // parameters for pw genomic alignment
            else if (s.substr(0,15)=="-pwgenomicdist=")
            {
                pwgendist = atof(string(argv[i]).substr(15).c_str());
            }

            /************************************************/

            else
            {
                cout<<"Unknown option: "<<s<<endl<<endl;
                printHelp(false);
                exit(0);
            }
        }
    }

    if (seqfile=="") {
        printHelp(false);
        exit(0);
    }

    // define a seed for random numbers
    if(seed>0)
        srand(seed);
    else
        srand(time(0));

    // partly aligned switches between F/non-F
    // and has to remember the settings
    if(FOREVER && PARTLYALIGNED)
        FOREVER_FOR_PA = true;
    else
        FOREVER_FOR_PA = false;

    // remind that +F is not the default option
    if (!FOREVER && !PREALIGNED && !CONVERT && !BACKTRANSLATE){
      DOTS=false;

      cout<<endl<<"Note: option '+F' is by default not selected.";
      if(argc==2 && seqfile!="")
        cout<<" You can select it with command \"prank +F -d="<<seqfile<<"\"."<<endl;
      else
        cout<<" You can select it by adding flag \"+F\"."<<endl;
    }

    // options don't work together
    if (PREALIGNED)
      FOREVER=false;

    if (format!=8 && format!=11 && format!=12 && format!=17 && format!=18)
        format = 8;

}


void printHelp(bool complete)
{
    cout<<endl<<"prank v.100802. ";
    cout<<"Minimal usage: 'prank sequence_file'"<<endl<<endl;;
    cout<<"Advanced usage: 'prank [optional parameters] -d=sequence_file [optional parameters]'"<<endl;;
    cout<<"\n input/output parameters:"<<endl;
    cout<<"  -d=sequence_file (in FASTA format)"<<endl;
    cout<<"  -t=tree_file [default: no tree, generate approximate NJ tree]"<<endl;
    if(complete)
        cout<<"  -tree=\"tree_string\" [tree in newick format; in double quotes]"<<endl;
    cout<<"  -m=model_file [default: HKY2/WAG]"<<endl;
    cout<<"  -o=output_file [default: 'output']"<<endl;
    cout<<"  -f=output_format ['fasta' (default), 'phylipi', 'phylips', 'paml', 'nexus']"<<endl;
    if(complete)
    {
        cout<<"  -f=output_format_number [default: 8] (deprecated; use format names)"<<endl;
        cout<<"     8. Pearson/Fasta"<<endl;
        cout<<"    11. Phylip sequential"<<endl;
        cout<<"    12. Phylip interleaved"<<endl;
        cout<<"    17. PAUP/NEXUS"<<endl;
        cout<<"    18. PAML"<<endl;
        cout<<"  -noxml [do not output xml-files]"<<endl;
        cout<<"  -notree [do not output dnd-files]"<<endl;
    }
    cout<<"  -shortnames [truncate names at first space]"<<endl;
    cout<<"  -quiet"<<endl;
    cout<<"\n model parameters:"<<endl;
    cout<<"  +F [force insertions to be always skipped]"<<endl;
    cout<<"  -F [equivalent]"<<endl;
    if(complete)
        cout<<"  -dots [show insertion gaps as dots]"<<endl;
    cout<<"  -gaprate=# [gap opening rate; default: dna "<<dnaGapRate<<" / prot "<<protGapRate<<"]"<<endl;
    cout<<"  -gapext=# [gap extension probability; default: dna "<<dnaGapExt<<" / prot "<<protGapExt<<"]"<<endl;
    if(complete)
    {
        cout<<"  -dnafreqs=#,#,#,# [ACGT; default: empirical]"<<endl;
        cout<<"  -kappa=# [ts/tv rate ratio; default:"<<kappa<<"]"<<endl;
        cout<<"  -rho=# [pur/pyr rate ratio; default:"<<rho<<"]"<<endl;
   }
    cout<<"  -codon [for coding DNA: use empirical codon model]"<<endl;
    cout<<"  -DNA / -protein [no autodetection: use dna or protein model]"<<endl;
    cout<<"  -termgap [penalise terminal gaps normally]"<<endl;
    cout<<"  -nomissing [no missing data, use -F for terminal gaps ]"<<endl;
    cout<<"  -e [pre-aligned sequences; do not remove gaps]"<<endl;
    cout<<"\n other parameters:"<<endl;
    cout<<"  -nopost [do not compute posterior support; default: compute]"<<endl;
    if(complete)
        cout<<"  -pwdist=# [expected pairwise distance for computing guidetree; default: dna "<<pwDnaDist<<" / prot "<<pwProtDist<<"]"<<endl;
    cout<<"  -once [run only once; default: twice if no guidetree given]"<<endl;
    cout<<"  -twice [run always twice]"<<endl;
    cout<<"  -prunetree [prune guide tree branches with no sequence data]"<<endl;

    if(complete)
        cout<<"  -skipins [skip insertions in posterior support]"<<endl;
    cout<<"  -uselogs [slower but should work for a greater number of sequences]"<<endl;
    if(complete)
    {
        cout<<"  -writeanc [output ancestral sequences]"<<endl;
        cout<<"  -printnodes [output each node; mostly for debugging]"<<endl;
        cout<<"  -matresize=# [matrix resizing multiplier]"<<endl;
        cout<<"  -matinitsize=# [matrix initial size multiplier]"<<endl;
        cout<<"  -longseq [save space in pairwise alignments]"<<endl;
        cout<<"  -pwgenomic [do pairwise alignment, no guidetree]"<<endl;
        cout<<"  -pwgenomicdist=# [distance for pairwise alignment; default: "<<pwgendist<<"]"<<endl;
        cout<<"  -scalebranches=# [scale branch lengths; default: dna "<<dnaBranchScalingFactor<<" / prot "<<protBranchScalingFactor<<"]"<<endl;
        cout<<"  -fixedbranches=# [use fixed branch lengths]"<<endl;
        cout<<"  -maxbranches=# [set maximum branch length]"<<endl;
        cout<<"  -realbranches [disable branch length truncation]"<<endl;
        cout<<"  -seed=# [set random number seed]"<<endl;
    }
    cout<<"  -translate [translate to protein]"<<endl;
    cout<<"  -mttranslate [translate to protein using mt table]"<<endl;
    cout<<"  -maxpairdist=# [maximum pairwise distance for matrix computation]"<<endl;

    cout<<"\n other:"<<endl;
    cout<<"  -convert [no alignment, just convert to another format]"<<endl;
    cout<<"\n  -help [show more options]"<<endl;

    cout<<""<<endl;
}

int parseFormat(string Format)
{
    string format = "";
    for(unsigned int i = 0; i < Format.size(); i++)
    {
        format += tolower(Format[i]);
    }

    if(format == "fasta" || format == "8")
        return 8;
    else if(format == "phylipi" || format == "phylip" || format == "12")
        return 12;
    else if(format == "phylips" || format == "11")
        return 11;
    else if(format == "nexus" || format == "paup" || format == "17")
        return 17;
    else if(format == "paml")
        return 18;
    else
    {
        cout<<"Warning: output format not recognized, using FASTA.\n";
        return 8;
    }
}
