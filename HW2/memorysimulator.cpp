//Christopher Torralba CS3800 Section: B

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

struct mainMem {
	//lru, clock, and fifo data structures
	//Clock Used bit array
	bool* used;
	int* timeStamp;
	int clockPtr;

	void lru(int uniqueVal, int progCount);
	void clock(int uniqueVal, int progCount);
	void fifo(int uniqueVal, int progCount);


	int* page;
	int pages_per_prog;
	int size;
	mainMem(int page, int num_programs);
};

mainMem::mainMem(int pageSize, int num_programs) {
	const int AVAILABLE_FRAMES = 512;
	unsigned int num_frames = AVAILABLE_FRAMES / pageSize;
	pages_per_prog = num_frames/num_programs; //size_per_prog = pages_per_prog

	size = num_frames;
	page = new int[num_frames];
	used = new bool[num_frames];
	timeStamp = new int[num_frames];
	for (int i = 0; i < num_frames; i++) {
		used[i] = 2;
		page[i] = -1;
		timeStamp[i] = 0;
	}

	clockPtr = 0;
}

void mainMem::fifo(int uniqueVal, int progCount) {
	//find the index with the earliest timestamp starting from the first slot in RAM
	int oldest = timeStamp[0];
	int index = 0;
	for (int i = 0; i < size; i++) {
		//value from timestamp is the lowest;
		if (oldest > timeStamp[i]) {
			oldest = timeStamp[i];
			index = i;
		}
	}
	//the oldest has been found so replace.
	page[index] = uniqueVal;
	timeStamp[index] = progCount;

	return;
}

void mainMem::clock(int uniqueVal, int progCount) {
	//starting from clockPtr
	//see if the value is a 1
	bool found = false;
	while (!found) {
		if (used[clockPtr] == 1) {
			used[clockPtr] = 0;
		}
		else { //use value is at 0
			page[clockPtr] = uniqueVal;
			timeStamp[clockPtr] = progCount;
			used[clockPtr] = 1;
			found = true;
		}
		if (clockPtr != size - 1) {
			clockPtr++;
		}
		else {
			clockPtr = 0;
		}
	}
	return;
}

void mainMem::lru(int uniqueVal, int progCount) {
	//find the index with the earliest timestamp starting from the first slot in RAM
	int oldest = timeStamp[0];
	int index = 0;
	for (int i = 0; i < size; i++) {
		//value from timestamp is the lowest;
		if (oldest > timeStamp[i]) {
			oldest = timeStamp[i];
			index = i;
		}
	}
	//the oldest has been found so replace.
	page[index] = uniqueVal;
	timeStamp[index] = progCount;

	return;
}

struct program {
	int size; //Store this
	vector<int> idNums; //contains the unique name
};

int main(int argc, char *argv[]) {
	bool inputError = false;
	ifstream list;
	ifstream trace;
	///////////////
	//Reading in Arguments
	///////////////
	if (argc < 6) {
		cout << "Not enough arguments specified" << endl;
		inputError = true;
	}
	else{
		string listFile(argv[1]);
		if (listFile.compare(listFile.size() - 4, 4, ".txt") == 0) {
			list.open(argv[1]);
		}
		else{
			listFile = listFile + ".txt";
			list.open(listFile.c_str());
		}
		if (!list.is_open()) {
			cout << "Program List File is not specified or found (arg1)" << endl;
			inputError = true;
		}
		string traceFile(argv[2]);
		if (traceFile.compare(traceFile.size() - 4, 4, ".txt") == 0) {
			trace.open(argv[2]);
		}
		else{
			traceFile = traceFile + ".txt";
			trace.open(traceFile.c_str());
		}
		if (!trace.is_open()) {
			cout << "Program Trace File is not specified or found (arg2)" << endl;
			inputError = true;
		}
		if (!(!strcmp(argv[3], "1") || !strcmp(argv[3], "2") || !strcmp(argv[3], "4") || !strcmp(argv[3], "8")
			|| !strcmp(argv[3], "16"))) {
			cout << "Invalid Page Size (arg3)" << endl;
			inputError = true;
		}
		if (!(!strcmp(argv[4], "clock") || !strcmp(argv[4], "fifo") || !strcmp(argv[4], "lru"))) {
			cout << "Invalid replacement algorithm (arg4)" << endl;
			inputError = true;
		}
		if (!(!strcmp(argv[5], "0") || !strcmp(argv[5], "1"))) {
			cout << "Invalid demand/prepaging option (arg5)" << endl;
			inputError = true;
		}
	}
	if (inputError) {
		cout << endl;
		cout << "The format for input is: " << endl;
		cout << "./memorysimulator arg1 arg2 arg3 arg4 arg5" << endl;
		cout << "arg1 = programlist file" << endl;
		cout << "arg2 = programtrace file" << endl;
		cout << "arg3 = page size (1,2,4,8,16)" << endl;
		cout << "arg4 = page replacement algorith (clock, fifo, or lru)" << endl;
		cout << "arg5 = demand/prepaging (0 = demand, 1 = prepaging)" << endl;
	}
	else{
		int page_size = atoi(argv[3]);
		
		//Counting the number of programs
		int num_of_prog = count(istreambuf_iterator<char>(list), istreambuf_iterator<char>(), '\n') + 1;
		list.clear();
		list.seekg(0, ios::beg);

		int num_of_acts = count(istreambuf_iterator<char>(trace), istreambuf_iterator<char>(), '\n') + 1;
		trace.clear();
		trace.seekg(0, ios::beg);


		//Declaring main memory struct
		mainMem main(page_size, num_of_prog);
		//Generating Page Table
		program prog;
		vector<program> pageTable;
		int program_no, progSize;
		int identifier = 0;

		for (int i = 0; i < num_of_prog; i++) {
			list >> program_no >> progSize;
			prog.size = 1 + ((progSize - 1) / page_size); //rounded up
			for (int j = 0; j < prog.size; j++) {
				prog.idNums.push_back(identifier);
				identifier++;
			}
			pageTable.push_back(prog);
			prog.idNums.clear();
		}
		
		//Default Loading the Memory
		int counter = 0;
		for (int i = 0; i < num_of_prog; i++){
			for (int j = 0; j < main.pages_per_prog; j++) {
				if (j < pageTable[i].size){
					main.page[counter] = pageTable[i].idNums[j];
					main.timeStamp[counter] = counter;
					main.used[counter] = 1;
				}
				counter++;
			}
		}
		
		//Performing actions from programtrace
		unsigned long progCounter = main.size;
		bool inMem = false;
		int accessIndex, relative_page, relative_word_no;
		int pageFaults = 0;
		bool page2 = false;

		//////////
		//Demand
		/////////
		for (int i = 0; i < num_of_acts; i++) {
			trace >> program_no >> relative_word_no;
			relative_page = relative_word_no / page_size;
			int search = pageTable[program_no].idNums[relative_page];
			inMem = false;
			//first find if it's in memory
			for (int j = 0; j < main.size; j++) {
				if (search == main.page[j]) {
					accessIndex = j;
					inMem = true;
					break;
				}
			}

			if (!inMem){ //If not in memory
				//then check if there's empty spot's in memory
				int indexEmpty = 0;
				bool emptySpot = false;
				pageFaults++;
				for (int k = 0; k < main.size; k++) {
					if (main.page[k] == -1) {
						indexEmpty = k;
						emptySpot = true;
						break;
					}
				}
				if (emptySpot) {
					main.page[indexEmpty] = search;
					main.timeStamp[indexEmpty] = progCounter;
					main.used[indexEmpty] = 1;
					progCounter++;
				}
				else { //if there's no empty spot perform and it's not in memory do replacement algorithm
					if (!strcmp(argv[4], "fifo")) {
						main.fifo(search, progCounter);
					}
					else if (!strcmp(argv[4], "clock")) {
						main.clock(search, progCounter);
					}
					else if (!strcmp(argv[4], "lru")) {
						main.lru(search, progCounter);
					}
					progCounter++;
				}
				///////////////
				//prepaging
				/////////////
				if (!strcmp(argv[5], "1") && ((relative_word_no / page_size) < pageTable[program_no].size)) {
					relative_page = (relative_word_no / page_size)+1;
					search = pageTable[program_no].idNums[relative_page];
					bool inMem2 = false;
					int accessIndex2;
					for (int j = 0; j < main.size; j++) {
						if (search == main.page[j]) {
							accessIndex2 = j;
							inMem2 = true;
							break;
						}
					}

					if (!inMem2) { //If not in memory
								  //then check if there's empty spot's in memory
						indexEmpty = 0;
						emptySpot = false;
						for (int k = 0; k < main.size; k++) {
							if (main.page[k] == -1) {
								indexEmpty = k;
								emptySpot = true;
								break;
							}
						}
						if (emptySpot) {
							main.page[indexEmpty] = search;
							main.timeStamp[indexEmpty] = progCounter;
							main.used[indexEmpty] = 1;
							progCounter++;
						}
						else { //if there's no empty spot perform and it's not in memory do replacement algorithm
							if (!strcmp(argv[4], "fifo")) {
								main.fifo(search, progCounter);
							}
							else if (!strcmp(argv[4], "clock")) {
								main.clock(search, progCounter);
							}
							else if (!strcmp(argv[4], "lru")) {
								main.lru(search, progCounter);
							}
							progCounter++;
						}
					}
					else {
						if (!strcmp(argv[4], "lru")) {
							main.timeStamp[accessIndex2] = progCounter;
						}
						if (!strcmp(argv[4], "clock")) {
							main.used[accessIndex2] = 1;
						}
						progCounter++;
					}
				}
			}
			else { //it is in memory
				if (!strcmp(argv[4], "lru")) {
					main.timeStamp[accessIndex] = progCounter;
				}
				if (!strcmp(argv[4], "clock")) {
					main.used[accessIndex] = 1;
				}
				progCounter++;
			}
		}
		cout << "========================" << endl;
		cout << "Algorithm: " << argv[4] << endl;
		cout << "Page Size: " << page_size << endl;
		cout << "Page Faults: " << pageFaults << endl;
		cout << "Demand (0) or Prepaging (1) : " << argv[5] << endl;
		cout << "========================" << endl;
	}
	return 0;
}