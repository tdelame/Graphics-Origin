/*  Created on: Dec 17, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_FILESYSTEM_H_
# define GRAPHICS_ORIGIN_FILESYSTEM_H_

# include <graphics_origin.h>
# include <string>
# include <sstream>

BEGIN_GO_NAMESPACE namespace tools {

/** Extract the basename of a filename */
extern
std::string get_basename( const std::string& file);

extern
std::string get_stem(const std::string& file );

extern
std::string get_extension(const std::string& file);

extern
std::string get_directory_name(const std::string& file);

extern
std::string change_extension(const std::string& file, const std::string& newExtension);

extern
bool file_exist(const std::string& filename);

} END_GO_NAMESPACE

#endif
