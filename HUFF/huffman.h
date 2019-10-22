//	File:			huffman.h
//	Author:			Kyle Uerkwitz
//	Date:			October 18, 2019
//	Class:			EECS 2510-001
//	Description:	The header file for huffman.cpp. Contains the public facing interface methods private implementations for them. 
//					Also pulls in the necessary modules from the std library.

#pragma once
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Huffman
{
public:
	Huffman();
	void displayHelp();																// Displays usage information on the program.
	void MakeTreeBuilder(string inputFile, string outputFile);						// Makes a tree builder from the given input file.
	void EncodeFile(string inputFile, string outputFile);							// Encodes a file (creating a tree in the process)
	void DecodeFile(string inputFile, string outputFile);							// Decodes a file (one already containing a tree)
	void EncodeFileWithTree(string inputFile, string TreeFile, string outputFile);	// Encodes a file with a separately defined tree.
private:
	struct node 
	{
		/*	The basis for a single 8-bit value's Huffman coding. Provides all required memory mappings */

		unsigned char byte_value;					// The value of the byte ("character") in the node (if it is a leaf)
		unsigned int weight;						// The weight of the node.
		node* LCH;									// The left child.
		node* RCH;									// The right child.
		node* parent = NULL;						// Parent node (to make traversing easier when building encoding strings.)
	};
	node* root_node;								// The root node of the Huffman tree. At the end of tree building, it will be an ancestor to every possible byte value.
	
	// Tree information
	node* frequency_table[256];						// The frequency table mapping. This table holds the calculated frequencies of all bytes in a file.
	unsigned char tree_sequence[510];				// The sequence that creates the tree for the input file. There are 510 positions, and each holds the frequency rows to be merged (from 0-255, fitting in an unsigned char).
	string bit_encodings[256];						// Array of strings that represent each byte value's encoded representation. We can use a string to store each of these so we don't need to worry about bits unil encoding.
	
	// Private IO Functions
	void DecodeFile_(ifstream &input_stream, ofstream &output_stream); // Decodes a file given in the input stream, to the output stream.
	void EncodeFile_(ifstream &input_stream, ofstream &output_stream); // Encodes a file given in the input stream, assuming the encoding strings have already been made.
	void outputTreeBuilder(ofstream &outputFile);		// Output the tree builder to the desired file.

	// Private Tree Building Functions
	void buildFrequencyTable(ifstream &input_stream);	// Calculates the frequency of characters from a given file.
	void rebuildTree(ifstream &input_stream);			// Rebuilds the tree from an input file.
	void createTree();									// The method that creates the tree from the frequency table.

	// Private Encoding String Functions
	void createEncodingStrings();						// Creates a list of bit_encodings in the form of strings by starting at the root and calling buildNodeString.
	void buildNodeString(node* current_node, string current_encoding);	// Creates a string representing the bit path of the node from the root.
	unsigned char stringToByte(string bit_string);		// Converts a bit string (1,0 in string format) to a byte.
};