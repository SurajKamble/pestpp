/*
� Copyright 2012, David Welter

This file is part of PEST++.

PEST++ is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PEST++ is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PEST++.  If not, see<http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "RunStorage.h"
#include "utilities.h"

using namespace std;

void usage(ostream &fout)
{
	fout << "--------------------------------------------------------" << endl;
	fout << "usage:" << endl << endl;
	fout << "    pbin2ascii.exe ext_file  runs_file  par_prefix" << endl << endl;
	fout << "    where:" << endl;
	fout << "        ext_file:   name of the pest++ .ext file containing the name binary run file" << endl;
	fout << "        runs_file:  name of file to be created with list of required runs" << endl;
	fout << "        par_prefix: prefix used to create parameter files" << endl;
	fout << "--------------------------------------------------------" << endl;
}

int main(int argc, char* argv[])
{
	string ext_filename = argv[1];
	string out_filename = argv[2];
	string base_par_filename = argv[3];

	if (argc != 4)
	{
		cerr << "incorect number of command line arguements" << endl << endl;
		usage(cerr);
		throw PestError("Error: incorect number of command line arguements");
	}


	string pbin_filename;
	try
	{
		ifstream fin_ext(ext_filename);
		getline(fin_ext, pbin_filename);
		pest_utils::strip_ip(pbin_filename);
	}
	catch (exception &e)
	{
		cerr << "Error processing external run manager *.ext \"" << pbin_filename << "\"";
		cerr << e.what() << endl;
		usage(cerr);
		throw(e);
	}


	RunStorage rs("");
	try
	{
		rs.init_restart(pbin_filename);
	}
	catch (exception &e)
	{
		cerr << "Error processing PEST++ binary file \"" << pbin_filename << "\"";
		cerr << e.what() << endl;
		usage(cerr);
		throw(e);
	}

	vector<string> par_name_vec = rs.get_par_name_vec();
	vector<string> obs_name_vec = rs.get_obs_name_vec();

	ofstream fout(out_filename);

	int n_runs = rs.get_nruns();
	cout << "processing " << n_runs << " runs" << endl;

	int status = 0;
	string info_text;
	double info_value;
	vector<double> pars_vec;
	vector<double> obs_vec;
	for (int i = 0; i < n_runs; ++i)
	{
		obs_vec.clear();
		pars_vec.clear();
		rs.get_info(i, status, info_text, info_value);
		if (status == 0)
		{
			rs.get_run(i, pars_vec, obs_vec);

			stringstream par_file_name;
			par_file_name << base_par_filename << "_" << i << ".par";
			ofstream fout_par;
			fout_par.open(par_file_name.str());
			assert(par_name_vec.size() == pars_vec.size());
			size_t n_par = par_name_vec.size();
			for (size_t ipar = 0; ipar < n_par; ++ipar)
			{
				fout_par << par_name_vec[ipar] << " ";
				int prec = fout_par.precision(numeric_limits<double>::digits10 + 1);
				fout_par << pars_vec[ipar] << endl;
				fout_par.precision(prec);
			}
			fout_par.close();
			fout << i << " " << par_file_name.str() << endl;
		}
	}
	fout.close();
}