#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

// output filled in CSV to std out

// amount of tests in inCSV = amount of lines in exit and expected codes.txt!!!

struct CSVline{
	string id;
	string instruction;
	string status;
	string author;

	string print(){
		return id + "," + instruction + "," + status + "," + author;
	}

};

int main(int argc, char *argv[]){
	
	vector<CSVline> CSV;
	vector<string> expC;
	vector<string> exitC;


	// open csv in testbench/tests.CSV	
	ifstream inCSV (argv[1]);
		if(!inCSV.is_open()){
		cerr << "could not open tests.csv" << endl;
		exit(EXIT_FAILURE);
	}

	// open expectedcodes
		//testbench/expectedCodes.txt
	ifstream expectedCodes (argv[2]);
		if(!expectedCodes.is_open()){
		cerr << "could not open testbench/expectedCodes.txt" << endl;
		exit(EXIT_FAILURE);
	}
	
	// open exitcodes
		//test/output/exitCodes.txt
	ifstream exitCodes (argv[3]);
		if(!exitCodes.is_open()){
		cerr << "could not open test/output/exitCodes.txt" << endl;
		exit(EXIT_FAILURE);
	}

	CSVline tempCSV;
	string temp;

	// read in tests.csv 
	while(inCSV >> tempCSV.id, inCSV >> temp, inCSV >> tempCSV.instruction, inCSV >> temp,
		 inCSV >> tempCSV.status, inCSV >> temp, inCSV >> tempCSV.author){
		CSV.push_back(tempCSV);
	}
	
	// read in expectedCodes.txt 
	while(expectedCodes >> temp){
		expC.push_back(temp);
	}

	// read in exitCodes.txt
	while(exitCodes >> temp){
		exitC.push_back(temp);
	}

	// compare and set status
	for(uint32_t i = 0; i < CSV.size(); i++){
		if(expC[i] == exitC[i]){
			CSV[i].status = "Pass";
		}
		else{
			CSV[i].status = "Fail";
		}
		cout << CSV[i].print() << endl;
	}

	return 0;
}