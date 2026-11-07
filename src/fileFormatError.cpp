#include "fileFormatError.h"

using namespace std;

FileFormatError::FileFormatError(const string& filename, int line, const string& message)
	: GameError(filename + " (linea " + to_string(line) + "): " + message)
	, filename(filename)
	, line(line)
{}

const string&
FileFormatError::getFilename() const
{
	return filename;
}

int
FileFormatError::getLine() const
{
	return line;
}
