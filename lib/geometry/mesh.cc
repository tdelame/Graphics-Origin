/*  Created on: Mar 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/mesh.h"
# include "../../graphics-origin/geometry/box.h"
# include "../../graphics-origin/geometry/triangle.h"
# include "../../graphics-origin/geometry/ray.h"
# include "../../graphics-origin/tools/filesystem.h"

# include <OpenMesh/Core/IO/exporter/BaseExporter.hh>
# include <OpenMesh/Core/IO/IOManager.hh>

BEGIN_GO_NAMESPACE namespace geometry {

  static const std::string obj_file_extension = ".obj";
  static const std::string off_file_extension = ".off";
  static const std::string ply_file_extension = ".ply";

  static unsigned char
  cc_to_uc( const double& value )
  {
    return (uint)(value*255.0);
  }

  static unsigned int
  cc_to_ui( const double& value )
  {
    return (uint)(value*65535);
  }

  class ExporterT : public OpenMesh::IO::BaseExporter {
  public:
    virtual ~ExporterT(){}

    // Constructor
    ExporterT(const mesh& _mesh) : mesh_(_mesh) {}


    // get vertex data

    OpenMesh::Vec3f point( mesh::VertexHandle _vh)    const
    {
      auto point = mesh_.point(_vh);
      return OpenMesh::Vec3f( point[0], point[1], point[2] );
    }

    OpenMesh::Vec3f  normal(mesh::VertexHandle _vh)   const
    {
      auto p = mesh_.normal(_vh);
      return OpenMesh::Vec3f( p[0], p[1], p[2] );
    }

    OpenMesh::Vec3uc color(mesh::VertexHandle _vh)    const
    {
      auto c = mesh_.color(_vh);
      return OpenMesh::Vec3uc( cc_to_uc(c[0]), cc_to_uc(c[1]), cc_to_uc(c[2]));
    }

    OpenMesh::Vec4uc colorA(mesh::VertexHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4uc( cc_to_uc(c[0]), cc_to_uc(c[1]), cc_to_uc(c[2]), cc_to_uc(c[3]));;
    }

    OpenMesh::Vec3ui colori(mesh::VertexHandle h)    const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3ui( cc_to_ui(c[0]), cc_to_ui(c[1]), cc_to_ui(c[2]));
    }

    OpenMesh::Vec4ui colorAi(mesh::VertexHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4ui( cc_to_ui(c[0]), cc_to_ui(c[1]), cc_to_ui(c[2]), cc_to_ui(c[3]));;
    }

    OpenMesh::Vec3f colorf(mesh::VertexHandle h)    const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3f( c[0], c[1], c[2] );
    }

    OpenMesh::Vec4f colorAf(mesh::VertexHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4f( c[0], c[1], c[2], c[3] );
    }

    OpenMesh::Vec2f  texcoord(mesh::VertexHandle _vh) const
    {
      return mesh_.texcoord2D(_vh);
    }

    // get edge data

    OpenMesh::Vec3uc color(mesh::EdgeHandle h)    const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3uc( cc_to_uc(c[0]), cc_to_uc(c[1]), cc_to_uc(c[2]));
    }

    OpenMesh::Vec4uc colorA(mesh::EdgeHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4uc( cc_to_uc(c[0]), cc_to_uc(c[1]), cc_to_uc(c[2]), cc_to_uc(c[3]));;
    }

    OpenMesh::Vec3ui colori(mesh::EdgeHandle h)    const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3ui( cc_to_ui(c[0]), cc_to_ui(c[1]), cc_to_ui(c[2]));
    }

    OpenMesh::Vec4ui colorAi(mesh::EdgeHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4ui( cc_to_ui(c[0]), cc_to_ui(c[1]), cc_to_ui(c[2]), cc_to_ui(c[3]));;
    }

    OpenMesh::Vec3f colorf(mesh::EdgeHandle h)    const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3f( c[0], c[1], c[2] );
    }

    OpenMesh::Vec4f colorAf(mesh::EdgeHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4f( c[0], c[1], c[2], c[3] );;
    }

    // get face data

    unsigned int get_vhandles(mesh::FaceHandle _fh,
            std::vector<mesh::VertexHandle>& _vhandles) const
    {
      unsigned int count(0);
      _vhandles.clear();
      for (typename mesh::CFVIter fv_it=mesh_.cfv_iter(_fh); fv_it.is_valid(); ++fv_it)
      {
        _vhandles.push_back(*fv_it);
        ++count;
      }
      return count;
    }

    OpenMesh::Vec3f  normal(mesh::FaceHandle h)   const
    {
      auto n = mesh_.normal(h);
      return OpenMesh::Vec3f( n[0], n[1], n[2] );
    }

    OpenMesh::Vec3uc  color(mesh::FaceHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3uc( cc_to_uc(c[0]), cc_to_uc(c[1]), cc_to_uc(c[2]));
    }

    OpenMesh::Vec4uc  colorA(mesh::FaceHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4uc( cc_to_uc(c[0]), cc_to_uc(c[1]), cc_to_uc(c[2]), cc_to_uc(c[3]));;
    }

    OpenMesh::Vec3ui  colori(mesh::FaceHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3ui( cc_to_ui(c[0]), cc_to_ui(c[1]), cc_to_ui(c[2]));
    }

    OpenMesh::Vec4ui  colorAi(mesh::FaceHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4ui( cc_to_ui(c[0]), cc_to_ui(c[1]), cc_to_ui(c[2]), cc_to_ui(c[3]));;
    }

    OpenMesh::Vec3f colorf(mesh::FaceHandle h)    const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec3f( c[0], c[1], c[2] );
    }

    OpenMesh::Vec4f colorAf(mesh::FaceHandle h)   const
    {
      auto c = mesh_.color(h);
      return OpenMesh::Vec4f( c[0], c[1], c[2], c[3] );;
    }

    virtual const OpenMesh::BaseKernel* kernel() { return &mesh_; }


    // query number of faces, vertices, normals, texcoords
    size_t n_vertices()  const { return mesh_.n_vertices(); }
    size_t n_faces()     const { return mesh_.n_faces(); }
    size_t n_edges()     const { return mesh_.n_edges(); }


    // property information
    bool is_triangle_mesh() const
    { return mesh::is_triangles(); }

    bool has_vertex_normals()   const { return mesh_.has_vertex_normals();   }
    bool has_vertex_colors()    const { return mesh_.has_vertex_colors();    }
    bool has_vertex_texcoords() const { return mesh_.has_vertex_texcoords2D(); }
    bool has_edge_colors()      const { return mesh_.has_edge_colors();      }
    bool has_face_normals()     const { return mesh_.has_face_normals();     }
    bool has_face_colors()      const { return mesh_.has_face_colors();      }

  private:

     const mesh& mesh_;
  };


  static const real inv_255 = real( 1.0 /  255.0 );

  static real
  cc_from_uc( const unsigned char& value )
  {
    return real( value ) * inv_255;
  }

  class ImporterT : public OpenMesh::IO::BaseImporter
  {
  public:

    typedef typename mesh::Point       Point;
    typedef typename mesh::Normal      Normal;
    typedef typename mesh::Color       Color;
    typedef typename mesh::TexCoord2D  TexCoord2D;
    typedef typename mesh::VertexHandle VertexHandle;
    typedef typename mesh::EdgeHandle EdgeHandle;
    typedef typename mesh::HalfedgeHandle HalfedgeHandle;
    typedef typename mesh::FaceHandle FaceHandle;
    typedef std::vector<VertexHandle>  VHandles;


    ImporterT(mesh& _mesh) : mesh_(_mesh), halfedgeNormals_() {}


    virtual VertexHandle add_vertex(const OpenMesh::Vec3f& p)
    {
      return mesh_.add_vertex( Point(p[0], p[1], p[2] ) );
    }

    virtual VertexHandle add_vertex()
    {
      return mesh_.new_vertex();
    }

    virtual FaceHandle add_face(const VHandles& _indices)
    {
      FaceHandle fh;

      if (_indices.size() > 2)
      {
        VHandles::const_iterator it, it2, end(_indices.end());


        // test for valid vertex indices
        for (it=_indices.begin(); it!=end; ++it)
          if (! mesh_.is_valid_handle(*it))
          {
            omerr() << "ImporterT: Face contains invalid vertex index\n";
            return fh;
          }


        // don't allow double vertices
        for (it=_indices.begin(); it!=end; ++it)
          for (it2=it+1; it2!=end; ++it2)
            if (*it == *it2)
            {
              omerr() << "ImporterT: Face has equal vertices\n";
              failed_faces_.push_back(_indices);
              return fh;
            }


        // try to add face
        fh = mesh_.add_face(_indices);
        if (!fh.is_valid())
        {
          failed_faces_.push_back(_indices);
          return fh;
        }

        //write the half edge normals
        if (mesh_.has_halfedge_normals())
        {
          //iterate over all incoming haldedges of the added face
          for (typename mesh::FaceHalfedgeIter fh_iter = mesh_.fh_begin(fh);
              fh_iter != mesh_.fh_end(fh); ++fh_iter)
          {
            //and write the normals to it
            typename mesh::HalfedgeHandle heh = *fh_iter;
            typename mesh::VertexHandle vh = mesh_.to_vertex_handle(heh);
            typename std::map<VertexHandle,Normal>::iterator it_heNs = halfedgeNormals_.find(vh);
            if (it_heNs != halfedgeNormals_.end())
              mesh_.set_normal(heh,it_heNs->second);
          }
          halfedgeNormals_.clear();
        }
      }
      return fh;
    }

    // vertex attributes

    virtual void set_point(VertexHandle _vh, const OpenMesh::Vec3f& p)
    {
      mesh_.set_point(_vh, Point( p[0], p[1], p[3] ) );
    }

    virtual void set_normal(VertexHandle _vh, const OpenMesh::Vec3f& n )
    {
      auto normal = Normal( n[0], n[1], n[3] );
      if (mesh_.has_vertex_normals())
        mesh_.set_normal(_vh, normal );

      //saves normals for half edges.
      //they will be written, when the face is added
      if (mesh_.has_halfedge_normals())
        halfedgeNormals_[_vh] = normal;
    }

    virtual void set_color(VertexHandle _vh, const OpenMesh::Vec4uc& c )
    {
      if (mesh_.has_vertex_colors())
        mesh_.set_color( _vh, Color( cc_from_uc(c[0]), cc_from_uc(c[1]), cc_from_uc(c[2]), cc_from_uc(c[3]) ) );
    }

    virtual void set_color(VertexHandle _vh, const OpenMesh::Vec3uc& c)
    {
      if (mesh_.has_vertex_colors())
        mesh_.set_color( _vh, Color( cc_from_uc(c[0]), cc_from_uc(c[1]), cc_from_uc(c[2]), 1.0 ) );
    }

    virtual void set_color(VertexHandle h, const OpenMesh::Vec4f& c )
    {
      if (mesh_.has_vertex_colors())
        mesh_.set_color( h, Color( c[0], c[1], c[2], c[3] ) );
    }

    virtual void set_color(VertexHandle _vh, const OpenMesh::Vec3f& c)
    {
      if (mesh_.has_vertex_colors())
        mesh_.set_color( _vh, Color( c[0], c[1], c[2], 1.0 ) );
    }

    virtual void set_texcoord(VertexHandle _vh, const OpenMesh::Vec2f& _texcoord)
    {
      if (mesh_.has_vertex_texcoords2D())
        mesh_.set_texcoord2D(_vh, _texcoord );
    }

    virtual void set_texcoord(HalfedgeHandle _heh, const OpenMesh::Vec2f& _texcoord)
    {
      if (mesh_.has_halfedge_texcoords2D())
        mesh_.set_texcoord2D(_heh, _texcoord );
    }

    // edge attributes

    virtual void set_color(EdgeHandle h, const OpenMesh::Vec4uc& c)
    {
        if (mesh_.has_edge_colors())
          mesh_.set_color( h, Color( cc_from_uc(c[0]), cc_from_uc(c[1]), cc_from_uc(c[2]), cc_from_uc(c[3]) ) );
    }

    virtual void set_color(EdgeHandle h, const OpenMesh::Vec3uc& c)
    {
        if (mesh_.has_edge_colors())
          mesh_.set_color( h, Color( cc_from_uc(c[0]), cc_from_uc(c[1]), cc_from_uc(c[2]), 1.0 ) );
    }

    virtual void set_color(EdgeHandle h, const OpenMesh::Vec4f& c)
    {
        if (mesh_.has_edge_colors())
          mesh_.set_color( h, Color( c[0], c[1], c[2], c[3] ) );
    }

    virtual void set_color(EdgeHandle h, const OpenMesh::Vec3f& c)
    {
        if (mesh_.has_edge_colors())
          mesh_.set_color( h, Color( c[0], c[1], c[2], 1.0 ) );
    }

    // face attributes

    virtual void set_normal(FaceHandle h, const OpenMesh::Vec3f& n)
    {
      if (mesh_.has_face_normals())
        mesh_.set_normal( h, Normal( n[0], n[1], n[2] ) );
    }

    virtual void set_color(FaceHandle h, const OpenMesh::Vec3uc& c)
    {
      if (mesh_.has_face_colors())
        mesh_.set_color( h, Color( cc_from_uc(c[0]), cc_from_uc(c[1]), cc_from_uc(c[2]), 1.0 ) );
    }

    virtual void set_color(FaceHandle h, const OpenMesh::Vec4uc& c)
    {
      if (mesh_.has_face_colors())
        mesh_.set_color( h, Color( cc_from_uc(c[0]), cc_from_uc(c[1]), cc_from_uc(c[2]), cc_from_uc(c[3]) ) );
    }

    virtual void set_color(FaceHandle h, const OpenMesh::Vec3f& c)
    {
      if (mesh_.has_face_colors())
        mesh_.set_color( h, Color( c[0], c[1], c[2], 1.0 ) );
    }

    virtual void set_color(FaceHandle h, const OpenMesh::Vec4f& c)
    {
      if (mesh_.has_face_colors())
        mesh_.set_color( h, Color( c[0], c[1], c[2], c[3] ) );
    }

    virtual void add_face_texcoords( FaceHandle _fh, VertexHandle _vh, const std::vector<OpenMesh::Vec2f>& _face_texcoords)
    {
      // get first halfedge handle
      HalfedgeHandle cur_heh   = mesh_.halfedge_handle(_fh);
      HalfedgeHandle end_heh   = mesh_.prev_halfedge_handle(cur_heh);

      // find start heh
      while( mesh_.to_vertex_handle(cur_heh) != _vh && cur_heh != end_heh )
        cur_heh = mesh_.next_halfedge_handle( cur_heh);

      for(unsigned int i=0; i<_face_texcoords.size(); ++i)
      {
        set_texcoord( cur_heh, _face_texcoords[i]);
        cur_heh = mesh_.next_halfedge_handle( cur_heh);
      }
    }

    virtual void set_face_texindex( FaceHandle _fh, int _texId ) {
      if ( mesh_.has_face_texture_index() ) {
        mesh_.set_texture_index(_fh , _texId);
      }
    }

    virtual void add_texture_information( int _id , std::string _name ) {
      OpenMesh::MPropHandleT< std::map< int, std::string > > property;

      if ( !mesh_.get_property_handle(property,"TextureMapping") ) {
        mesh_.add_property(property,"TextureMapping");
      }

      if ( mesh_.property(property).find( _id ) == mesh_.property(property).end() )
        mesh_.property(property)[_id] = _name;
    }

    // low-level access to mesh

    virtual OpenMesh::BaseKernel* kernel() { return &mesh_; }

    bool is_triangle_mesh() const
    { return mesh::is_triangles(); }

    void reserve(unsigned int nV, unsigned int nE, unsigned int nF)
    {
      mesh_.reserve(nV, nE, nF);
    }

    // query number of faces, vertices, normals, texcoords
    size_t n_vertices()  const { return mesh_.n_vertices(); }
    size_t n_faces()     const { return mesh_.n_faces(); }
    size_t n_edges()     const { return mesh_.n_edges(); }


    void prepare() { failed_faces_.clear(); }


    void finish()
    {
      if (!failed_faces_.empty())
      {
        omerr() << failed_faces_.size()
        << " faces failed, adding them as isolated faces\n";

        for (unsigned int i=0; i<failed_faces_.size(); ++i)
        {
          VHandles&  vhandles = failed_faces_[i];

          // double vertices
          for (unsigned int j=0; j<vhandles.size(); ++j)
          {
            Point p = mesh_.point(vhandles[j]);
            vhandles[j] = mesh_.add_vertex(p);
            // DO STORE p, reference may not work since vertex array
            // may be relocated after adding a new vertex !

            // Mark vertices of failed face as non-manifold
            if (mesh_.has_vertex_status()) {
                mesh_.status(vhandles[j]).set_fixed_nonmanifold(true);
            }
          }

          // add face
          FaceHandle fh = mesh_.add_face(vhandles);

          // Mark failed face as non-manifold
          if (mesh_.has_face_status())
              mesh_.status(fh).set_fixed_nonmanifold(true);

          // Mark edges of failed face as non-two-manifold
          if (mesh_.has_edge_status()) {
              typename mesh::FaceEdgeIter fe_it = mesh_.fe_iter(fh);
              for(; fe_it.is_valid(); ++fe_it) {
                  mesh_.status(*fe_it).set_fixed_nonmanifold(true);
              }
          }
        }

        failed_faces_.clear();
      }
    }



  private:

    mesh& mesh_;
    std::vector<VHandles>  failed_faces_;
    // stores normals for halfedges of the next face
    std::map<VertexHandle,Normal> halfedgeNormals_;
  };

  mesh::mesh()
    : OpenMesh::TriMesh_ArrayKernelT< detail::mesh_traits > ()
  {}

  mesh::mesh( const std::string& filename )
  {
    load( filename );
  }

  bool
  mesh::load( const std::string& filename )
  {
    clear();
    ImporterT importer(*this);
    auto option = OpenMesh::IO::Options(); //todo: set correctly the options!
    if( OpenMesh::IO::IOManager().read( filename, importer,  option) )
      {
        update_normals();
        return true;
      }
    return false;
  }

  void
  mesh::save( const std::string& filename )
  {
    ExporterT exporter(*this);
    OpenMesh::IO::IOManager().write( filename, exporter );
  }

  void
  mesh::compute_bounding_box( aabox& b ) const
  {
    # pragma omp declare reduction(minmeshpoint: mesh::Point: omp_out.minimize( omp_in )) \
      initializer(omp_priv = mesh::Point{REAL_MAX,REAL_MAX,REAL_MAX})
    # pragma omp declare reduction(maxmeshpoint: mesh::Point: omp_out.maximize( omp_in )) \
      initializer(omp_priv = mesh::Point{-REAL_MAX,-REAL_MAX,-REAL_MAX})
    mesh::Point minp = mesh::Point{REAL_MAX,REAL_MAX,REAL_MAX};
    mesh::Point maxp = mesh::Point{-REAL_MAX,-REAL_MAX,-REAL_MAX};
    const auto size = n_vertices();
    # pragma omp parallel for reduction(minmeshpoint:minp) reduction(maxmeshpoint:maxp)
    for( size_t i = 0; i < size; ++ i )
      {
        const auto& p = point( VertexHandle(i) );
        minp.minimize( p );
        maxp.maximize( p );
      }
    b = aabox( vec3{ minp[0], minp[1], minp[2]}, vec3{ maxp[0], maxp[1], maxp[2] } );
  }

  bool
  has_a_mesh_file_extension( const std::string& filename )
  {
    auto ext = graphics_origin::tools::get_extension( filename );
    return ext == obj_file_extension || ext == off_file_extension
        || ext == ply_file_extension;
  }

  mesh_spatial_optimization::~mesh_spatial_optimization()
  {
    delete m_bvh;
  }

  mesh_spatial_optimization::mesh_spatial_optimization( mesh& m )
    : m_mesh{ m }, m_kdtree{ 3, *this, nanoflann::KDTreeSingleIndexAdaptorParams{32} },
      m_triangles( m.n_faces() )
  {
    const auto nfaces  = m_triangles.size();
    # pragma omp parallel for schedule(static)
    for( size_t i = 0; i < nfaces; ++ i )
      {
        mesh::FaceVertexIter it = m.fv_begin( mesh::FaceHandle(i) );
        auto& p1 = m_mesh.point( *it ); ++ it;
        auto& p2 = m_mesh.point( *it ); ++ it;
        auto& p3 = m_mesh.point( *it );

        m_triangles[ i ] = triangle(
            vec3{ p1[0], p1[1], p1[2] },
            vec3{ p2[0], p2[1], p2[2] },
            vec3{ p3[0], p3[1], p3[2] } );
      }

    m_mesh.compute_bounding_box( bounding_box );
//    LOG( debug, "bounding box = " << bounding_box.get_min() << " -- " << bounding_box.get_max() );
    m_bvh = new bvh<aabox>( m_triangles.data(), bounding_box, nfaces );
    m_kdtree.buildIndex();
  }

  bool
  mesh_spatial_optimization::intersect( const ray& r, real& t ) const
  {
    size_t dummy = 0;
//    t = REAL_MAX;
    return intersect( r, t, dummy );
  }

  bool
  mesh_spatial_optimization::intersect( const ray& r, real& distance_to_mesh, size_t& closest_face_index ) const
  {
    bool result = false;
    ray_with_inv_dir inv_r( r );

    std::vector< std::pair<uint32_t, real> > stack;
    stack.reserve( 64 );
    stack.push_back( std::make_pair( uint32_t(0), real(-1) ) );
    uint32_t node_index = 0;
    do
      {
        real t1 = REAL_MAX;
        real t2 = REAL_MAX;
        auto& node = m_bvh->get_node( node_index );
        bool leafL = m_bvh->is_leaf( node.left_index );
        bool leafR = m_bvh->is_leaf( node.right_index );

        bool overlapL = leafL ?
            m_triangles[ m_bvh->get_node( node.left_index ).element_index ].intersect( r, t1 )
          : m_bvh->get_node( node.left_index ).bounding.intersect( inv_r, t1 )
          ;

//        if( !overlapL )
//          {
//            if( leafL )
//              {
//                LOG( debug, "detected that leaf " << intern.left_index << " "
//                     << m_bvh->get_leaf_node( intern.left_index & bvh_leaf_index_mask ).bounding.get_min() << " -- "
//                     << m_bvh->get_leaf_node( intern.left_index & bvh_leaf_index_mask ).bounding.get_max() << " does not intersect the ray");
//              }
//            else
//              {
//                LOG( debug, "detected that node " << intern.left_index << " "
//                     << m_bvh->get_internal_node( intern.left_index ).bounding.get_min() << " -- "
//                     << m_bvh->get_internal_node( intern.left_index ).bounding.get_max() << " does not intersect the ray");
//              }
//          }

        bool overlapR = leafR ?
            m_triangles[ m_bvh->get_node( node.right_index ).element_index ].intersect( r, t2 )
          : m_bvh->get_node( node.right_index ).bounding.intersect( inv_r, t2 )
          ;

//        if( !overlapR )
//          {
//            if( leafR )
//              {
//                LOG( debug, "detected that leaf " << intern.right_index << " "
//                     << m_bvh->get_leaf_node( intern.right_index & bvh_leaf_index_mask ).bounding.get_min() << " -- "
//                     << m_bvh->get_leaf_node( intern.right_index & bvh_leaf_index_mask ).bounding.get_max() << " does not intersect the ray");
//              }
//            else
//              {
//                LOG( debug, "detected that node " << intern.right_index << " "
//                     << m_bvh->get_internal_node( intern.right_index ).bounding.get_min() << " -- "
//                     << m_bvh->get_internal_node( intern.right_index ).bounding.get_max() << " does not intersect the ray");
//              }
//          }

        if( overlapL && leafL && t1 <= distance_to_mesh )
          {
            closest_face_index = m_bvh->get_node( node.left_index ).element_index;
//            LOG( debug, "found candidate #" << closest_face_index <<" at distance = " << t1 << " in leaf node " << (intern.left_index & bvh_leaf_index_mask) );
            distance_to_mesh = std::min( distance_to_mesh, t1 );
            result = true;
          }
        if( overlapR && leafR && t2 <= distance_to_mesh )
          {
            closest_face_index = m_bvh->get_node( node.right_index ).element_index;
//            LOG( debug, "found candidate #" << closest_face_index <<" at distance = " << t2 << " in leaf node " << (intern.right_index & bvh_leaf_index_mask) );
            result = true;
            distance_to_mesh = std::min( distance_to_mesh, t2 );
          }
        bool traverseL = (overlapL && !leafL && t1 <= distance_to_mesh );
        bool traverseR = (overlapR && !leafR && t2 <= distance_to_mesh );
        if( !traverseL && !traverseR )
          {
//            stack.pop_back();
            while( stack.back().second > distance_to_mesh )
              stack.pop_back();
            node_index = stack.back().first;
            stack.pop_back();
//            LOG( debug, "going to node #" << node_index << ": " << m_bvh->get_internal_node( node_index ).bounding.get_min() << " -- " << m_bvh->get_internal_node( node_index ).bounding.get_max() );
          }
        else
          {
            if( traverseL && traverseR )
              {
                node_index = ( t1 < t2 ) ? node.left_index : node.right_index;
                stack.push_back( ( t1 < t2 ) ? std::make_pair( node.right_index, t2 ) : std::make_pair( node.left_index, t1 ) );
              }
            else
              node_index = (traverseL) ? node.left_index : node.right_index;
//            LOG( debug, "going to node #" << node_index << ": " << m_bvh->get_internal_node( node_index ).bounding.get_min() << " -- " << m_bvh->get_internal_node( node_index ).bounding.get_max() );
          }
      }
    while( node_index );
    return result;
  }

  void
  mesh_spatial_optimization::get_closest_vertex( const vec3& location, size_t& vertex_index, real& distance_to_vertex ) const
  {
    m_kdtree.knnSearch( &location.x, 1, &vertex_index, &distance_to_vertex );
  }

  bool
  mesh_spatial_optimization::contain( const vec3& p ) const
  {
    real distance_to_mesh = 0;
    size_t closest_vertex_index = 0;
    get_closest_vertex( p, closest_vertex_index, distance_to_mesh );

    //fixme: check if vfit == vfitend to throw an exception
    size_t closest_face_index = m_mesh.vf_begin( mesh::VertexHandle( closest_vertex_index ) )->idx();

    vec3 target = m_triangles[ closest_face_index ].get_vertex( triangle::vertex_index::V0 );
    target += m_triangles[ closest_face_index ].get_vertex( triangle::vertex_index::V1 );
    target += m_triangles[ closest_face_index ].get_vertex( triangle::vertex_index::V2 );
    target *= real(1.0 / 3.0);

    vec3 temp = target;

    target -= p;
    distance_to_mesh = length( target );
    target *= real(1.0) / distance_to_mesh;
    ray r( p, target );

    // look for an intersection
    distance_to_mesh *= 1.05;

//    LOG( error, "checking point " << p << " --> " << temp );
    if( !intersect( r,  distance_to_mesh, closest_face_index ) )
      {
        LOG( error, "WTF? for " << p << " to " << temp );
      }
    auto normal = m_mesh.normal( mesh::FaceHandle( closest_face_index ) );
    return normal[0] * target[0] + normal[1] * target[1] + normal[2] * target[2] > 0;
  }

} END_GO_NAMESPACE
