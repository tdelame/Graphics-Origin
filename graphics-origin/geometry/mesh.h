/*  Created on: Mar 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_MESH_H_
# define GRAPHICS_ORIGIN_MESH_H_
# include "../graphics_origin.h"
# include "triangle.h"
# include "traits.h"
# include "box.h"
# include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace nanoflann {
  template <typename Distance, class DatasetAdaptor,int DIM, typename IndexType>
  class KDTreeSingleIndexAdaptor;

  template<class T, class DataSource, typename _DistanceType>
  struct L2_Simple_Adaptor;
}

BEGIN_GO_NAMESPACE namespace geometry {

  namespace detail {
    struct mesh_traits:
        public OpenMesh::DefaultTraits
    {
      typedef OpenMesh::Vec3d Point;
      typedef OpenMesh::Vec3d Normal;
      typedef OpenMesh::Vec4d Color;

      VertexAttributes   ( ( OpenMesh::Attributes::Normal
                         |   OpenMesh::Attributes::Status
                         |   OpenMesh::Attributes::Color        ) );
      HalfedgeAttributes ( ( OpenMesh::Attributes::PrevHalfedge
                         |   OpenMesh::Attributes::Status
                         |   OpenMesh::Attributes::Normal       ) );
      EdgeAttributes     (   OpenMesh::Attributes::Status         ); //very, very, VERY important: if not, you can't delete a face!
      FaceAttributes     ( ( OpenMesh::Attributes::Status
                         |   OpenMesh::Attributes::Normal
                         |   OpenMesh::Attributes::Color        ) );
    };
  }
  class ray;

  /**@brief A triangular mesh class.
   *
   * This class represent a mesh with triangular faces. This mesh is represented
   * used an half-edge format. This means that an edge has at most two half
   * edges, or, equivalently, that an edge is shared by at most two triangles.
   *
   * This class is based on an OpenMesh class, so you can use any functions of
   * OpenMesh. The only addition is a better I/O system, since on some version of
   * the OpenMesh library, the exporter/importer internal classes of OpenMesh does
   * not compile.
   *
   * Ideally, it would nicer to have vec3 for the position of
   * vertices and geometry::triangle for faces. This would require to rewrite a whole
   * mesh class, which is not a priority for now.
   */
  struct mesh
    : public OpenMesh::TriMesh_ArrayKernelT< detail::mesh_traits > {
    mesh();
    mesh( const std::string& filename );
    /**@brief Clean a mesh.
     *
     * Clean a mesh, i.e. reorganize its internal structures due to the removal
     * of vertices, edges and faces. After this function, handles will be
     * invalidated. */
    void clean();
    bool load( const std::string& filename );
    void save( const std::string& filename );
    void compute_bounding_box( aabox& b ) const;
  };

  /**@brief Check if a filename has a mesh file format extension.
   *
   * Check if a given filename has an extension of a known mesh file format. In
   * particular, this functions checks if the extension is one of the following:
   *  - .off
   *  - .ply
   *  - .obj
   * @param filename The file name to check.
   * @return True if the extension of the file name is either .off, or .ply or
   * .obj. */
  bool has_a_mesh_file_extension( const std::string& filename );

  /**@brief Geometric traits specialization for the mesh class.
   *
   * This is the specialization of the geometric traits for the mesh class.
   * Only the computation of the bounding box is available for this class.
   * Other operations would be too expensive to perform without an optimization
   * structure.
   */
  template <>
  struct geometric_traits<mesh> {
    static const bool is_bounding_box_computer = true;
  };



  template<
    typename bounding_object >
  class bvh;

  /**@brief Implement some operations on a mesh with spatial optimization.
   *
   * This class speed-up some operations done on a mesh thanks to two spatial
   * hierarchies: one kdtree for the indices and one bvh for the faces.
   * Currently, those optimization structures are not recomputed when the mesh
   * changes. So be sure to build an instance of this class once the mesh is
   * fully built (and cleaned).
   *
   * The optimized mesh should be a manifold surface, i.e. it should represent
   * the surface of a solid object. If such condition is not met, an exception
   * is thrown at the construction. */
  class mesh_spatial_optimization {
  public:
    /**@brief Build a spatial optimization of a mesh.
     *
     * Build a new spatial optimization for the given mesh.
     * @param m The mesh to optimize. This mesh should be manifold, otherwise
     * an exception will be thrown.
     * @param build_the_kdtree Tells if the kdtree should be built in the constructor.
     * If this is not the case, be sure to call the function build_ktree() before
     * using a method requiring the kdtree to be built (i.e. get_closest_vertex(),
     * contain(), k_nearest_vertices()).
     * @param build_the_bvh Tells if the BVH should be built in the constructor. If this
     * is not the case, be sure to call the function build_bvh() before using a method
     * requiring the BVH to be built (i.e. intersect() or contain()). */
    mesh_spatial_optimization(
        mesh& m,
        bool build_the_kdtree = true,
        bool build_the_bvh = true);
    /**@brief Destroy the spatial optimization structures.
     *
     * Destroy the mesh spatial optimizations. */
    ~mesh_spatial_optimization();

    /**@brief Optimized functions.
     *
     * Those functions are optimized thanks to two spatial optimization structures:
     * \li a kdtree for the vertices
     * \li a box bvh for the triangles
     * @{
     */
    /**@brief Get the closest vertex near a location.
     *
     * Find the closest vertex from a given location. The functions only uses
     * the kdtree.
     * @param location The location of interest.
     * @param vertex_index Will contain the index of the closest vertex after the call.
     * @param squared_distance_to_vertex Will contain the squared distance to the closest vertex after the call.
     */
    void get_closest_vertex( const vec3& location, size_t& vertex_index, real& squared_distance_to_vertex ) const;

    /**@brief Check if a ray intersects the mesh.
     *
     * Check if a ray intersects the mesh. This functions only uses the BVH.
     * @param r The ray to check.
     * @param t Will store the distance between the ray origin to the closest
     * intersection point if any is found.
     * @return True if an intersection if found. */
    bool intersect( const ray& r, real& t ) const;
    /**@brief Check if a ray intersects the mesh.
     *
     * Check if a ray intersects the mesh and get the index of the face
     * containing the closest intersection point. Note that we ignore if the
     * ray intersects an edge or a vertex. Those cases require to deal with
     * numerical precision and the expect results might change depending on
     * the task. This function only uses the BVH.
     * @param r The ray to test.
     * @param distance_to_mesh The distance between the ray origin and the
     * closest intersection point if any intersection is found.
     * @param closest_face_index Index of the face on which lies the closest
     * intersection point if it exist.
     * @return True if an intersection is found. */
    bool intersect( const ray& r, real& distance_to_mesh, size_t& closest_face_index ) const;

    /**@brief Check if a point is inside the mesh.
     *
     * Check if a point is located inside the mesh. THis functions uses both the
     * kdtree and the BVH.
     * @param p The point to check.
     * @return True if the point is inside the mesh. */
    bool contain( const vec3& p ) const;
    /**@brief Perform a KNN on the vertices.
     *
     * Look for a given number of nearest vertices. This functions uses only the kdtree.
     * @param location The location of interest.
     * @param k The number of neighbors to find.
     * @param indices Pointer to an array with enough place to store the indices of the nearest vertices.
     * @param squared_distances Point to an array with enough place to store the squared distances between
     * the nearest vertices and the location of interest. */
    void k_nearest_vertices( const vec3& location, uint32_t k, size_t* indices, real* squared_distances );

    /**@brief Optimized functions.
     *
     * Those functions are required by the nanoflann library to build a kdtree
     * on custom geometric data.
     * @{
     */
    /**@brief Get the number of points inside the kdtree.
     *
     * This function is required by the nanoflann library to build a kdtree
     * on custom geometric data. It simply returns the number of vertices to
     * store in the ktree, i.e. the number of mesh vertices.
     * @return The number of vertices in the mesh. */
    inline size_t kdtree_get_point_count() const
    {
      return m_mesh.n_vertices();
    }

    /**@brief Get the kdtree distance between two points.
     *
     * This function is required by the nanoflann library to build a kdtree
     * on custom geometric data. It computes the distance between a given point
     * and a point stored in the kdtree.
     * @param a Pointer to three consecutive real values representing the first point.
     * @param idx The index of the second point in the kdtree.
     * @param size The dimension of points in the kdtree (3 here).
     * @return The squared distance between the two given points. */
    inline real kdtree_distance(const real* a, const size_t b_idx, size_t size) const
    {
      (void)size;
      const real* b = get_point( b_idx );
      real result = a[0] - b[0];
      result *= result;
      real temp = a[1] - b[1];
      temp *= temp;
      result += temp;
      temp = a[2] - b[2];
      temp *= temp;
      result += temp;
      return result;
    }

    /**@brief Get the component of a point stored in the kdtree.
     *
     * This function is required by the nanoflann library to build a kdtree
     * on custom geometric data. It fetches the component of a point stored
     * in the kdtree, which is here the position of a mesh vertex.
     * @param idx Index of the point to fetch.
     * @param component Component index to fetch, i.e. 0 for X, 1 for Y and
     * 2 for Z.
     * @return The component-th component of the vertex of index idx. */
    inline const real& kdtree_get_pt(const size_t idx, int component) const
    {
      return m_points[ idx * 3 + component ];
    }

    /**@brief Utilities functions.
     * @{
     */

    /**@brief Access to a vertex position thanks to its index.
     *
     * Get the position of a vertex.
     * @param idx Index of the vertex.
     * @return A pointer to an array of three real values representing the
     * position of the vertex. */
    inline const real* get_point( const uint32_t idx ) const
    {
      return m_points + idx * 3;
    }

    /**@brief Access to a vertex normal thanks to its index.
     *
     * Get the normal of a vertex.
     * @param idx Index of the vertex.
     * @return A pointer to an array of three real values representing the
     * normal of the vertex. */
    inline const real* get_normal( const uint32_t idx ) const
    {
      return m_normals + idx * 3;
    }

    /**@brief Get the number of triangles.
     *
     * Get the number of triangles in the underlying mesh.
     * @return The number of triangles.
     */
    inline size_t get_number_of_triangles() const noexcept
    {
      return m_triangles.size();
    }

    /**@brief Access to a triangle thanks to its index.
     *
     * Get a triangle. Triangles are initialized even if the BVH is not built.
     * @param idx Index of the triangle.
     * @return The requested triangle. */
    inline const triangle& get_triangle( const uint32_t idx ) const
    {
      return m_triangles[ idx ];
    }

    /**@brief Get the bounding box the mesh.
     *
     * This function is required by the nanoflann library to build a kdtree
     * on custom geometric data. It asks for the bounding box of the underlying
     * data. If no bounding box is found (return value is false), the nanoflann
     * library will compute the bounding box.
     * @param bb The bounding box requested.
     * @return True if the bounding box is set by this function. */
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& bb) const
    {
      auto low  = bounding_box.get_min();
      auto high = bounding_box.get_max();
      for( size_t d = 0; d < 3; ++ d )
       {
          bb[ d ].low  =  low[d];
          bb[ d ].high = high[d];
       }
      return true;
    }

    /**@brief Access to the bvh.
     *
     * Get the bvh built with axis aligned boxes as bounding objects and the
     * triangles of the mesh as bounded objects.
     * @return The bvh. */
    bvh<aabox>* get_bvh();

    /**@brief Access to the mesh.
     *
     * Get the mesh spatially optimized by this.
     * @return The mesh. */
    mesh& get_geometry();

    /**@brief Access to the mesh's bounding box.
     *
     * Get the bounding box of the mesh.
     * @return The bounding box. */
    const aabox& get_bounding_box() const;

    void build_kdtree();

    void build_bvh();

  private:
    aabox bounding_box;
    std::vector< triangle > m_triangles;
    const real* m_points;
    const real* m_normals;
    mesh& m_mesh;
    nanoflann::KDTreeSingleIndexAdaptor<
     nanoflann::L2_Simple_Adaptor< real, mesh_spatial_optimization, real >,
     mesh_spatial_optimization,
     3, size_t
     >* m_kdtree;
    bvh<aabox>* m_bvh;
  };

  template <>
  struct geometric_traits<mesh_spatial_optimization> {
    static const bool is_ray_intersecter = true;
    static const bool is_point_container = true;
  };
} END_GO_NAMESPACE
# endif
