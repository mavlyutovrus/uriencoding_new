#pragma once

#define GCC_VERSION (__GNUC__ * 10000 \
	    + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION >= 40300
#include <tr1/unordered_map>
#define hash_map std::tr1::unordered_map
#else
#include <ext/hash_map>
#define hash_map __gnu_cxx::hash_map
#endif


#include <string>
#include <vector>
#include <map>
#include <boost/unordered_map.hpp>
#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_map>
#include "lexico_tree/KeyManager.h"
#include "bplus_tree.hpp"
#include "wrapper_proto.h"

#include "art-tree/art.h"
#include "hat-trie/hat-trie.h"


using std::vector;
using std::string;
using std::cout;
using std::ifstream;
using std::pair;


struct TEmtpyWrapper : TWrapper {
	TEmtpyWrapper() {
		Name = "empty_wrapper";
	}
	~TEmtpyWrapper() {
	}
	bool Set(const string& key) {
		return true;
	}
	const char Get(const string& key) {
		return 1;
	}
};


struct TBoostUnorderedMapWrapper : TWrapper {
	TBoostUnorderedMapWrapper() {
		Name = "boost::unordered_map";
	}
	~TBoostUnorderedMapWrapper() {
	}
	boost::unordered_map<string, char> Dictionary;
	bool Set(const string& key) {
		bool inserted = Dictionary.insert(pair<string, char>(key, 1)).second;
		return inserted;
	}
	const char Get(const string& key) {
		boost::unordered_map<string, char>::const_iterator it = Dictionary.find(key);
		if (it == Dictionary.end()) {
			return 0;
		}
		return it->second;
	}
};



struct TLexicoTreeWrapper : TWrapper {
	TLexicoTreeWrapper() {
		Name = "Lexicographic tree";
	}
	~TLexicoTreeWrapper() {
	}
	NLexicoTree::KeyManager Dictionary;
	bool Set(const string& key) {
		NLexicoTree::Leaf* leaf = Dictionary.Add(key.c_str());
		return leaf != NULL;
	}
	const char Get(const string& key) {
		NLexicoTree::KEY_ID keyId = Dictionary.Get(key.c_str());
		return 1;
	}
};


struct TBPlusTreeWrapper : TWrapper {
	TBPlusTreeWrapper() {
		Name = "B+ tree";
	}
	~TBPlusTreeWrapper() {
	}
	NBPlusTree::BPlusTree<string, char, 10, 10> Dictionary;
	bool Set(const string& key) {
		Dictionary.insert(key, 1);
		return 1;
	}
	const char Get(const string& key) {
		char value;
		bool found = Dictionary.find(key, &value);
		return found ? value : 0;
	}
};




struct THashMapWrapper : TWrapper {
	THashMapWrapper() {
		Name = "std::unordered_map";
	}
	~THashMapWrapper() {
	}
	hash_map<string, char> Dictionary;
	bool Set(const string& key) {
		bool inserted = Dictionary.insert(pair<string, char>(key, 1)).second;
		return inserted;
	}
	const char Get(const string& key) {
		hash_map<string, char>::const_iterator it = Dictionary.find(key);
		if (it == Dictionary.end()) {
			return 0;
		}
		return it->second;
	}
};

struct TStdMapWrapper : TWrapper {
	TStdMapWrapper() {
		Name = "std::map";
	}
	~TStdMapWrapper() {
	}
	std::map<string, char> Dictionary;
	bool Set(const string& key) {
		bool inserted = Dictionary.insert(pair<string, char>(key, 1)).second;
		return inserted;
	}
	const char Get(const string& key) {
		std::map<string, char>::const_iterator it = Dictionary.find(key);
		if (it == Dictionary.end()) {
			return 0;
		}
		return it->second;
	}
};


struct TGoogleDenseMapWrapper : TWrapper {
	TGoogleDenseMapWrapper() {
		Name = "google dense_hash_map";
		string empty_key = "a hyle";
		Dictionary.set_empty_key(empty_key);
	}
	~TGoogleDenseMapWrapper() {
	}
	google::dense_hash_map<string, char> Dictionary;
	bool Set(const string& key) {
		bool inserted = Dictionary.insert(pair<string, char>(key, 1)).second;
		return inserted;
	}
	const char Get(const string& key) {
		google::dense_hash_map<string, char>::iterator it = Dictionary.find(key);
		if (it == Dictionary.end()) {
			return 0;
		}
		return it->second;
	}
};

struct TGoogleSparseMapWrapper : TWrapper {
	TGoogleSparseMapWrapper() {
		Name = "google sparse_hash_map";
		string empty_key = "a hyle";
		//Dictionary.set_empty_key(empty_key);
	}
	~TGoogleSparseMapWrapper() {
	}
	google::sparse_hash_map<string, char> Dictionary;
	bool Set(const string& key) {
		bool inserted = Dictionary.insert(pair<string, char>(key, 1)).second;
		return inserted;
	}
	const char Get(const string& key) {
		google::sparse_hash_map<string, char>::iterator it = Dictionary.find(key);
		if (it == Dictionary.end()) {
			return 0;
		}
		return it->second;
	}
};


struct TArtTreeWrapper : TWrapper {
	art_tree* TreePtr;
	TArtTreeWrapper() {
		Name = "art_tree";
		string empty_key = "a hyle";
		TreePtr = new art_tree();
		init_art_tree(TreePtr);
	}
	~TArtTreeWrapper() {
	    destroy_art_tree(TreePtr);
	    delete TreePtr;
	}
	bool Set(const string& key) {
		return art_insert(TreePtr, (unsigned char*)key.c_str(), key.length(), 1) == -1;
	}

	const char Get(const string& key) {
		return (char)art_search(TreePtr, (unsigned char*)key.c_str(), key.length());
	}
};


struct THATTrieWrapper : TWrapper {
	hattrie_t* TriePtr;
	THATTrieWrapper() {
		Name = "hat_trie";
		string empty_key = "a hyle";
		TriePtr = hattrie_create();
	}
	~THATTrieWrapper() {
	    hattrie_free(TriePtr);
	}
	bool Set(const string& key) {
		value_t* valuePtr = hattrie_get(TriePtr, key.c_str(), key.length());
		*valuePtr = 1;
		return true;
	}

	const char Get(const string& key) {
		value_t* valuePtr = hattrie_get(TriePtr, key.c_str(), key.length());
		return *valuePtr == 1 ? 1 : -1;
	}
};

