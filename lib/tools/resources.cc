/*  Created on: May 16, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "../../graphics-origin/tools/resources.h"
# include <boost/filesystem.hpp>
BEGIN_GO_NAMESPACE
  namespace tools
  {
    static std::string ensure_last_char_is_slash( const std::string& path )
    {
      if( path.empty() || path.back() == '/' )
        return path;
      return path + '/';
    }

    std::string
    path_manager::get_root_directory () const noexcept
    {
      return m_root_directory;
    }

    void
    path_manager::set_root_directory( const std::string& root ) noexcept
    {
      m_root_directory = ensure_last_char_is_slash(root);
    }

    std::string
    path_manager::get_resource_directory( const std::string& resource_name ) const noexcept
    {
      auto find = m_paths.find( resource_name );
      if( find == m_paths.end() )
        return m_root_directory + resource_name + '/';
      return find->second;
    }

    void
    path_manager::set_relative_resource_directory( const std::string& resource_name ) noexcept
    {
      m_paths.erase( resource_name );
    }

    void
    path_manager::set_absolute_resource_path(
        const std::string& resource_name,
        const std::string& resource_path ) noexcept
    {
      m_paths[resource_name] = ensure_last_char_is_slash(resource_path);
    }


    path_manager& get_path_manager()
    {
      static path_manager instance;
      return instance;
    }


  }
END_GO_NAMESPACE

