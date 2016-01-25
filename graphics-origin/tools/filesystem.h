/*  Created on: Dec 17, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_FILESYSTEM_H_
# define GRAPHICS_ORIGIN_FILESYSTEM_H_

# include "../graphics_origin.h"
# include <string>
# include <sstream>

/** @file filesystem.h
 * A set of basic functions to deal with the file system. More evolved API
 * could be used, such as boost::filesystem which is used here in the
 * implementation.
 */

BEGIN_GO_NAMESPACE namespace tools {

/** Extract the base name of a file name, e.g.:
 * <tt>get_basename( "/etc/pacman.conf" )</tt> returns "pacman.conf"
 * @param filename The file name
 **/
extern
std::string get_basename( const std::string& filename );

/** Extract the stem of a file name, e.g.:
 * <tt>get_basename( "/etc/pacman.conf" )</tt> returns "pacman"
 * @param filename The file name
 */
extern
std::string get_stem(const std::string& filename );

/** Extract the extension of a file name, e.g.:
 * <tt>get_basename( "/etc/pacman.conf" )</tt> returns ".conf"
 * @param filename The file name
 */
extern
std::string get_extension( const std::string& filename );

/** Extract the directory of a file name, e.g.:
 * <tt>get_basename( "/etc/pacman.conf" )</tt> returns "/etc/"
 * @param filename The file name
 */
extern
std::string get_directory_name( const std::string& filename );

/** Change the extension of a file name, e.g.:
 * \li <tt>change_extension( "/etc/pacman.conf", "old") </tt> returns "/etc/pacman.old"
 * \li <tt>change_extension( "/etc/pacman.conf", ".new")</tt> returns "/etc/pacman.new"
 */
extern
std::string change_extension( const std::string& file, const std::string& new_extension);

/** Tells if a (regular) file exist.
 * @param filename The file name
 */
extern
bool file_exist(const std::string& filename);

} END_GO_NAMESPACE

#endif
