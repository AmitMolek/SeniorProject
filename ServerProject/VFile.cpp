#include "VFile.h"
#include "VContainer.h"

VFile::VFile() : StorageObject(""), fileStream(), fileName(), fileSize(), fileBuffer() {}

VFile::VFile(VFile&& other) noexcept :
	StorageObject(std::move(other)),
	fileStream(std::move(other.fileStream)), 
	fileName(std::move(other.fileName)),
	fileSize(std::exchange(other.fileSize, 0)), 
	fileBuffer(std::move(other.fileBuffer)) {
	fileStream.swap(other.fileStream);
}

VFile::VFile(fs::path _rootPath, std::string _fileName, 
			 uint64_t _fileSize, StorageObject* _parent) :
	fileBuffer(), StorageObject(_rootPath, _parent) {
	fileName = _fileName;
	fileSize = _fileSize;
}

VFile& VFile::operator << (std::string _str){
	AppendBytes(_str);
	return *this;
}

void VFile::operator = (VFile&& _vFile) noexcept {
	rootPath = std::move(_vFile.rootPath);
	fileStream = std::move(_vFile.fileStream);
	fileName = std::move(_vFile.fileName);
	fileBuffer = std::move(_vFile.fileBuffer);
	fileSize = std::exchange(_vFile.fileSize, 0);
	parentObject = std::exchange(_vFile.parentObject, nullptr);
}

std::ostream& operator<<(std::ostream& out, const VFile& obj) {
	out << "[" 
		<< obj.parentObject
		<< "," << obj.rootPath 
		<< "," << obj.fileName 
		<< "," << obj.fileSize 
		<< "]";
	return out;
}

void VFile::OpenFileStream() {
	fileStream = std::ofstream(rootPath, std::ios::binary);
}

void VFile::CloseFileStream(){
	SaveFileStream();
	fileStream.close();
}

void VFile::SaveFileStream() {
	fileStream << fileBuffer.str();
}

void VFile::AppendBytes(std::string _str) {
	fileBuffer << _str;
}
