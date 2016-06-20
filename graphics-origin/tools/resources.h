/*  Created on: May 16, 2016
 *      Author: T. Delame (tdelame@gmail.com) */

# ifndef RESOURCES_H_
# define RESOURCES_H_
# include "../graphics_origin.h"
# include <string>
# include <unordered_map>

BEGIN_GO_NAMESPACE namespace tools {

  /**
   * Paths can be given in two different ways:
   * - relatively to a root directory
   * - in an absolute way.
   *
   * For simplicity purpose, all path are initially given relatively to a root
   * directory, which is the working directory. This root directory can be after
   * modified, to take into account a command line argument for example. Then,
   * paths can be given in an absolute way.
   */
  class GO_API path_manager {
  public:

    /**@brief Access to the resource root directory.
     *
     * Get the resources root directory, that is the directory in which resource
     * directories are found, e.g. images/, meshes/ and so on.
     *
     * @return The resources root directory.
     */
    std::string get_root_directory() const noexcept;

    /**@brief Set the resource root directory.
     *
     * Set a new resource root directory.
     * @param root The new resource root directory.
     */
    void
    set_root_directory(
        const std::string& root ) noexcept;

    /**@brief Set a resource directory relatively to the root directory.
     *
     * Set the directory for a resource relatively to the root directory.
     * @param resource_name Name of the resource.
     */
    void
    set_relative_resource_directory(
        const std::string& resource_name ) noexcept;

    /**@brief Set a resource directory by an absolute path.
     *
     * Set the directory for a resource by an absolute path.
     * @param resource_name Name of the resource.
     * @param resource_path Absolute path of the new directory for this resource.
     */
    void
    set_absolute_resource_path(
        const std::string& resource_name,
        const std::string& resource_path ) noexcept;

    /**@brief Get the directory of a resource.
     *
     * Get the path of the directory in which resources of a specific type are
     * stored.
     * @param resource_name Name of the resource type.
     * @return The path in which are stored resources of that type.
     */
    std::string
    get_resource_directory( const std::string& resource_name ) const noexcept;
  private:
    std::string m_root_directory;
    std::unordered_map< std::string, std::string > m_paths;
  };

  /**@brief Get the singleton of path manager.
   *
   * Access to a unique instance of a path manager, i.e. a singleton.
   * @return The unique instance.
   */
  GO_API
  path_manager& get_path_manager();

} END_GO_NAMESPACE
# endif /* RESOURCES_H_ */
