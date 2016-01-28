/*  Created on: Jan 26, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/gl_helper.h>
# include <graphics-origin/application/shader_program.h>

# include <GL/glew.h>

BEGIN_GO_NAMESPACE
namespace application {

  static const shader_program::identifier
  shader_program::null_identifier = shader_program::identifier(-1);

  shader_program::shader_program( const std::list< std::string >& shader_sources )
    : m_source_filenames{ shader_sources }, m_program_id{ 0 }
  {
    reload();
  }

  shader_program::~shader_program()
  {
    glcheck(glDeleteProgram( m_program_id ));
  }

  void
  shader_program::load( const std::list< std::string >& shader_sources )
  {
    m_source_filenames = shader_sources;
    reload();
  }

  void
  shader_program::reload()
  {
    m_loaded = false;
  }

  static void
  shader_program::unbind()
  {
    glcheck(glUseProgram(0));
  }

  shader_program::identifier
  shader_program::get_uniform_location( const std::string& name ) const
  {
    auto search = m_uniforms.find( name );
    if( search != m_uniforms.end() )
      return search->second;
    return null_identifier;
  }

  shader_program::identifier
  shader_program::get_attribute_location( const std::string& name ) const
  {
    auto search = m_attributes.find( name );
    if( search != m_attributes.end() )
      return search->second;
    return null_identifier;
  }

  static const GLenum uniform_properties[3] = {
      GL_BLOCK_INDEX,
      GL_NAME_LENGTH,
      GL_LOCATION
  };

  static const GLenum attribute_properties[2] = {
      GL_NAME_LENGTH,
      GL_LOCATION
  };

  void
  shader_program::variables_introspection()
  {
    //Clean the maps
    m_uniforms.clear();
    m_attributes.clear();

    GLint values[3];

    GLint num_uniforms = 0;
    glcheck(glGetProgramInterfaceiv( m_program_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_uniforms));
    for(int unif = 0; unif < num_uniforms; ++unif)
    {
      glcheck(glGetProgramResourceiv( m_program_id, GL_UNIFORM, unif, 3, uniform_properties, 3, NULL, values));

      //Skip any uniforms that are in a block.
      if(values[0] != -1)
        continue;

      char* name = new char[values[1]];
      glcheck(glGetProgramResourceName(m_program_id, GL_UNIFORM, unif, values[1], NULL, &name[0]));
      m_uniforms.insert( {{name, values[2]}});
      delete[] name;
    }

    GLint num_attributes = 0;
    glcheck(glGetProgramInterfaceiv( m_program_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &num_attributes ));
    for( int att = 0; att < num_attributes; ++ att )
      {
        glcheck(glGetProgramResourceiv( m_program_id, GL_PROGRAM_INPUT, att, 2, attribute_properties, 2, NULL, values ));
        char* name = new char[values[0]];
        glcheck(glGetProgramResourceName(m_program_id, GL_PROGRAM_INPUT, att, values[0], NULL, &name[0]));
        m_attributes.insert( {{name, values[1] }} );
        delete[]name;
      }
  }

  struct gpu_type {
     gpu_type ( std::string ext, GLenum n );
     std::string extension;
     GLenum      name     ;
   };

   gpu_type :: gpu_type ( std::string ext, GLenum n )
       : extension ( ext ), name ( n )
   {}

   static const unsigned int NUMBER_OF_SHADER_TYPES = 6;
   static gpu_type gpu_file_extensions [ NUMBER_OF_SHADER_TYPES ] =
   {
     gpu_type ( ".cs",   GL_COMPUTE_SHADER         ),
     gpu_type ( ".vert", GL_VERTEX_SHADER          ),
     gpu_type ( ".frag", GL_FRAGMENT_SHADER        ),
     gpu_type ( ".geom", GL_GEOMETRY_SHADER        ),
     gpu_type ( ".tcs",  GL_TESS_CONTROL_SHADER    ),
     gpu_type ( ".tes",  GL_TESS_EVALUATION_SHADER )
   };

   static bool is_supported_extension( const std::string& ext, GLenum& type )
   {
     for( uint i = 0; i < NUMBER_OF_SHADER_TYPES; ++ i )
       {
         if ( ext == gpu_file_extensions [ i ].extension )
           {
             type = gpu_file_extensions [ i ].name;
             return true;
           }
       }
     return false;
   }

   static GLuint compile_shader ( const std::string& gpu_name, GLuint type )
   {
     // open the shader file
     std::ifstream gpu_file ( gpu_name.c_str() );
     if ( !gpu_file.is_open() )
       {
         LOG( error, "cannot open shader file " << gpu_name );
         return 0;
       }

     // create a new shader object
     GLuint shader = glCreateShader ( type );
     if ( !shader )
       {
         LOG( error, "error when creating shader [" << gpu_name << "]");
         return 0;
       }

     // load the shader source in one string
     std::stringstream gpu_data;
     gpu_data << gpu_file.rdbuf();
     gpu_file.close(); // no more needed, so close it now
     const std::string gpu_string = gpu_data.str();

     // set the source of the shader (as one big cstring)
     const char*  strShaderVar = gpu_string.c_str();
     GLint iShaderLen = gpu_string.size();
     glShaderSource ( shader, 1,(const GLchar**) &strShaderVar, (GLint*)&iShaderLen );

     // compile the shader
     glCompileShader ( shader );
     GLint result;
     glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
     if( GL_FALSE == result )
       {
         LOG( error, "shader [" << gpu_name << "] compilation failed!");
         GLint logLen;
         glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLen );
         if( logLen > 0 )
           {
             char*  log = (char* )malloc(logLen);
             GLsizei written;
             glGetShaderInfoLog( shader, logLen, &written, log);
             LOG( warning, "shader log:\n" << log );
             free(log);
           }
         glDeleteShader ( shader );
         return 0;
       }
     return shader;
  }

  void
  shader_program::bind()
  {
    if( !m_loaded )
      {
        if( m_program_id )
          glcheck(glDeleteShader(m_program_id));

        m_program_id = 0;
        bool valid = true;
        for( auto& filename : m_source_filenames )
          {
            if( !m_program_id )
              glcheck( m_program_id = glCreateProgram());
            GLenum type;
            if( !is_supported_extension( tools::get_extension( filename ), type ) )
              {
                LOG( error, "unknown filename extension: " << filename );
                valid = false;
                break;
              }
            else
              {
                GLuint shader = compile_shader( filename, type );
                if( shader )
                  {
                    glcheck(glAttachShader( m_program_id, shader ) );
                    glcheck(glDeleteShader( shader ));
                  }
                else
                  {
                    valid = false;
                    break;
                  }
              }
          }

        if( valid && m_program_id )
          {
            glcheck(glLinkProgram( m_program_id ));
            GLint status;
            glcheck(lGetProgramiv( m_program_id, GL_LINK_STATUS, &status ));
            if( GL_FALSE == status )
              {
                LOG( error, "failed to link shader program " << m_program_id );
                GLint logLen;
                glGetProgramiv( m_program_id, GL_INFO_LOG_LENGTH, &logLen);
                if( logLen > 0 )
                  {
                    char * log = (char *)malloc(logLen);
                    GLsizei written;
                    glGetProgramInfoLog( m_program_id, logLen, &written, log);
                    LOG( error, "program log:\n" << log);
                    free(log);
                  }
                valid = false;
              }
          }
        if( !valid )
          {
            glcheck(glDeleteProgram( m_program_id ));
            m_program_id = 0;
          }

        m_loaded = true;
      }

    glcheck(glUseProgram( m_program_id ));
  }

}
END_GO_NAMESPACE
