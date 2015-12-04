
#include <vector>
#include <time.h> 
#include <chrono>
#include <iostream>
#include <fstream> 
#include <string>
#include <algorithm>
#include <unistd.h>

#include "mem_usage.h"
#include "wrappers.hpp"
#include "time_routines.hpp"
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::ifstream;
using std::pair;


string GetHostName() {
	char HOST_NAME[100];
	gethostname(HOST_NAME, sizeof(HOST_NAME));
	return (string)HOST_NAME;
}

const int CHECK_INTERVAL = 100000;

double GetTime100KQueries(const vector<string>& storage, const int inserted, TWrapper* mapperPtr) {

	vector<string>* queriesPtr = new vector<string>(CHECK_INTERVAL, "");

	{// assign random 100K keys from already inserted
		vector<int>* orderPtr = new vector<int>();
		for (int index = 0; index < inserted; ++index) {
			orderPtr->push_back(index);
		}
		std::srand ( unsigned ( std::time(0) ) );
		std::random_shuffle(orderPtr->begin(), orderPtr->end());
		for (int order_index = 0; order_index < CHECK_INTERVAL; ++order_index) {
			int keyIndex = (*orderPtr)[order_index];
			(*queriesPtr)[order_index] = storage[keyIndex] + "k";
		}
		delete orderPtr;
	}

	TTime startTime = GetTime();
	{
		for (int keyIndex = 0; keyIndex < (*queriesPtr).size(); ++keyIndex) {
			char result = mapperPtr->Get((*queriesPtr)[keyIndex]);
			if (result != 1) {
				std::cerr << "FUCKUP\n";
			}
		}
	}
	double elapsedTime = GetElapsedInSeconds(startTime, GetTime());

	{
		//give memory back
		delete queriesPtr;
		double vm_usage, resident_set;
		//2 times to be sure that the memory counter is refreshed
		process_mem_usage(vm_usage, resident_set);
		process_mem_usage(vm_usage, resident_set);
		cout << "#mem after query test " << vm_usage << "\n";
	}
	return elapsedTime;
}

void FillDictionary(const vector<string>& storage, TWrapper* mapperPtr) {
    double dataInserted = 0;
    double memoryConsumedBefore, residentSetBefore;
    process_mem_usage(memoryConsumedBefore, residentSetBefore);
    cout << "#before mem: " << memoryConsumedBefore / 1024.0 << "Mb\n";
    cout << "MAPPER:" << mapperPtr->Name << "\n";
    TTime start_time = GetTime();
    for (size_t keyIndex = 0; keyIndex < storage.size(); ++keyIndex) {
		if (keyIndex > 0 && keyIndex % CHECK_INTERVAL == 0) {
			double time100KInserts = GetElapsedInSeconds(start_time, GetTime());
			//time is measured, timer stopped
			double vm_usage, resident_set;
			process_mem_usage(vm_usage, resident_set);
			cout << "#mem before query test " << vm_usage << "\n";

			double time100KRequests = GetTime100KQueries(storage, keyIndex, mapperPtr);

			cout <<  ""   << "KEYS_INSERTED:" << keyIndex << 
			         "," << "DATA_INSERTED_MB:" << dataInserted / (1024.0 * 1024.0) << 
				 "," << "VM_MEM_CONSUMPT_MB:" << (vm_usage - memoryConsumedBefore) / 1024.0 << 
				 "," << "RESIDENT_MEM_CONSUMPT_MB:" << (resident_set - residentSetBefore) / 1024.0 << 
				 "," << "TIME_LAST_100K_INSERTS_S:" << time100KInserts << 
				 "," << "TIME_100K_QUERIES_S:" << time100KRequests << "\n";
			//reset timer and start counting time again
			start_time = GetTime();
		}
		string key = storage[keyIndex] + "k";
		bool inserted = mapperPtr->Set(key);
		if (inserted) {
			dataInserted += key.size() + 2;
		}
    }
}

void UploadData(const char* filename, vector<string>* storagePtr) {
    ifstream file(filename);
    string line;
    if (!file.is_open()) {
    	return;
    }
    while (getline(file, line)) {
		storagePtr->push_back(line);
		if (GetHostName() == "butch" && storagePtr->size() > 2000000) {
			break;
		}
    }
}

void Usage(char** argV) {
    cout << argV[0] << " <path to file with data>\n";
}

int main(int argC, char** argV) {
    if (argC != 2) {
		Usage(argV);
		return 1;
    }

    vector<string> storage;
    vector<TWrapper*> mappers;

    mappers.push_back(new THashMapWrapper());
    mappers.push_back(new TStdMapWrapper());
    mappers.push_back(new TGoogleDenseMapWrapper());
    mappers.push_back(new TGoogleSparseMapWrapper());
    mappers.push_back(new TBoostUnorderedMapWrapper());
    mappers.push_back(new TLexicoTreeWrapper());
    mappers.push_back(new TBPlusTreeWrapper());
    mappers.push_back(new TArtTreeWrapper());
    mappers.push_back(new THATTrieWrapper());

    vector<int> order;
    for (int index = 0; index < mappers.size(); ++index) {
    	order.push_back(index);
    }

    std::srand ( unsigned ( std::time(0) ) );
    std::random_shuffle(order.begin(), order.end());

    UploadData(argV[1], &storage);
    std::random_shuffle(storage.begin(), storage.end());
	for (int order_index = 0; order_index < mappers.size(); ++order_index) {
		int algo_index = order[order_index];
		FillDictionary(storage, mappers[algo_index]);
		delete mappers[algo_index];
		double memoryConsumedAfter, residentSetAfter;
		// yes, many times, to be sure that this stuff refreshed
	    process_mem_usage(memoryConsumedAfter, residentSetAfter);
	    process_mem_usage(memoryConsumedAfter, residentSetAfter);
	    process_mem_usage(memoryConsumedAfter, residentSetAfter);
	}
    return 0;
}
