#ifndef UTILS_FILE_HPP
#define UTILS_FILE_HPP

#include <string>

namespace files {

/*! Read a file
 *  \param filepath to access the file
 *  \param result   [out] where to store content of the read file
 *  \return true if everything was ok, false otherwise
 *  \warning the content of the result will be override
 */
std::string read(const std::string& filepath);

bool create( const std::string& filepath, const std::string& content );

}

#endif // UTILS_FILE_HPP
