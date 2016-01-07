/*  Created on: Dec 17, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */

# include <tools/filesystem.h>
# include <boost/filesystem.hpp>
BEGIN_GO_NAMESPACE namespace tools {
std::string
get_basename(const std::string& filename)
{
  return (boost::filesystem::path(filename).filename().string());
}

std::string
get_stem(const std::string& filename )
{
  return( boost::filesystem::path(filename).stem().string());
}

std::string
get_extension(const std::string& filename )
{
  return (boost::filesystem::path(filename).extension().string());
}

std::string
get_directory_name(const std::string& filename)
{
  return (boost::filesystem::path(filename).parent_path().string());
}

std::string
change_extension(const std::string& filename, const std::string& new_extension)
{
  return (boost::filesystem::change_extension(boost::filesystem::path(filename),
      new_extension).string());
}

bool
file_exist(const std::string& filename)
{
  boost::filesystem::path path(filename);
  return (boost::filesystem::is_regular_file(path)
      && boost::filesystem::exists(path));
}
} END_GO_NAMESPACE
