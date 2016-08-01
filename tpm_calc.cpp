#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h> 
#include <random>

using namespace std;

struct meta{
	vector<string> headers;
	vector<string> annotation;
	vector<double> scale;
};
meta head;

vector<vector<double> > load_counts(char* filename, int start_size, int anno_col, int start_col){
	
	vector<vector<double> > push_matrix;
	vector<double> sum_scale;
	ifstream counts_file(filename);
	string line;
	
	cerr << "Loading counts object to memory" << "\n";	
	if (counts_file.is_open()){
		int n = 0, col = 0;
		while(getline(counts_file,line)){
			istringstream iss(line);
			string token;
			vector<string> string_holder;
			while(getline(iss,token,'\t')){
				if ((n == 0) && (col >= start_col - 1)) {
					head.headers.push_back(token);
					head.scale.push_back(0);
				}else
					string_holder.push_back(token);
				col++;	
			}		
			if(n > 0){
				head.annotation.push_back(string_holder[anno_col-1]);
				double sizer = (std::stod(string_holder[start_size]) - std::stod(string_holder[start_size-1])) / 1000;
				vector<double> struct_holder;
				int sum_index = 0;
				for (int i = start_col - 1; i < string_holder.size(); i++){
					double holder;
					holder = std::stod(string_holder[i]) / sizer; 
					struct_holder.push_back(holder);
					head.scale[sum_index] += holder;
					sum_index++;
				}
				push_matrix.push_back(struct_holder);
			}
			n++;
		}
		counts_file.close();
		for(x : head.scale){
			x /= 1000000;
		}
		return(push_matrix);
	}else{
		cerr << "Count files unsuccessfully loaded to memory. Check path to file. Exiting" << "\n";
		exit(-1);
	}
	
}


void write_counts(std::vector<std::vector<double>> &counts_matrix, char* tpm_loc, char* logtpm_loc, auto head){

	ofstream tpm;
	ofstream logtpm;
	tpm.open(tpm_loc);
	logtpm.open(logtpm_loc);

	vector<double> min_val;
	for (x : head.headers)
		min_val.push_back(1000000);
	
	cout << "Writing headers\n";
	tpm << "annotation" << "\t";
	logtpm << "annotation" << "\t";
	for (int i = 0; i < head.headers.size(); i++){
		if (i < head.headers.size()-1){
			tpm << head.headers[i] << "\t";
			logtpm << head.headers[i] << "\t";
		}else{
			tpm << head.headers[i];
			logtpm << head.headers[i];
		} 
	} 
	
	cout << "Writing TPM values\n";
	for(int i = 0; i < head.annotation.size(); i++){
		tpm << "\n" << head.annotation[i];
		for (int x = 0; x < counts_matrix[i].size(); x++){
			double tval = counts_matrix[i][x] / head.scale[x];
			tpm << "\t" << tval;
			if(tval > 0){
				if(tval < min_val[x])
					min_val[x] = tval;
			}
		}
	}
	tpm.close();
	
	cout << "Writing log2 TPM values\n";
	std::mt19937 generator;
	for(int i = 0; i < head.annotation.size(); i++){
		logtpm << "\n" << head.annotation[i];
		for (int x = 0; x < counts_matrix[i].size(); x++){
			double tval = counts_matrix[i][x] / head.scale[x];
			if (tval > 0)
				logtpm << "\t" << log2(tval);
			else{
				std::normal_distribution<double> normal(min_val[x] - 2, 1);
				logtpm << "\t" << log2(normal(generator));
			}
		}
	}
	logtpm.close();
		
}

int main (int argc, char *argv[]){
	
	// usage
	// ./tpm-calc begin-size-column annotation-column start-column counts-file tpm-output logtpm-output
	std::string helper = argv[1];
	if (helper == "--help")
		cout << "./tpm-calc begin-size-column annotation-column start-column counts-file tpm-output logtpm-output" << "\n";
	else {
		vector<vector<double> > counts_matrix = load_counts(argv[4], atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
		write_counts(counts_matrix, argv[5], argv[6], head);
	}
	
}



