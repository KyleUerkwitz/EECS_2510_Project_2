//	File:			huffman.cpp
//	Author:			Kyle Uerkwitz
//	Date:			October 18, 2019
//	Class:			EECS 2510-001
//	Description:	The implementation code for a Huffman tree that can encode, decode, and build a tree file.
//					This file contains the public methods that serve as an interface to outside calls. These
//					public methods in turn call the private functions that perform intermediary steps, such as
//					building the tree, or performing encoding operations.

#include "huffman.h"

void Huffman::displayHelp()
{
	/*	This method displays the available commands for the Huffman Tree Encoding to the user. 
	*/

	cout << "HUFF -h/-?/-help \t\t\t Display help usage" << endl;
	cout << "HUFF -e file1 [file2] \t\t\t Encode file1 (if file2 not specified, outputs file1.huf)" << endl;
	cout << "HUFF -d file1 file2 \t\t\t Decode file1 into file2 (both must be specified)" << endl;
	cout << "HUFF -t file1 [file2] \t\t\t Create a tree file from file1 (if file2 not specified, outputs file1.htree)" << endl;
	cout << "HUFF -et file1 file2 [file3] \t\t Encode file1 using tree in file2 (if file3 not specified, outputs file1.huf)" << endl;
}

void Huffman::MakeTreeBuilder(string inputFile, string outputFile) 
{
	/*	A public method that makes input streams, then calls the proper
		private methods to output the tree builder information.
		Cleans up the I/O streams as well.
	*/
	ifstream file_input(inputFile, ios::binary | ios::in);		// Open file in binary mode
	if (!file_input)
	{
		cout << "Input file " << inputFile << " not accessible. Ensure the path is correct. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	ofstream file_output(outputFile, ios::binary | ios::out);
	if (!file_output)
	{
		cout << "Output file " << outputFile << " not accessible. Ensure the path is correct, \n or that the file specified is available to be written to. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}

	buildFrequencyTable(file_input);						// Build the frequency table
	file_input.clear();										// Because we built the frequency table, we should seek it back to the beginning
	file_input.seekg(0);
	createTree();											// Make the tree for the frequencies found
	file_input.close();										// Close the file to prevent errrors.
	outputTreeBuilder(file_output);							// Output the tree built
	file_output.close();									// Close the file to prevent errrors.
}
void Huffman::EncodeFile(string inputFile, string outputFile)
{
	/*	A public method that makes input streams, then calls the proper
		private methods to encode the input file to the output.
		Cleans up the I/O streams as well.
	*/
	ifstream file_input(inputFile, ios::binary | ios::in);		// Open file in binary mode
	if (!file_input)
	{
		cout << "Input file " << inputFile << " not accessible. Ensure the path is correct. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	ofstream file_output(outputFile, ios::binary | ios::out);
	if (!file_output)
	{
		cout << "Output file " << outputFile << " not accessible. Ensure the path is correct, \n or that the file specified is available to be written to. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	buildFrequencyTable(file_input);						// Build the frequency table
	file_input.clear();										// Because we built the frequency table, we should seek it back to the beginning.
	file_input.seekg(0);
	createTree();											// Make the tree for the frequencies found
	outputTreeBuilder(file_output);							// Place the tree in the first 510 bytes.
	EncodeFile_(file_input, file_output);					// Encode the rest of the file
	file_input.close();										// Close the file to prevent errrors.
	file_output.close();									// Close the file to prevent errrors.
}
void Huffman::DecodeFile(string inputFile, string outputFile)
{
	/*	A public method that makes input streams, then calls the proper
		private methods to decode the encoded input to the output specified.
		Cleans up the I/O streams as well.
	*/
	ifstream file_input(inputFile, ios::binary | ios::in);		// Open file in binary mode
	if (!file_input)
	{
		cout << "Input file " << inputFile << " not accessible. Ensure the path is correct. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	ofstream file_output(outputFile, ios::binary | ios::out);
	if (!file_output)
	{
		cout << "Output file " << outputFile << " not accessible. Ensure the path is correct, \n or that the file specified is available to be written to. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	rebuildTree(file_input);								// Rebuild the tree in the file
	file_input.clear();										// Because we read the tree, we should reset it.
	file_input.seekg(0);
	DecodeFile_(file_input, file_output);					// Decode the file into the output
	file_input.close();										// Close the file to prevent errrors.
	file_output.close();									// Close the file to prevent errrors.
}
void Huffman::EncodeFileWithTree(string inputFile, string TreeFile, string outputFile)
{
	/*	A public method that makes input streams, then calls the proper
		private methods to encode the tree using the provided tree file
		and input file.
		Cleans up the I/O streams as well.
	*/
	ifstream file_input(inputFile, ios::binary | ios::in);		// Open file in binary mode
	if (!file_input)
	{
		cout << "Input file " << inputFile << " not accessible. Ensure the path is correct. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	ifstream tree_input(TreeFile, ios::binary | ios::in);		// Open file in binary mode
	if (!tree_input)
	{
		cout << "Input file " << TreeFile << " not accessible. Ensure the path is correct. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	ofstream file_output(outputFile, ios::binary | ios::out);
	if (!file_output)
	{
		cout << "Output file " << outputFile << " not accessible. Ensure the path is correct, \n or that the file specified is available to be written to. \n";		// If the file won't open, display the error to the user and quit.
		exit(1);
	}
	rebuildTree(tree_input);								// Rebuild the tree
	tree_input.close();										// Close the file to prevent errors.
	outputTreeBuilder(file_output);							// Output the tree builder info
	file_output.clear();									// Reset the output
	file_output.seekp(0);
	EncodeFile_(file_input, file_output);					// Encode the file
	file_input.close();										// Close the file to prevent errors.
	file_output.close();									// Close the file to prevent errors.
}

Huffman::Huffman() 
{
	/*	The constructor for the Huffman tree. Initializes the frequency table 
		to 0 weight nodes of all possible 8-bit values.
	*/

	// Initialize (clear) the frequency table before beginning.
	for (int i = 0; i < 256; i++)
	{
		node* initial_node = new node();
		initial_node->byte_value = i;		// Each node will have the 8-bit value corresponding to index, and the value of the 'character' found.
		frequency_table[i] = initial_node;
	}
}


void Huffman::buildFrequencyTable(ifstream &file_input)
{
	/*	Iterates through the entire file, constructing nodes for each possible byte value,
		incrementing the weight of each one as it is encountered. These are stored in a
		class array to be accessed later for building the tree itself.
	*/

	char current_char;								// Make a byte value.
	file_input.read(&current_char, 1);				// PRIMING READ. Read in a single byte from the file. This expects a char, so we then convert
	while (file_input)								// Read while there is content left
	{
		unsigned char current_byte = unsigned char(current_char); // Convert to unsigned char
		frequency_table[current_byte]->weight++;	// Increase the weight of that byte.
		file_input.read(&current_char, 1);			// Read in a single byte from the file for the next iteration. 
	}
}

void Huffman::createTree()
{
	/*	This method takes the frequency values constructed, and builds a tree out of them.
		Minimum nodes are found based on weight, and their positions recorded.
		These nodes are 'merged' by linking them to a parent that records the sum of their weights.
		This algorithm runs until all of the nodes are in a single root node. 
	*/

	// We know we will need just enough iterations to merge all the nodes together in the tree. (255 merges of 2 nodes = 510 bytes)
	for(int k=0; k <255; k++)
	{
		// Set up variables to find the lowest weights and record their position.
		unsigned char rowNode1, rowNode2;
		rowNode1 = rowNode2 = 0;
		unsigned int minVal1, minVal2;
		minVal1 = minVal2 = UINT_MAX;

		// Iterate through and find the lowest node.
		for (int i = 0; i < 256; i++)
		{
			node* current = frequency_table[i];		// Record the current node for code clarity
			if (current == NULL) continue;			// Skip NULL nodes to avoid access errors.
			if (current->weight < minVal1)			
			{
				minVal1 = current->weight;			// Record the current weight if it's the new minimum
				rowNode1 = i;						// Also record that subscript.
			}
		}
		// Second minimum value loop.
		for (int i = 0; i < 256; i++)
		{
			node* current = frequency_table[i];				// Record the current node for code clarity
			node* minimumNode1 = frequency_table[rowNode1];	// Keep track of the previous minimum node so we don't hit it.
			if (current == NULL || current == minimumNode1 ) continue;			// Skip NULL nodes, and the first minimum
			if (current->weight < minVal2)
			{
				minVal2 = current->weight;					// Record the current weight if it's the new minimum
				rowNode2 = i;								// Also record that subscript.
			}
		}

		// Find the smaller and larger subscript to properly give the correct children based on subscript.
		// This makes it easier to figure out which node came "first" in the tree
		unsigned char minRow, maxRow;				
		if (rowNode1 < rowNode2) { minRow = rowNode1; maxRow = rowNode2; }
		else {minRow = rowNode2; maxRow = rowNode1;}
		
		node* minimumNode1 = frequency_table[minRow];	// We can now assign the two nodes properly		
		node* minimumNode2 = frequency_table[maxRow];
		
		node* newParent = new node();					// Make the parent, giving the smaller subscript node to the left child.
		newParent->LCH = minimumNode1;
		newParent->RCH = minimumNode2;
		minimumNode1->parent = newParent;
		minimumNode2->parent = newParent;

		newParent->weight = minimumNode1->weight + minimumNode2->weight;		// Add the weights of the children
		
		frequency_table[minRow] = newParent;	// Reassign the frequency table nodes.
		frequency_table[maxRow] = NULL;			// The larger subscript points to nothing now.

		
		tree_sequence[k*2] = minRow;			// Record the two nodes that were squashed in the tree_sequence.
		tree_sequence[k*2+1] = maxRow;			// Because the loop is 0 <= k < 255, we need to multiply by 2 to access the 'pair' index
	}
	root_node = frequency_table[0]; // Assign the root node for easy access.
	frequency_table[0] = NULL;
}

void Huffman::rebuildTree(ifstream &file_input)
{
	/*	A private method to rebuild the tree from a given encoded tree in a file.
		This method reads the first 510 bytes of the file to  construct the tree and
		store it in the root_node of the Huffman object.
		The method checks if there are not enough bytes for the tree, or if it creates
		an invalid tree object. 
	*/

	unsigned char firstNode, secondNode;
	firstNode = secondNode = NULL;
	
	for (int i = 0; i < 510; i++)								// Read the first 510 bytes of the file.
	{
		if (!file_input)		// Check the case that we run out of bits to read before the loop is over.
		{
			cerr << "Tree encoding could not be read. The file is less than the required Huffman tree length.\n";
			exit(1);			// Tell the user there is an error and exit.
		}
		char current_char;							// Make a byte value.
		file_input.read(&current_char, 1);			// Read in a single byte from the file. This expects a char, so we then convert
		tree_sequence[i] = unsigned char(current_char);
	}
	for (int i = 0; i < 255; i++)
	{
		firstNode = tree_sequence[i*2];
		secondNode = tree_sequence[i*2+1];
		node* minimumNode1 = frequency_table[firstNode];		// Assign the nodes as given
		node* minimumNode2 = frequency_table[secondNode];

		if (minimumNode1 == NULL || minimumNode2 == NULL)		// If either node is NULL, it's possible the tree file provided was invalid
		{
			cerr << "Could not build Huffman tree from the provided file. Make sure it is in the correct format. \n";
			exit(1);
		}

		node* newParent = new node();							// Make the parent, assigning the first node to the left.
		newParent->LCH = minimumNode1;
		newParent->RCH = minimumNode2;
		minimumNode1->parent = newParent;
		minimumNode2->parent = newParent;

		frequency_table[firstNode] = newParent;					// Put the merged nodes into the lower subsript.
		frequency_table[secondNode] = NULL;						// Clear the larger subscript pointer.
	}
	root_node = frequency_table[0];
}

void Huffman::outputTreeBuilder(ofstream &output_stream)
{
	/*	This method takes the tree being stored in the Huffman object,
		and writes it out to a file in 510 bytes. The method checks
		if the file is available for writing to.
	*/
	for (unsigned char i : tree_sequence)
	{
		output_stream << i;
	}
	return;
}

void Huffman::EncodeFile_(ifstream &file_input, ofstream &file_output)
{
	/*	A method to take an input file, read in each byte, and output
		the encoded result based on the Huffman tree.
	*/
	createEncodingStrings();
	file_output.seekp(510);		// Because the tree is always included in the file, we need to seek the output file and skip those 510 bytes.

	char current_char;											// Read a byte from the file to begin 
	file_input.read(&current_char, 1);							// Priming read to get the first character. .read() expects a char so we need to convert.
	unsigned char currentByte = unsigned char(current_char);	// Convert to unsigned character
	string outputBuffer = "";									// Initialize a string output buffer to hold the encodings until there is enough bits to write.

	while(file_input)
	{
		string currentEncoding = bit_encodings[currentByte];	// Lookup the bit encoding for the currentByte
		outputBuffer += currentEncoding;						// Add it to the output	buffer
		while (outputBuffer.size() >= 8)						// If we have enough in the output buffer, we can begin writing it.
		{
			string outputBitString = outputBuffer.substr(0,8);	// Store the string of output that will be converted to a byte. (Characters 0-7, length of 8)
			outputBuffer.erase(0, 8);							// Remove characters 0-7 of the output buffer.
			unsigned char outputByte = stringToByte(outputBitString);	// Convert this string to a byte.
			file_output << outputByte;							// Write the byte.
		}
		
		file_input.read(&current_char, 1);						// Finally, read the last character.
		currentByte = unsigned char(current_char);				// Convert it to unsigned.
	}
	if (outputBuffer.size() > 0)								// If we still have some bytes left in the output buffer, we'll need some padding bits!
	{
		string padding_bits = "";								// Start with an empty padding
		for (int i = 0; i < 256; i++)							// Search the encoding strings array
		{
			int bits_required = 8 - outputBuffer.size();		// Change the value to the amount remaining
			if (bit_encodings[i].size() > bits_required)	// We can use a padding bit that will cause us to not hit any leaf nodes by choosing one longer than the output buffer size.
			{
				padding_bits = bit_encodings[i].substr(0, bits_required);		// Get the string of the amount for the padding.
				break;				// Exit the loop on the first one we find.
			}
		}
		outputBuffer += padding_bits;		// Concatenate the padding bits to the string.
		unsigned char outputByte = stringToByte(outputBuffer);	// Now, we can just convert the byte and output it.
		file_output << outputByte;
	}
}

unsigned char Huffman::stringToByte(string bit_string)
{
	/*	A method to convert a given bit encoding string of length 8 into
		a byte where each bit is represented by the character at that position.
	*/
	unsigned char outputByte = 0;
	for (int i = 0; i < 8; i++)
	{
		if (bit_string.at(i) == '1')	// When the current bit should be 1
		{
			outputByte |= 0x01;			// Bitwise OR the output byte with 0000 0001 to add it to the end
		}
		if (i == 7) break;				// Break so we don't shift the byte more than we need to (7 shifts only, not 8).
		outputByte = outputByte<< 1;	// Shift the byte over. We don't need to check for 0, as all bits are 0 to begin with. 
	}
	return outputByte;
}
void Huffman::DecodeFile_(ifstream &file_input, ofstream &file_output)
{
	/*	A method to decode the provided file into the output file. This serves as a private
		function that does the behind the scenes work. The calling function to this
		can decide whether the tree is included or not, allowing for files to be encoded from 
		a tree in a separate file.
	*/

	node* current_node = root_node;				// Set the current node being navigated as the root node.
	char current_char;							// Read a byte from the file to begin 
	file_input.seekg(510);						// Because the tree is included in the file, we need to seek the input file and skip those 510 bytes.
	
	file_input.read(&current_char, 1);							// Priming read to get the first character. .read() expects a char so we need to convert.
	unsigned char currentByte = unsigned char(current_char);	// Convert to unsigned character
	int iteration = 0;											// Keep track of which iteration to see if we need to read an additional byte
	while (file_input)
	{
		if (iteration == 8)						// If the bit string has gone through 8 iterations, it is now x00. We need a new byte.
		{
			file_input.read(&current_char, 1);	// Read in the new byte, then convert it below.
			currentByte = unsigned char(current_char);
			iteration = 0;						// Restart this iterator

			if (!file_input)					// FIXED BUG from first submission : Check that the file input is not at the end.
			{
				break;							// We need to break in the case that the EOF flag was set. This prevents extraneous bytes from being written at the end.
			}
		}

		if ( (currentByte | 0x7F) == 0xFF) current_node = current_node->RCH;	// If the current first bit is true, we go right
		else current_node = current_node->LCH;	// Else, we go left
			
		currentByte = currentByte << 1;			// Remove the bit we just read.

		if (current_node == NULL) break;		// If the node is null, we fell OFF the tree. This is an ending value.

		if (current_node->LCH == NULL && current_node->RCH == NULL)	// Once we have hit a leaf (no children), we can write the byte.
		{
			file_output << current_node->byte_value;		// Write the bytes
			current_node = root_node;						// Start over at the root
		}
		iteration++;		// Increase the bit iteration for the current byte.
	}
}

void Huffman::createEncodingStrings()
{
	/*	Begins creating encoding strings for every node by starting 
		at the root, with an empty encoding string.
	*/
	buildNodeString(root_node, "");
}

void Huffman::buildNodeString(node* current_node, string current_encoding)
{
	/*	This method visits all nodes (starting from the root provided) and 
		creates a string based on the path taken to reach their root.
		Recurses LEFT-wards through until a leaf is hit. When a leaf is hit, 
		it will then start work on the RIGHT path. 
		Each time a LEFT path is taken, the current encoding gains a 0, and
		a 1 for the RIGHT path. After a leaf is hit, the path is stored in the
		class array. 
	*/

	if (current_node->LCH == NULL && current_node->RCH == NULL)			// If we hit a leaf node, we save the encodings in the table
	{
		bit_encodings[unsigned int(current_node->byte_value)] = current_encoding;		// Save the encodings from the current node in the byte value the node holds
	}
	else
	{
		buildNodeString(current_node->LCH, current_encoding + '0');		// First try the left path, adding a 0 to the encoding if we do.

		buildNodeString(current_node->RCH, current_encoding + '1');		// Once the left path returns, we try the right path, adding a 1 to the encoding if we do.
	}
}