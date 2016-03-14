/*  Created on: Mar 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/geometry/mesh.h>
# include <graphics-origin/geometry/box.h>
# include <graphics-origin/tools/filesystem.h>

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
    b = create_aabox_from_min_max( vec3{ minp[0], minp[1], minp[2]}, vec3{ maxp[0], maxp[1], maxp[2] } );
  }

  bool
  has_a_mesh_file_extension( const std::string& filename )
  {
    auto ext = graphics_origin::tools::get_extension( filename );
    return ext == obj_file_extension || ext == off_file_extension
        || ext == ply_file_extension;
  }

} END_GO_NAMESPACE
