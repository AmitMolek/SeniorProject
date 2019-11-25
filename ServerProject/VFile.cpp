#include "VFile.h"

VFile::VFile() : rootPath(), fileStream(), fileName(), fileSize(), fileBuffer(), parent() {}

VFile::VFile(VFile&& other) noexcept :
	rootPath(std::move(other.rootPath)), 
	fileStream(std::move(other.fileStream)), 
	fileName(std::move(other.fileName)),
	fileSize(std::exchange(other.fileSize, 0)), 
	fileBuffer(std::move(other.fileBuffer)), 
	parent(std::exchange(other.parent, nullptr)) {
	fileStream.swap(other.fileStream);
}

VFile::VFile(fs::path _rootPath, std::string _fileName, 
			 unsigned long long int _fileSize, StorageObject* _parent) : fileBuffer() {
	rootPath = _rootPath;
	fileName = _fileName;
	fileSize = _fileSize;
	parent = _parent;
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
	parent = std::exchange(_vFile.parent, nullptr);
}

std::ostream& operator<<(std::ostream& out, const VFile& obj) {
	out << "[" 
		<< obj.parent
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
	if (parent != nullptr)
		parent->UseCapacity(fileBuffer.str().size());
}

void VFile::SaveFileStream() {
	fileStream << fileBuffer.str();
}

void VFile::AppendBytes(std::string _str) {
	fileBuffer << _str;
}
