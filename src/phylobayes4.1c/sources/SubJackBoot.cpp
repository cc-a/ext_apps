#include  "phylo.h"


int main(int argc, char* argv[])	{

	if (argc != 6)	{
		cerr << "jackboot datafile N1 N2 Nrep outfile\n";
		exit(1);
	}

	// Random::Random();
	string name = argv[1];
	int N1 = atoi(argv[2]);
	int N2 = atoi(argv[3]);
	int Nrep = atoi(argv[4]);
	string outfile = argv[5];
	
	MCParameters* mParam = new MCParameters();
	mParam->ReadDataFromFile(name);
	
	if ((N1+N2) > mParam->Nsite)	{
		cerr << "error : target size should be < Nsite\n";
		exit(1);
	}

	int* mask = new int[mParam->Nsite];
	int* index = new int[N1+N2];
	
	for (int rep=0; rep<Nrep; rep++)	{
		for (int i=0; i<mParam->Nsite; i++)	{
			mask[i] = 0;
		}
		Random::DrawFromUrn(index,N1+N2,mParam->Nsite);
		for (int i=0; i<N1; i++)	{
			mask[index[i]] = 1;
		}
		ostringstream s;
		s << outfile << rep << "train.ali";
		mParam->WriteDataToFileSiteMask(s.str(),mask);

		for (int i=0; i<mParam->Nsite; i++)	{
			mask[i] = 0;
		}
		for (int i=N1; i<N1+N2; i++)	{
			mask[index[i]] = 1;
		}
		ostringstream s2;
		s2 << outfile << rep << "test.ali";
		mParam->WriteDataToFileSiteMask(s2.str(),mask);
	}

}

