/*  Created on: Mar 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_MESH_H_
# define GRAPHICS_ORIGIN_MESH_H_
# include "../graphics_origin.h"
# include "traits.h"
# include "bvh.h"
# include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
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

  class aabox;
  class triangle;
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
   * Ideally, it would more nice and efficient to have vec3 for the position of
   * vertices and geometry::triangle for faces. This would require to rewrite a whole
   * mesh class, which is not a priority for now.
   */
  struct mesh
    : public OpenMesh::TriMesh_ArrayKernelT< detail::mesh_traits > {
    mesh();
    mesh( const std::string& filename );
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
   * \todo Create a wrapper class of a mesh and an optimization structure to
   * enable other geometric traits.
   */
  template <>
  struct geometric_traits<mesh> {
    static const bool is_bounding_box_computer = true;
  };

  class mesh_spatial_optimization {
  public:
    mesh_spatial_optimization( mesh& m );
    ~mesh_spatial_optimization();
    bool intersect( const ray& r, real& t ) const;
    bool contain( const vec3& p ) const;
  private:

    struct nanoflann_adaptator {
      // Must return the number of data points
      inline size_t kdtree_get_point_count() const
      {
        return m_size;
      }

      inline real kdtree_distance(const real* a, const size_t b_idx, size_t size) const
      {
       real result = 0;
       real* b = pts + b_idx * size;
       for( size_t i = 0; i < size; ++ i, ++ b )
         {
           real temp = a[i] - *b;
           temp *= temp;
           result += temp;
         }
       return result;
      }


      inline real kdtree_get_pt(const size_t idx, int component) const
      {
       return pts[ idx * dimension + component ];
      }

      // Optional bounding-box computation: return false to default to a standard bbox computation loop.
      //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
      //   Look at bb.m_size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)

      template <class BBOX>
      bool kdtree_get_bbox(BBOX& bb) const
      {
        auto low  = bounding_box.get_min();
        auto high = bounding_box.get_min();
        for( size_t d = 0; d < dimension; ++ d )
         {
            bb[ d ].low  =  low[d];
            bb[ d ].high = high[d];
         }
        return true;
      }
      aabox bounding_box;
      real* pts;
      const size_t m_size;
      static const size_t dimension;
    };

    std::vector< triangle > m_triangles;
    std::vector< vec3 > m_vertices;
    bvh<aabox>* m_bvh;
    mesh& m_mesh;
  };

  template <>
  struct geometric_traits<mesh_spatial_optimization> {
    static const bool is_ray_intersecter = true;
    static const bool is_point_container = true;
  };
} END_GO_NAMESPACE
# endif
