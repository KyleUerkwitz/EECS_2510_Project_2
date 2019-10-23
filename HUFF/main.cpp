//	File:			main.cpp
//	Author:			Kyle Uerkwitz
//	Date:			October 18, 2019
//	Class:			EECS 2510-001
//	Description:	The commandline interface and logic that drives the Huffman tree encoding process.
//					Commandline parameters are evaluated for validity, then executed different functions that
//					are supported by the Huffman class. As well, the time of execution for a function is recorded
//					and displayed to the user.

#include <chrono>
#include "huffman.h"

using namespace std;

unsigned int getFileSize(string path)
{
	/*	A method to get the file size from a given path.
		This method  helps gives the user information on the
		amount of compression for the files. */

	// NOTE:	Because this implementation was not included in the definition of the Huffman interface,
	//			I felt it best to include this in the 'main' file, as it interacts with the user.

	ifstream file_input(path, ios::binary | ios::in);	// Construct the input stream.
	unsigned int begin = file_input.tellg();			// Record the beginning position.
	file_input.seekg(0, ios::end);						// Move the stream head to the end.
	unsigned int end = file_input.tellg();				// Record the end position.
	file_input.close();									// Close the stream.
	return end - begin;									// Return size.
}

int main(int argc, char* argv[])
{
	Huffman* HuffmanTreeEncoder = new Huffman();						// Create the encoder object to begin operations.


	string input_path = "";												// Initialize some variables to begin
	string output_path = "";
	string tree_file = "";

	auto beginTime = chrono::high_resolution_clock::now();				// Begin recording the time of the operation.

	// Begin validating parameters, and make decisions on what commands to assume.
	if (argv[1] == NULL)												// No parameters provided, exit the script.
	{
		cerr << "No parameters provided. \n";
		exit(1);
	}
	else if (string(argv[1]) == "-h" || string(argv[1]) == "-?" || string(argv[1]) == "-help")	// Help command
	{
		HuffmanTreeEncoder->displayHelp();								// Display Help, and exit
		exit(1);
	}
	else if (string(argv[1]) == "-e")									// Encode Command
	{
		if (argv[2] == NULL)
		{
			cout << "No file was provided to encode. \n";				// Exit if a file was not provided
			exit(1);
		} 

		input_path = argv[2];											// Record input path

		if (argv[3] == NULL)
		{
			output_path = input_path.substr(0, input_path.find('.')) += ".huf";	// If the output file was not specified, remove the extension and add ".huf"
		}
		else {
			output_path = argv[3];										// Else, use the specified path
		}
		HuffmanTreeEncoder->EncodeFile(input_path, output_path);		// Call the function
	}
	else if (string(argv[1]) == "-d")
	{
		if (argv[2] == NULL)
		{
			cout << "No file was provided to decode. \n";				// Exit if a file was not provided
			exit(1);
		}
		if (argv[3] == NULL)
		{
			cout << "No output path was specified. \n";					// Exit if a file was not provided
			exit(1);
		}
		input_path = argv[2];											// Record input path
		output_path = argv[3];											// Record output path
		
		HuffmanTreeEncoder->DecodeFile(input_path, output_path);		// Call the function
	}
	else if (string(argv[1]) == "-t")									// Build tree command
	{
		if (argv[2] == NULL)
		{
			cout << "No file was provided to build a tree with. \n";				// Exit if a file was not provided
			exit(1);
		}
		input_path = argv[2];
		if (argv[3] == NULL)
		{
			output_path = input_path.substr(0, input_path.find('.')) += ".htree";	// If the output file was not specified, remove the extension and add ".htree"
		}
		else {
			output_path = argv[3];													// Else, use the specified path
		}
		HuffmanTreeEncoder->MakeTreeBuilder(input_path, output_path);	// Call the function
	}
	else if (string(argv[1]) == "-et")									// Encode with tree command
	{
		if (argv[2] == NULL)
		{
			cout << "No file was provided to encode with. \n";		// Exit if a file was not provided
			exit(1);
		}
		input_path = argv[2];
		if (argv[3] == NULL)
		{
			cout << "No tree file was provided to encode with \n";					// Exit if a file was not provided
			exit(1);
		}
		tree_file = argv[3];
		if (argv[4] == NULL)
		{
			output_path = input_path.substr(0, input_path.find('.')) += ".huf";	// If the output file was not specified, remove the extension and add ".htree"
		}
		else {
			output_path = argv[4];													// Else, use the specified path
		}
		HuffmanTreeEncoder->EncodeFileWithTree(input_path, tree_file, output_path);
	}
	else {
		cout << "No valid parameters were found. Try 'HUFF -h' for help.  \n";
		return(1);		// Break if we were not given a valid parameter
	}

	auto endTime = chrono::high_resolution_clock::now();											// End the clock.
	float duration = chrono::duration_cast<chrono::milliseconds>(endTime - beginTime).count();		// Calculate the time difference
	printf("Time: %.3f seconds. \t", duration / 1000);												// Print the time duration.
	printf("%u bytes in / %u bytes out", getFileSize(input_path), getFileSize(output_path));		// Print the bytes in and bytes out of the operation.
}