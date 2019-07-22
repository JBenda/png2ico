#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

/** 
 \brief converts PNG with size 256x256 to ICO (windows icon) file
 \param _src path to png file
 \param _dst path where ico file schould be saved
 \param _options additonal options for more detalis see ::Options
 \return error stack, size = 0 -> no error occure
 \attention png must have the size 256x256

 + if _dst has no extension it's will saved as ".ico"
*/
std::vector<std::string>&& ConvertToICO(const fs::path& _src, const fs::path& _dst, const unsigned int _options = 0);

/**
	\brief Conversion Options
*/
enum Options : unsigned int {
	Overwrite = 0b1 /*!< if _dst already esxist overwite it*/
};