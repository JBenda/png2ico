#include "api.hpp"
#include <cassert>
#include <fstream>
#include <optional>

#include "CImg.h"

typedef unsigned char px_t;

template <typename ...Args>
std::string&& sprint(const char* format, Args ...args) {
	int len = std::snprintf(nullptr, 0, format, args...);
	assert(len >= 0);

	char* buf = new char[len + 1];
	std::snprintf(buf, len + 1, format, args...);
	std::string str(buf);
	delete[] buf;
	return std::move(str);
}

struct ICOFile {
	ICOFile(unsigned int _dim) : dim{ _dim }, pngSize{}{}
	ICOFile(unsigned int _dim, unsigned int _pngSize) : dim{ _dim }, pngSize{ _pngSize } {}
	unsigned int dim; // width = height = dim
	// if png data then exist else calculate bmp size
	std::optional<unsigned int> pngSize; // size from img in byte
};

std::ostream& PrintLittleEndien(std::ostream& _os, unsigned int num, unsigned int size) {
	for (unsigned int i = 0; i < size * 8; i += 8) {
		_os.put(static_cast<unsigned char>(num >> i));
	}
	return _os;
}

std::ostream& PrintICOHeader(std::ostream& _os, const std::vector<ICOFile>& _pics, std::vector<std::string>& _errStack) {
	constexpr unsigned int HedearSize = 6;
	constexpr unsigned int FileHeaderSize = 16;
	constexpr unsigned int BMPHeaderSize = 54;

	const unsigned int imgsCount = static_cast<unsigned int>(_pics.size());
	if (imgsCount > 0xFFFF) {
		_errStack.push_back(sprint("max imgs per imgs per ico file are: 0xFFFF: %d", imgsCount));
		return _os;
	}
	if (imgsCount == 0) {
		_errStack.push_back(sprint("need min 1 image to transfare to ico: %d", imgsCount));
		return _os;
	}

	unsigned int offset = 0;
	constexpr char Sig[] = { 0, 0, 1, 0 };
	constexpr unsigned int SigSize = sizeof(Sig) / sizeof(*Sig);
	_os.write(Sig, SigSize);
	PrintLittleEndien(_os, imgsCount, 2);
	
	offset = imgsCount * FileHeaderSize + HedearSize;

	for (const ICOFile& f : _pics) {
		if (f.dim > 256) {
			_errStack.push_back(sprint("Max supported Dimension from ico is 256px: %d", f.dim));
			return _os;
		}
		unsigned char dim = static_cast<unsigned char>(f.dim);
		if (dim == 256) dim = 0;

		constexpr char FSig[] = {0, 0, 1, 0, 32, 0};
		constexpr unsigned int FSigSize = sizeof(FSig) / sizeof(*FSig);
		PrintLittleEndien(_os, dim, 1);
		PrintLittleEndien(_os, dim, 1);
		_os.write(FSig, FSigSize);
		const unsigned int imgSize = f.pngSize ? f.pngSize.value() 
			: dim * dim * 5 + BMPHeaderSize; // rgba + AND Mask
		
		PrintLittleEndien(_os, imgSize, 4);
		PrintLittleEndien(_os, offset, 4);
		offset += imgSize;
	}
	return _os;
}


/**
	\brief print BMP without FileHedear to file with AND Mask
	\warning only supports 32Bit
*/
std::ostream& PrintBMP(std::ostream& _os, const cimg_library::CImg<px_t>& _img,
	std::vector<std::string>& _errStack) {
	const unsigned int pxNum = _img.height() * _img.width();
	std::vector<unsigned char> andMask((pxNum >> 3) + (pxNum & 0x7 ? 1 : 0), 0);

	if (_img.spectrum() != 4) {
		_errStack.push_back(sprint("only can sava BMP with 4 Channels: %d", _img.spectrum()));
		return _os;
	}

	PrintLittleEndien(_os, 40, 4);
	PrintLittleEndien(_os, _img.width(), 4);
	PrintLittleEndien(_os, _img.height() * 2, 4);
	PrintLittleEndien(_os, 1, 2);
	PrintLittleEndien(_os, 32, 2);
	PrintLittleEndien(_os, 0, 4);
	PrintLittleEndien(_os, 0, 4);
	PrintLittleEndien(_os, 0, 4);
	PrintLittleEndien(_os, 0, 4);
	PrintLittleEndien(_os, 0, 4);

	const px_t* data = _img.data();
	_os.write(reinterpret_cast<const char*>(data), pxNum * 4);
	
	data += 3; // select alpha channel
	std::vector<px_t>::iterator andItr = andMask.begin();
	unsigned char maskStrip = 0;
	for (unsigned int i = 0; i < pxNum; ++i) {
		if (*data == 0) {
			
		}
		if (!(i & 0x7)) {
			*andItr = maskStrip;
			++andItr;
			maskStrip = 0;
		}
		data += 4;
	}
	if (andItr != andMask.end()) {
		*andItr = maskStrip;
		++andItr;
	}
	if (andItr != andMask.end()) {
		_errStack.push_back(sprint("failed to cretae AND Mask: differenz %d", andMask.end() - andItr));
		return _os;
	}
	_os.write(reinterpret_cast<const char*>(andMask.data()), andMask.size());
	return _os;
}

/**
	\brief print img data in file
	\param offset don't print the first x imgs
	\attention only supports BMP export
*/
std::ostream& PrintPics(std::ostream& _os, cimg_library::CImg<px_t>& _img,
	const std::vector<ICOFile>& _pics, std::vector<std::string>& _errStack, unsigned int offset = 0) {
	unsigned int lastDim = std::numeric_limits<unsigned int>::max();
	for (auto itr = _pics.begin() + offset; itr != _pics.end(); ++itr) {
		if (itr->pngSize) {
			_errStack.push_back(sprint("only can export to BMP imgae %d is an PNG", itr - _pics.begin()));
			return _os;
		}
		if (itr->dim > lastDim) {
			_errStack.push_back(sprint("pictures must be printed in decarssing resolution: %d > %d", itr->dim, lastDim));
			return _os;
		}

		// ref http://cimg.eu/reference/structcimg__library_1_1CImg.html#a6a668c8b3f9d756264d1fb31b7a915fc
		// linear interpolation
		_img.resize(static_cast<const int>(itr->dim), static_cast<const int>(itr->dim), 1, 4, 3);

		PrintBMP(_os, _img, _errStack);
		if (!_errStack.empty()) return _os;
	}
	return _os;
}

std::vector<std::string>&& ConvertToICO(const fs::path& _src, const fs::path& _dst, const unsigned int _options) {
	std::vector<std::string> errorStack{};
	// fs::perms prms; TODO: chek access rigts

	if (!fs::exists(_src)) {
		errorStack.push_back(sprint("src file not exist: \"%s\"", _src.c_str()));
	}
	if (!(_options & Options::Overwrite) && fs::exists(_dst)) {
		errorStack.push_back(sprint("dst file already exist, when you wantd to overwrite set tho Overwtie Flag: \"%s\"", _dst.c_str()));
	}
	if (!errorStack.empty()) return std::move(errorStack);

	cimg_library::CImg<px_t> image(_src.string().c_str());
	if (!image) {
		errorStack.push_back(sprint("failed to load imga: \"%s\"", _src.c_str()));
	}
	if (!errorStack.empty()) return std::move(errorStack);

	const int h = image.height();
	const int w = image.width();
	if (w != h || w != 256 || h != 256) {
		errorStack.push_back(sprint("width and height mus be equal 256, w = %d, h = %d", w, h));
	}
	if (!errorStack.empty()) return std::move(errorStack);

	fs::path dst(_dst);
	if (_dst.has_extension())
		dst +=".ico";
	std::ofstream icoFile(dst, std::ios::binary);
	if (!icoFile) {
		errorStack.push_back(sprint("can't open file to write: \"%s\"", dst.c_str()));
	}
	if (!errorStack.empty()) return std::move(errorStack);

	unsigned int srcPngSize = static_cast<unsigned int>(fs::file_size(_src));


	const std::vector<ICOFile> subImgs = {
		ICOFile(256, srcPngSize),
		ICOFile(64),
		ICOFile(48),
		ICOFile(40),
		ICOFile(32),
		ICOFile(24),
		ICOFile(16)
	};

	PrintICOHeader(icoFile, subImgs, errorStack);
	if (!errorStack.empty()) return std::move(errorStack);

	PrintPics(icoFile, image, subImgs, errorStack, 1);
	icoFile.close();
	
	return std::move(errorStack);
}